#include "connackMsg.h"

mqtt::ConnAckMsg::ConnAckMsg(mqtt::ConnReturnCode returnCode)
    : returnCode(returnCode), Header(CONNACK) {}

std::vector<char> mqtt::ConnAckMsg::serialize() {

  remainingLength = 2;
  std::vector<char> msg = Header::serialize();

  msg.push_back(char(0x00));

  msg.push_back(char(returnCode));

  return msg;
}