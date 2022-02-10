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
char camSelect = 'B';
bool waitingForResp = false;


void setup() {
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  Wire.begin(44);        // join i2c bus (address optional for master)
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);  // start serial for output
  radioLink = new RFM96LoRALink();
  radioLink->enable();
  delay(5000);
  digitalWrite(4, LOW);
  lasttime = millis();

  
}

void loop() {
  if(length - transmitted > 0){
    //if there is more to transmit, continue transmitting
    
    if (received > 256 && received - transmitted > 0) {
      int num = min(received - transmitted, 128);
      Serial.write(data + transmitted, num);
      transmitted += num;
    }

    if(length - transmitted == 0){
      //Recieved the whole picture, carry on to the next request
      //TODO: shouldnt happen but what if for some reason we dont get the whole picture? Need this to be a little safer
      waitingForResp = false;
    }
  } else if (!waitingForResp) {
    //if we are done transmitting, request a new photo

    //Toggle which camera we want a picture from
    if(camSelect == 'A') {
      camSelect == 'B';
    } else {
      camSelect == 'A';
    }
    
    Wire.beginTransmission(44);
    Wire.write(camSelect);
    Wire.endTransmission();
    //indicate which camera we are interested in
    
    waitingForResp = true;
  } else {
    //we are waiting for a response after requesting a photo, do nothing?
  }


//  delay(50);
//  if (millis() - lasttime > 60000) {
//    digitalWrite(4, HIGH);
//  }
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
