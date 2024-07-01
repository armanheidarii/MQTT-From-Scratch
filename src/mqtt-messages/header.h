#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace mqtt {
enum MessageType {
  CONNECT = 1,
  CONNACK,
  PUBLISH,
  PUBACK,
  PUBREC,
  PUBREL,
  PUBCOMP,
  SUBSCRIBE,
  SUBACK,
  UNSUBSCRIBE,
  UNSUBACK,
  PINGREQ,
  PINGRESP,
  DISCONNECT
};

enum QosLevel { AtMostOnce = 0x00, AtLeastOnce = 0x01, ExactlyOnce = 0x02 };

using MsgIterator = std::vector<char>::const_iterator;

class Header {
protected:
  MessageType messageType;
  bool duplicate;
  QosLevel qosLevel;
  bool retain;
  int remainingLength = 0;

  const std::string delimiter = "  ";

public:
  Header() = default;

  Header(const Header& header);

  Header(MessageType messageType, bool duplicate = false,
         QosLevel qosLevel = AtMostOnce, bool retain = false,
         int remainingLength = 0);

  bool getRetain() const;

  MessageType getMessageType() const;

  int getRemainingLength() const;

  QosLevel getQosLevel() const;

  int deserialize(const std::vector<char>& msg);

  std::vector<char> serialize() const;

  friend std::ostream& operator<<(std::ostream& os, const MessageType& rhs);
  friend std::ostream& operator<<(std::ostream& os, const QosLevel& rhs);
  friend std::ostream& operator<<(std::ostream& os, const Header& rhs);
};
} // namespace mqtt