#include "broker.h"

mqtt::Broker::Broker(const int port) : port(port) {}

int mqtt::Broker::handleConnect(const mqtt::Header& header,
                                const std::vector<char>& remainingBytes,
                                Socket* socket) {
  mqtt::ConnMsg connMsg(header);

  if (!connMsg.deserialize(remainingBytes)) {
    std::cerr << "Failed to deserialize Connection Message\n";
    return 0;
  }

  std::cout << connMsg << "\n";

  mqtt::ConnAckMsg connAckMsg(mqtt::ConnectionAccepted);

  std::cout << connAckMsg << "\n";

  socket->send(connAckMsg.serialize());

  return 1;
}

int mqtt::Broker::handlePingReq(const mqtt::Header& header, Socket* socket) {

  std::cout << header << "\n";

  mqtt::Header pingRespMsg(mqtt::PINGRESP);

  std::cout << pingRespMsg << "\n";

  std::vector<char> serializedMsg = pingRespMsg.serialize();

  socket->send(serializedMsg);

  return 1;
}

int mqtt::Broker::handleSubscribe(const mqtt::Header& header,
                                  const std::vector<char>& remainingBytes,
                                  Socket* socket) {

  if (!socket)
    return -1;

  mqtt::SubMsg subMsg(header);

  if (!subMsg.deserialize(remainingBytes)) {
    std::cerr << "Failed to deserialize Subscribe Message\n";
    return 0;
  }

  std::cout << subMsg << "\n";

  std::vector<Topic> topics = subMsg.getTopics();

  subs_mtx.lock();
  for (const auto& topic : topics)
    subscribers[topic.value].insert(socket);
  subs_mtx.unlock();

  mqtt::SubAckMsg subAckMsg(subMsg.getPacketId(), mqtt::SuccessQoS0);

  std::cout << subAckMsg << "\n";

  socket->send(subAckMsg.serialize());

  subs_mtx.lock();
  for (const auto& topic : topics) {
    if (last_publish.count(topic.value))
      socket->send(last_publish[topic.value]);
  }
  subs_mtx.unlock();

  return 1;
}

int mqtt::Broker::handleUnsubscribe(const mqtt::Header& header,
                                    const std::vector<char>& remainingBytes,
                                    Socket* socket) {

  mqtt::UnsubMsg unsubMsg(header);

  if (!unsubMsg.deserialize(remainingBytes)) {
    std::cerr << "Failed to deserialize Unsubscribe Message\n";
    return 0;
  }

  std::cout << unsubMsg << "\n";

  auto topics = unsubMsg.getTopics();

  subs_mtx.lock();
  for (const auto& topic : topics) {
    // std::set<Socket*>& topicSubs = subscribers[topic.value];

    if (subscribers[topic.value].contains(socket)) {
      subscribers[topic.value].erase(
          socket); // Remove socket from subscribing to the topic

      // If the set of subscribers for the topic is empty, remove the topic
      if (subscribers[topic.value].empty())
        subscribers.erase(topic.value);
    }
  }
  subs_mtx.unlock();

  return 1;
}

int mqtt::Broker::handlePublish(const mqtt::Header& header,
                                const std::vector<char>& remainingBytes,
                                Socket* socket) {

  if (header.getQosLevel() != mqtt::AtMostOnce) {
    std::cerr << "QoS level other than 0 is not supported yet for Publish "
                 "messages.\n";
    return 0;
  }

  mqtt::PublishMsg publishMsg(header);

  if (!publishMsg.deserialize(remainingBytes)) {
    std::cerr << "Failed to deserialize Publish Message\n";
    return 0;
  }

  std::cout << publishMsg << "\n";

  std::string topic = publishMsg.getTopic();

  subs_mtx.lock();
  std::set<Socket*> topicSubs = subscribers[topic];
  std::set<Socket*> wildcardSubs = subscribers["#"];

  // Also publish message to subscribers of the wildcard '#'
  topicSubs.merge(wildcardSubs);

  std::cout << "Subscribers: " << topicSubs.size() << "\n";
  subs_mtx.unlock();

  std::vector<char> fullMsg = header.serialize();

  fullMsg.insert(fullMsg.end(), remainingBytes.begin(), remainingBytes.end());

  if (header.getRetain()) {
    last_publish[publishMsg.getTopic()] = fullMsg;
  }

  for (const auto& sub : topicSubs) {
    if (sub)
      sub->send(fullMsg);
  }

  return 1;
}

int mqtt::Broker::handleDisconnect(const mqtt::Header& header, Socket* socket) {

  std::cout << header << "\n";

  removeSubs(socket);

  if (socket)
    socket->close();

  return 1;
}

void mqtt::Broker::removeSubs(Socket* socket) {

  for (auto& [key, sockets] : subscribers) {

    if (sockets.contains(socket)) {
      sockets.erase(socket); // Remove socket from subscribing to the topic

      // If the set of subscribers for the topic is empty, remove the topic
      if (subscribers[key].empty())
        subscribers.erase(key);
    }
  }
}

int mqtt::Broker::handleClient(Socket* socket) {

  while (true) {
    std::vector<char> bytes = socket->receive(2);

    if (bytes.empty()) {
      return 0;
    }

    mqtt::Header header;
    if (!header.deserialize(bytes)) {
      std::cerr << "Failed to deserialize Disconnect Message\n";
      continue;
    }

    std::vector<char> remainingBytes =
        socket->receive(header.getRemainingLength());

    mqtt::MessageType msgType = header.getMessageType();

    switch (msgType) {
    case mqtt::CONNECT:
      if (!handleConnect(header, remainingBytes, socket))
        std::cerr << "Failed to handle connect.\n";
      break;
    case mqtt::PUBLISH:
      if (!handlePublish(header, remainingBytes, socket))
        std::cerr << "Failed to handle publish.\n";
      break;
    case mqtt::PINGREQ:
      if (!handlePingReq(header, socket))
        std::cerr << "Failed to handle ping request.\n";
      break;
    case mqtt::SUBSCRIBE:
      if (!handleSubscribe(header, remainingBytes, socket))
        std::cerr << "Failed to handle subscribe.\n";
      break;
    case mqtt::UNSUBSCRIBE:
      if (!handleUnsubscribe(header, remainingBytes, socket))
        std::cerr << "Failed to handle unsubscribe.\n";
      break;
    case mqtt::DISCONNECT:
      handleDisconnect(header, socket);
      delete socket;
      return 1;
    default:
      std::cout << "Message type " << msgType << " not supported yet.\n";
      removeSubs(socket);
      delete socket;
      return 0;
    }
  }
}

int mqtt::Broker::start() {

  while (true) {
    Socket* socket = new Socket(port);

    if (!socket->connect()) {
      std::cout << "failed to connect to incoming connection\n";
      delete socket;
      continue;
    }

    std::thread clientThread(&Broker::handleClient, this, socket);
    clientThread.detach();
  }
}