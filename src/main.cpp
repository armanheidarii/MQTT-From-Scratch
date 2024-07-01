#include "broker.h"

int main() {
  const int port = 1885;
  mqtt::Broker broker(port);
  broker.start();
  return 0;
}