#include <SBUDNIC.h>

LinkInterface* radioLink;

void setup() {
  // put your setup code here, to run once:
  delay(5000);
  radioLink = new RFM96LoRALink();
}

void loop() {
  // put your main code here, to run repeatedly:
  byte data[135] = {0};
  radioLink->enable();  
  radioLink->transmit(data, 135);
  delay(1000);
  radioLink->disable();  
  delay(5000);
}
