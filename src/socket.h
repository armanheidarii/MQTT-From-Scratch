#pragma once

#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class Socket {
public:
  Socket(const int port);

  int connect();

  std::vector<char> receive(int byteCount);

  std::vector<char> receive();

  void send(const std::vector<char>& msg);

  void close();

private:
  int port;
  int sockfd;
  int newsockfd;
  sockaddr_in addr_in;
  sockaddr* addr;
  int addrlen;
};