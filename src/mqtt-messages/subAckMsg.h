#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

#include "header.h"

namespace mqtt {
enum SubReturnCode {
  SuccessQoS0 = 0x00,
  SuccessQoS1 = 0x01,
  SuccessQoS2 = 0x02,
  Failure = 0x80
};

class SubAckMsg : public Header {
protected:
  uint16_t packetId;
  SubReturnCode returnCode;

public:
  SubAckMsg(uint16_t packetId, SubReturnCode returnCode);

  std::vector<char> serialize();

  friend std::ostream& operator<<(std::ostream& os, const SubAckMsg& rhs);
  friend std::ostream& operator<<(std::ostream& os, const SubReturnCode& rhs);
};
} // namespace mqtt