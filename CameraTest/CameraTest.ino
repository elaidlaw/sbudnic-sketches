#include <SBUDNIC.h>

Camera* camera0;

void setup() {
  Serial.begin(9600);
  delay(5000);
  camera0 = new Camera(0);
}

void loop() {
//  Serial.println("enabling");
  delay(1500);
  camera0->enable();
  delay(1500);
//  Serial.println("enabled");

  uint32_t picLen = camera0->takePicture();
  uint8_t pckts[10000];
  memset(pckts, 0, sizeof(uint8_t) * 10000);
  int numPackets = camera0->readData(pckts, 10000);
  for (int i = 0; i < 256 * numPackets; i++) {
    Serial.write(pckts[i]);
//    if (i % 256 == 0) {
//      Serial.println();
//    }
  }
  delay(100000);

  camera0->disable();
}
