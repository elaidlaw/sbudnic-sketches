/*
   RadioLib SX127x Receive Example
   This example listens for LoRa transmissions using SX127x Lora modules.
   To successfully receive data, the following settings have to be the same
   on both transmitter and receiver:
    - carrier frequency
    - bandwidth
    - spreading factor
    - coding rate
    - sync word
    - preamble length
   Other modules from SX127x/RFM9x family can also be used.
   For default module settings, see the wiki page
   https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx127xrfm9x---lora-modem
   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// SX1278 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
SX1278 radio = new Module(10, 2, 9, 3);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1278 radio = RadioShield.ModuleA;

void setup() {
  Serial.begin(500000);

  // initialize SX1278 with default settings
  int state = radio.begin();
  radio.setFrequency(435.0);
}

void loop() {
  //Serial.println("LOOP");
  byte byteArr[256] = {};
  byte charArr[256] = {};
  
  int stateRec = radio.receive(byteArr, 256);

  int stateSend = 0;
  
  if (Serial.available() != 0) {
    String mydata = Serial.readString();
    mydata.trim();
    //Serial.println(mydata);
    if (mydata.length() >= 2) {
     //Serial.println("SENDING COMMAND");
     memset(byteArr, 0, 256);
     String newString = "GS" + mydata.substring(0,2) + "SBK" + "Sending Command: " + mydata;

    int n = newString.length();
 
    // declaring character array
    char char_array[n + 1];
 
    // copying the contents of the
    // string to char array
    strcpy(char_array, newString.c_str());
     
     //Serial.println(newString);
     //newString.toCharArray(charArr, newString.length());
     sprintf(byteArr, "%s", char_array);
     Serial.write(byteArr, 256);
     Serial.println("");
     int stateSend = radio.transmit(mydata);
    }
  }

  if (stateRec == RADIOLIB_ERR_NONE) {
    Serial.write(byteArr, 256);
    Serial.println();
  }
}
