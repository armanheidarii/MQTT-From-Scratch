#include "socket.h"
#include <stdio.h>
#include <cstring>

Socket::Socket(const int port) : port(port) {}

int Socket::connect() {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Creating socket file descriptor
  if (sockfd == 0) {
    std::cerr << "socket failed\n";
    return -1;
  }

  int use_opt = 1;

  // Attach to port even if it is already in use
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &use_opt, sizeof(use_opt))) {
    std::cerr << "setsockopt failed\n";
    return -1;
  }

  addrlen = sizeof(addr_in);
  addr_in.sin_family = AF_INET;
  addr_in.sin_addr.s_addr = INADDR_ANY;
  addr_in.sin_port = htons(port);

  addr = reinterpret_cast<sockaddr*>(&addr_in);

  // Forcefully attaching socket to the port
  if (bind(sockfd, addr, addrlen) < 0) {
    std::cerr << "bind failed\n";
    return -1;
  }

  if (listen(sockfd, 10) < 0) {
    std::cerr << "listen failed\n";
    return -1;
  }

  newsockfd = accept(sockfd, addr, reinterpret_cast<socklen_t*>(&addrlen));
  ::close(sockfd);

  if (newsockfd < 0) {
    std::cerr << "accept failed\n";
    return -1;
  }

  return 1;
}

std::vector<char> Socket::receive(int byteCount) {
  char msg[byteCount];

  // Receive a response into a fixed size style char array
  int readByteCount = read(newsockfd, msg, byteCount);

  if (readByteCount < 0) {
    std::cerr << "Failed to read bytes, error: " << strerror(errno) << "\n";
    return std::vector<char>();
  }

  // Convert the char array to a vector that is as long as the number of read
  // bytes
  return std::vector<char>(msg, msg + byteCount);
}

std::vector<char> Socket::receive() {
  int msgSize = 2048;

  return receive(msgSize);
}

void Socket::send(const std::vector<char>& msg) {
  ::send(newsockfd, msg.data(), msg.size(), 0);
}

void Socket::close() { ::close(newsockfd); }