#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

const int CS1 = 7;
const int CS2 = 9;
bool is_header = false;
int mode = 0;
uint8_t start_capture = 0;
uint32_t length;
bool sentLength = false;

ArduCAM myCAM1(OV5642, CS1);
ArduCAM myCAM2(OV5642, CS2);

void setup() {
  Wire.begin();
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  Serial.println(F("ACK CMD ArduCAM Start! END"));
  // set the CS as an output:
  pinMode(CS1, OUTPUT);
  digitalWrite(CS1, HIGH);
  pinMode(CS2, OUTPUT);
  digitalWrite(CS2, HIGH);
  // initialize SPI:
  SPI.begin();
   
  //Reset the CPLD
  myCAM1.write_reg(0x07, 0x80);
  delay(100);
  myCAM1.write_reg(0x07, 0x00);
  delay(100);

  myCAM2.write_reg(0x07, 0x80);
  delay(100);
  myCAM2.write_reg(0x07, 0x00);
  delay(100);

  uint8_t vid, pid;
  uint8_t temp;
  
  while (1) {
    //Check if the ArduCAM SPI bus is OK
    myCAM1.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM1.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55)
    {
      Serial.println(F("ACK CMD SPI1 interface Error! END"));
      delay(1000);continue;
    }else{
      Serial.println(F("ACK CMD SPI1 interface OK. END"));break;
    }
  }
  while (1) {
    //Check if the ArduCAM SPI bus is OK
    myCAM2.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM2.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55)
    {
      Serial.println(F("ACK CMD SPI2 interface Error! END"));
      delay(1000);continue;
    }else{
      Serial.println(F("ACK CMD SPI2 interface OK. END"));break;
    }
  }
  while (1) {
    Serial.println("check");
    //Check if the camera module type is OV5642
    myCAM1.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
    myCAM1.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
    Serial.println("check2");
    if ((vid != 0x56) || (pid != 0x42)) {
      Serial.println(F("ACK CMD Can't find OV5642 module! END"));
      delay(1000);continue;
    }else{
      Serial.println(F("ACK CMD OV5642 detected. END"));break;      
    }
  }
  //Change to JPEG capture mode and initialize the OV5642 module
  myCAM1.set_format(JPEG);
  myCAM1.InitCAM();
  myCAM1.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  myCAM2.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  myCAM1.clear_fifo_flag();
  myCAM1.write_reg(ARDUCHIP_FRAMES, 0x00);
  myCAM2.write_reg(ARDUCHIP_FRAMES, 0x00);
  myCAM1.clear_fifo_flag();
  myCAM2.clear_fifo_flag();
  delay(2000);
  Serial.println("Initialization complete");
}

void loop() {
  Serial.println("starting capture 1");

  myCAM1.flush_fifo(); 
  myCAM1.clear_fifo_flag();   
  myCAM1.start_capture();
  while(!myCAM1.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  Serial.println("finished capture");
  length = myCAM1.read_fifo_length();
  Serial.println(length);
  myCAM1.CS_LOW();
  myCAM1.set_fifo_burst();
  Serial.println("burst");
  delay(2000);
  length--;
  uint8_t temp,temp_last;
  bool done = false;
  while (length > 0 && !done) {
    int counter = 0;
    Wire.beginTransmission(44);
    Serial.println("begun");
    while( length-- && counter < 32) {
      temp_last = temp;
      temp =  SPI.transfer(0x00);//read a byte from spi
      if(is_header == true) {
        Serial.write(temp);
        Wire.write(temp);
        counter += 1;
      }
      else if((temp == 0xD8) & (temp_last == 0xFF)) {
        Serial.println(F("ACK IMG END"));
        is_header = true;
        Serial.write(temp);
        Wire.write(temp_last);
        Wire.write(temp);
        counter += 2;
      }
      if( (temp == 0xD9) && (temp_last == 0xFF) ) {
        done = true;
        break;
      }
      delayMicroseconds(15);
    }
    Wire.endTransmission();
  }
  
  myCAM1.CS_HIGH();
  delay(10000);
  Serial.println("starting capture 2");

  myCAM2.flush_fifo(); 
  myCAM2.clear_fifo_flag();   
  myCAM2.start_capture();
  while(!myCAM2.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  Serial.println("finished capture");
  length = myCAM2.read_fifo_length();
  Serial.println(length);
  char lengthStr[8];
  sprintf(lengthStr, "LS%06d", length);
  myCAM2.CS_LOW();
  myCAM2.set_fifo_burst();
  Serial.println("burst");
  delay(2000);
  length--;
  Wire.beginTransmission(44);
  Wire.write(lengthStr);
  Wire.endTransmission();
  done = false;
  while (length > 0 && !done) {
    int counter = 0;
    Wire.beginTransmission(44);
    Serial.println("begun");
    while( length-- && counter < 32) {
      temp_last = temp;
      temp =  SPI.transfer(0x00);//read a byte from spi
      if(is_header == true) {
        Serial.write(temp);
        Wire.write(temp);
        counter += 1;
      }
      else if((temp == 0xD8) & (temp_last == 0xFF)) {
        Serial.println(F("ACK IMG END"));
        is_header = true;
        Serial.write(temp);
        Wire.write(temp_last);
        Wire.write(temp);
        counter += 2;
      }
      if( (temp == 0xD9) && (temp_last == 0xFF) ) {
        done = true;
        break;
      }
      delayMicroseconds(15);
    }
    Wire.endTransmission();
  }
  
  myCAM2.CS_HIGH();
  delay(10000);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Serial.println("request");
  if (length == 0) {
    return;
  }
  if (!sentLength) {
    char out[6];
    sprintf(out, "%06d", length);
    Wire.write(out); // respond with message of 6 bytes
    sentLength = true;
  } else {
    int counter = 0;
    
  }
  
}
