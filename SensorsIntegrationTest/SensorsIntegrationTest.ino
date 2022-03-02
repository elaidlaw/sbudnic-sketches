#include <SBUDNIC.h>
#include <Wire.h>

#define PREFIX_LENGTH 7
#define DATA_LENGTH 200

ReceiverInterface* receiver;
LinkInterface* radioLink;
SensorsInterface* sensors;
uint8_t data[50000];
uint8_t packet[256];

int photoId = 0;
int telemetryId = 0;

// Low power configuration
int main(void) {
  init();
  initVariant();
  
  NRF_UART0->TASKS_STOPTX = 1;
  NRF_UART0->TASKS_STOPRX = 1;
  NRF_UART0->ENABLE = 0;
  
  *(volatile uint32_t *)0x40002FFC = 0;
  *(volatile uint32_t *)0x40002FFC;
  *(volatile uint32_t *)0x40002FFC = 1;
  setup();
  for(;;){
    loop();
  }
  return 0;
}

void setup() {
  digitalWrite(LED_PWR, LOW);
  digitalWrite(PIN_ENABLE_SENSORS_3V3, HIGH);
  digitalWrite(PIN_ENABLE_I2C_PULLUP, HIGH);
  WDT::enable();
  WDT::reload();
  Serial.begin(9600);
  receiver = new Receiver(data, 50000);
  receiver->disable();
  WDT::wddelay(5000);
  Wire.begin(44);
  Wire.onReceive(Receiver::receive_data);
  Wire.onRequest(Receiver::send_settings);

  WDT::reload();
  radioLink = new RFM96LoRALink();
  WDT::reload();
  sensors = new Sensors();
  WDT::reload();

  radioLink->enable();
  LinkProtocol::getRestartPacket(packet);
  WDT::reload();
  radioLink->transmit(packet, HEADER_LENGTH);
  WDT::reload();
  radioLink->disable();
  WDT::reload();
  
}

void loop() {
  getAndTransmitPhoto();
  getAndTransmitTelemetry();
  WDT::wddelay(10000);
  
  
}

void getAndTransmitPhoto() {
  WDT::reload();
  receiver->enable();
  long photoStart = millis();
  while (receiver->get_state() == REC_WAIT && millis() - photoStart < 60000) {
    WDT::reload();
    delay(2000);
  }
  receiver->disable();
  WDT::reload();
  int numImagePackets = receiver->get_output_pos() / PHOTO_BODY_LENGTH;
  LinkProtocol::getPhotoStart(packet, photoId, numImagePackets, Receiver::useCam1 ? 1 : 0);
  radioLink->enable();
  radioLink->transmit(packet, HEADER_LENGTH);
  for (int i = 0; i < numImagePackets; i++) {
    WDT::reload();
    LinkProtocol::getPhotoPacket(packet, photoId, i, data + i * PHOTO_BODY_LENGTH);
    radioLink->transmit(packet, HEADER_LENGTH + PHOTO_BODY_LENGTH);
  }
  WDT::reload();
  LinkProtocol::getPhotoEnd(packet, photoId, numImagePackets, Receiver::useCam1 ? 1 : 0);
  delay(100);
  radioLink->transmit(packet, HEADER_LENGTH);
  WDT::reload();
  radioLink->disable();
  WDT::reload();
  photoId++;
  photoId = photoId % 10;
  Receiver::useCam1 = !Receiver::useCam1;
  WDT::reload();
}


void getAndTransmitTelemetry() {
  WDT::reload();
  sensors->enable();
  delay(1000);
  WDT::reload();
  char sensorData[DATA_LENGTH] = {0};
  sensors->readAllSensors(sensorData);
  WDT::reload();
  delay(1000);
  sensors->readAllSensors(sensorData);
  WDT::reload();
  sensors->disable();
  WDT::reload();
  radioLink->enable();
  WDT::reload();
  LinkProtocol::getTelemetryPacket(packet, 0, (uint8_t*)sensorData);
  radioLink->transmit(packet, HEADER_LENGTH + TELEMETRY_BODY_LENGTH);
  WDT::reload();
  radioLink->disable();
  WDT::reload();
}
