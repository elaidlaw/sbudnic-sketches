// Wire Controller Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI peripheral device
// Refer to the "Wire Peripheral Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#include <SBUDNIC.h>
LinkInterface* radioLink;

byte data[20000];
int length = 0;
int received = 0;
int transmitted = 0;
long lasttime = 0;

byte camToUse = 1;


void setup() {
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  Wire.begin(44);        // join i2c bus (address optional for master)
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);  // start serial for output
  radioLink = new RFM96LoRALink();
  delay(5000);
  radioLink->enable();
  digitalWrite(6, LOW);
  lasttime = millis();
}

void loop() {
  if (received > 256 && received - transmitted > 0) {
    int num = min(received - transmitted, 128);
    Serial.println("transmitting");
    radioLink->transmit(data + transmitted, num);
    transmitted += num;
  }
  delay(50);
  if (millis() - lasttime > 60000) {
    digitalWrite(6, HIGH);
  }
}

void receiveEvent(int howMany)
{
  char in[howMany];
  for (int i = 0; i < howMany; i++) {
    in[i] = Wire.read();
  }
  if (in[0] == 'L' && in[1] == 'S') {
    length = atoi(in + 2);
    received = 0;
    transmitted = 0;
    return;
  }
  if (in[0] == 'L' && in[1] == 'E') {
    length = 0;
  }
  memcpy(data + received, in, howMany);
  received += howMany;
}

void requestEvent() {
  Wire.write(camToUse);
}
