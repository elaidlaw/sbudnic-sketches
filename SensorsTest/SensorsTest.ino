#include <SBUDNIC.h>

SensorsInterface* sensors;

void setup() {
  // create and enable the sensors interface
  sensors = new Sensors();
  sensors->enable();
}

void loop() {
  // put your main code here, to run repeatedly:
  char data[100];
  sensors->readAllSensors(data);
  Serial.println(data);
  delay(1000);
}
