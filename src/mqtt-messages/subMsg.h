#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

#include "header.h"

namespace mqtt {
struct Topic {
  int length;
  std::string value = "";
};
class SubMsg : public Header {
protected:
  uint16_t packetId;

  std::vector<Topic> topics = {};

public:
  SubMsg(const Header& header);

  uint16_t getPacketId();

  std::vector<Topic> getTopics();

  int deserialize(const std::vector<char>& remainingBytes);

  friend std::ostream& operator<<(std::ostream& os, const SubMsg& rhs);
};
} // namespace mqtt