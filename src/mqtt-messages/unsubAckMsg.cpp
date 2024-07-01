#include "unsubAckMsg.h"

std::vector<char> mqtt::UnsubAckMsg::serialize() {
  remainingLength = 2;
  std::vector<char> msg = mqtt::Header::serialize();

  msg.push_back(char(packetId >> 8));
  msg.push_back(char(packetId & 0xff));

  return msg;
}

std::ostream& mqtt::operator<<(std::ostream& os, const mqtt::UnsubAckMsg& rhs) {
  os << static_cast<Header>(rhs) << "\n";

  os << "----Unsubscribe Ack Message----\n";
  os << "Packet Id: " << rhs.packetId << "\n";
  return os;
}
