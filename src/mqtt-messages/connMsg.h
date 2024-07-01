#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "header.h"

namespace mqtt {
class ConnMsg : public Header {
public:
  ConnMsg(const Header& header);

  int remainingLength;
  int nameLength;
  std::string protocolName = "";
  int version;

  bool cleanSession;

  int keepAlive;

  int clientIdLength;
  std::string clientId = "";

  int deserialize(const std::vector<char>& remainingBytes);

  friend std::ostream& operator<<(std::ostream& os, const ConnMsg& rhs);
};
} // namespace mqtt