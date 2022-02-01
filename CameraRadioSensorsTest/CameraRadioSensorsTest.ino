#include <SBUDNIC.h>

#define PREFIX_LENGTH 7
#define DATA_LENGTH 128

CameraInterface* camera0;
LinkInterface* radioLink;
SensorsInterface* sensors;

int photoId = 0;
int telemetryId = 0;

void setup() {
  Serial.begin(9600);
  delay(5000);
  camera0 = new Camera(0);
  radioLink = new RFM96LoRALink();
//  sensors = new Sensors();
}

void transmitWithPrefix(char* prefix, void* data, int dataLength) {
  uint8_t packet[DATA_LENGTH + PREFIX_LENGTH] = {0};
  memcpy(packet, prefix, PREFIX_LENGTH);
  memcpy(packet + PREFIX_LENGTH, data, dataLength);
  radioLink->transmit(packet, DATA_LENGTH + PREFIX_LENGTH);
}

void takeAndTransmitPhoto() {
  camera0->enable();
  uint32_t picLen = camera0->takePicture();
  if (picLen > 10000) {
    return;
  }
  uint8_t pckts[10000] = {0};
//  int numPackets = camera0->readData(pckts, 10000, photoId);
  int numPackets = 1;
  camera0->disable();
  delay(5000);
  radioLink->enable();
  char packetPrefix[10];

  sprintf(packetPrefix, "C%02dS%03d", photoId, numPackets * 2);
  
  transmitWithPrefix(packetPrefix, 0, 0);
  delay(100);
  for (int i = 0; i < numPackets * 2; i++) {
    sprintf(packetPrefix, "C%02dD%03d", photoId, i);
    transmitWithPrefix(packetPrefix, pckts + i * 128, 128);
    delay(100);
  }
  sprintf(packetPrefix, "C%02dE%03d", photoId, numPackets * 2);
  transmitWithPrefix(packetPrefix, 0, 0);
  delay(100);
  radioLink->disable();
  photoId++;
  if (photoId == 100) {
    photoId = 0;
  }
}

void getAndTransmitTelemetry() {
  sensors->enable();
  char sensorData[DATA_LENGTH] = {0};
  sensors->readAllSensors(sensorData);
  sensors->disable();

  char packetPrefix[10];

  sprintf(packetPrefix, "T%06d", telemetryId);
  radioLink->enable();
  delay(100);
  transmitWithPrefix(packetPrefix, sensorData, DATA_LENGTH);
  delay(100);
  radioLink->disable();
  telemetryId++;
}

void loop() {
  takeAndTransmitPhoto();
  delay(10000);
//  for (int i = 0; i < 100; i++) {
//    getAndTransmitTelemetry();
//    delay(500);
//  }
}
