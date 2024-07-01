#include <cstdint>
#include <iostream>
#include <vector>

#include "header.h"
#include "subMsg.h" //mqtt::Topic

namespace mqtt {
class UnsubMsg : public Header {
protected:
  uint16_t packetId;

  std::vector<Topic> topics;

public:
  UnsubMsg(const Header& header);

  std::vector<Topic> getTopics();

  int deserialize(const std::vector<char>& remainingBytes);

  friend std::ostream& operator<<(std::ostream& os, const UnsubMsg& rhs);
};
} // namespace mqtt