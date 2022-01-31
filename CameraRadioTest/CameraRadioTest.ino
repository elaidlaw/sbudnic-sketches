#include <SBUDNIC.h>

CameraInterface* camera0;
LinkInterface* radioLink;
SensorsInterface* sensors;

void setup() {
  Serial.begin(9600);
  delay(5000);
  camera0 = new Camera(0);
  radioLink = new RFM96LoRALink();
  sensors = new Sensors();
}

void loop() {
  camera0->enable();
  uint32_t picLen = camera0->takePicture();
  uint8_t pckts[10000];
  memset(pckts, 0, sizeof(uint8_t) * 10000);
  int numPackets = camera0->readData(pckts, 10000);
  camera0->disable();
  radioLink->enable();
  Serial.print("transmitting ");
  Serial.print(numPackets);
  Serial.println(" packets");
  radioLink->transmit("Starting picture downlink");
  delay(100);
  for (int i = 0; i < numPackets * 256; i += 128) {
    Serial.println(radioLink->transmit(pckts + i, 128));
    delay(100);
  }
  radioLink->transmit("Done with picture downlink");
  delay(100);
  radioLink->disable();
  delay(10000);
}
