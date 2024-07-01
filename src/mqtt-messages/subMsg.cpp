#include "subMsg.h"

mqtt::SubMsg::SubMsg(const mqtt::Header& header) : mqtt::Header(header) {}

uint16_t mqtt::SubMsg::getPacketId() { return packetId; }

std::vector<mqtt::Topic> mqtt::SubMsg::getTopics() { return topics; }

int mqtt::SubMsg::deserialize(const std::vector<char>& remainingBytes) {

  packetId = static_cast<uint16_t>((remainingBytes[0] << 8) |
                                   (remainingBytes[1] & 0x00ff));

  mqtt::MsgIterator msgIt = remainingBytes.begin() + 2;

  while (msgIt < remainingBytes.begin() + remainingLength) {
    Topic topic;

    topic.length = static_cast<int>((*msgIt << 8) | (*(msgIt + 1) & 0x00ff));
    msgIt += 2;

    topic.value = "";

    mqtt::MsgIterator topicStartIt = msgIt;
    for (; msgIt < topicStartIt + topic.length; msgIt++) {
      topic.value += *msgIt;
    }

    topics.push_back(topic);
    msgIt++;
  }

  return 1;
}

std::ostream& mqtt::operator<<(std::ostream& os, const mqtt::SubMsg& rhs) {
  os << static_cast<Header>(rhs) << "\n";

  os << "----Subscribe Message----\n";
  os << "Packet ID: " << rhs.packetId << "\n";
  os << "------Topics:\n";
  for (const auto& topic : rhs.topics) {
    os << "Topic Length: " << topic.length << rhs.delimiter;
    os << "Topic: " << topic.value << "\n";
  }

  return os;
}