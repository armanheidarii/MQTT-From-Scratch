#include "publishMsg.h"

mqtt::PublishMsg::PublishMsg(const Header& header) : mqtt::Header(header) {}

std::ostream& mqtt::operator<<(std::ostream& os, const mqtt::PublishMsg& rhs) {
  os << static_cast<mqtt::Header>(rhs) << "\n";

  os << "----Publish Message---\n";
  os << "Topic Length: " << rhs.topicLength << rhs.delimiter;
  os << "Topic: " << rhs.topic << rhs.delimiter;

  return os;
}

int mqtt::PublishMsg::deserialize(std::vector<char> remainingBytes) {
  topicLength =
      static_cast<int>((remainingBytes[0] << 8) | (remainingBytes[1] & 0x00ff));

  MsgIterator msgIt = remainingBytes.begin() + 2;

  const auto topicStartIt = msgIt;

  for (; msgIt < topicStartIt + topicLength; msgIt++) {
    topic += *msgIt;
  }

  packetId = static_cast<uint16_t>((*msgIt << 8) | (*(msgIt + 1) & 0x00ff));

  return 1;
}

std::string mqtt::PublishMsg::getTopic() { return topic; }
