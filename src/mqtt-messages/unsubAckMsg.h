#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

#include "header.h"

namespace mqtt {
class UnsubAckMsg : public Header {
protected:
  uint16_t packetId;

public:
  std::vector<char> serialize();

  friend std::ostream& operator<<(std::ostream& os, const UnsubAckMsg& rhs);
};
} // namespace mqtt