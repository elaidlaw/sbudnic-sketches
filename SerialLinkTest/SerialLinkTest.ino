#include <SBUDNIC.h>

LinkInterface* link;

void setup() {
  // create and enable the link
  link = new SerialLink();
  link->enable();
}

void loop() {
  // try receiving for 1000ms
  int received = link->receive(1000);
  if (received == 0) {
    // if no command was received, transmit saying that
    link->transmit("nothing received", 30);
  } else {
    // if a command was received, transmit with the received command
    char msg[20];
    sprintf(msg, "received %d", received);
    link->transmit(msg, 20);
  }
}
