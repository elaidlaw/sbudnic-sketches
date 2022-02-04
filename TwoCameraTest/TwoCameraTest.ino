#include <SBUDNIC.h>

CameraInterface* camera00;
//CameraInterface* camera10;
bool cam0 = true;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(500);
  camera00 = new Camera(0);
//  camera10 = new Camera(1);

}

void loop() {
//  Serial.println("enabling");
//  if(cam0){

    if (Serial.available()) {
      char cmd = Serial.read();
      
      if (cmd == 'a') {
        camera00->enable();
        delay(500);
        uint32_t picLen = camera00->takePicture();
        uint8_t pckts[10000];
        memset(pckts, 0, sizeof(uint8_t) * 10000);
        int numPackets = camera00->readData(pckts, 10000, 1);
        for (int i = 0; i < 256 * numPackets; i++) {
          Serial.write(pckts[i]);
      //    if (i % 256 == 0) {
      //      Serial.println();
      //    }
        }
        delay(3000);
      }
    }
//    delay(1500);
//    camera00->enable();
//    delay(500);
//  //  Serial.println("enabled");
//
//    uint32_t picLen = camera00->takePicture();
//    uint8_t pckts[10000];
//    memset(pckts, 0, sizeof(uint8_t) * 10000);
//    int numPackets = camera00->readData(pckts, 10000, 1);
//    for (int i = 0; i < 256 * numPackets; i++) {
//      Serial.write(pckts[i]);
//  //    if (i % 256 == 0) {
//  //      Serial.println();
//  //    }
//    }
//    delay(3000);
//
//    camera00->disable();
//    cam0 = false;
//  } else {
//    
//    delay(1500);
//    camera10->enable();
//    delay(500);
//  //  Serial.println("enabled");
//  
//    uint32_t picLen = camera10->takePicture();
//    uint8_t pckts[10000];
//    memset(pckts, 0, sizeof(uint8_t) * 10000);
//    int numPackets = camera10->readData(pckts, 10000, 1);
//    for (int i = 0; i < 256 * numPackets; i++) {
//      Serial.write(pckts[i]);
//  //    if (i % 256 == 0) {
//  //      Serial.println();
//  //    }
//    }
//    delay(3000);
//  
//    camera10->disable();
//    cam0 = true;
//  }
  
}
