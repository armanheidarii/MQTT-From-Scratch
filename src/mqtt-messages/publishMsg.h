#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "header.h"

namespace mqtt {
class PublishMsg : public Header {
protected:
  int topicLength;
  std::string topic = "";
  uint16_t packetId;

public:
  PublishMsg(const Header& header);

  int deserialize(std::vector<char> remainingBytes);

  std::string getTopic();

  friend std::ostream& operator<<(std::ostream& os, const PublishMsg& rhs);
};
} // namespace mqtt