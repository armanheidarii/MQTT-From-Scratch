#pragma once

#include <iostream>
#include <vector>

#include "header.h"

namespace mqtt {
enum ConnReturnCode {
  ConnectionAccepted = 0x00,
  UnnacceptableVersion,
  IdentifierRejected,
  ServerUnavailable,
  BadUsrNameOrPwd,
  NotAuthorized
};
class ConnAckMsg : public Header {
private:
  ConnReturnCode returnCode;

public:
  ConnAckMsg(ConnReturnCode returnCode);

  std::vector<char> serialize();
};
} // namespace mqtt