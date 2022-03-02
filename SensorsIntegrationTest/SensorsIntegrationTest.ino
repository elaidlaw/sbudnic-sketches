#include <SBUDNIC.h>
#include <Wire.h>


#define PREFIX_LENGTH 7
#define DATA_LENGTH 128

RecieverInterface* recobj;
LinkInterface* radioLink;
SensorsInterface* sensors;
uint8_t data[10000];

int photoId = 0;
int telemetryId = 0;

void setup() {
  Serial.begin(9600);
  recobj = new Reciever(data, 10000);
  recobj->disable();
  delay(500);
  Wire.begin(44);
  Wire.onReceive(Reciever::recieve_data);
  Wire.onRequest(Reciever::send_settings);

  radioLink = new RFM96LoRALink();
  sensors = new Sensors();
}

void loop() {
  // put your main code here, to run repeatedly:
//  recobj->enable();
//  int num;
//  while((num = recobj->get_state()) == REC_WAIT){
//    Serial.println("waiting");
//    delay(500);
//  }
////  Serial.println(num);
//  recobj->disable();
////  Serial.println("picture done");
////  Serial.println();
//  int d = recobj->get_output_pos();
//  int z;
//  for(z=0;z<d;z++){
//    Serial.write(data[z]);
//    delay(10);
//  }
//  delay(2000);
//
//  radioLink->enable();
//  delay(1000);
//  radioLink->disable();
//  delay(1000);
////  Serial.println("radio done");

  sensors->enable();
  char sensorData[DATA_LENGTH] = {0};
  sensors->readAllSensors(sensorData);
  delay(1000);
//  sensorData[DATA_LENGTH] = {0};
//  sensors->readAllSensors(sensorData);
//  sensors->disable();
//  Serial.println("sensors done");
  int z;
  for(z=0; z<DATA_LENGTH;z++){
    Serial.print(sensorData[z]);
  }
  Serial.println();

//  delay(500);
  
}


//void getAndTransmitTelemetry() {
//  sensors->enable();
//  char sensorData[DATA_LENGTH] = {0};
//  sensors->readAllSensors(sensorData);
//  sensors->disable();
//
//  char packetPrefix[10];
//
//  sprintf(packetPrefix, "T%06d", telemetryId);
//  radioLink->enable();
//  delay(100);
//  transmitWithPrefix(packetPrefix, sensorData, DATA_LENGTH);
//  delay(100);
//  radioLink->disable();
//  telemetryId++;
//}
