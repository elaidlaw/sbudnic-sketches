#include <SBUDNIC.h>

LinkInterface* link;

void setup() {
  // put your setup code here, to run once:
  link = new RFM96Link();
  link->enable();  
}

void loop() {
  // put your main code here, to run repeatedly:
  link->transmit("hellossss", 20);
  delay(1000);
}
