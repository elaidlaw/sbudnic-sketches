#include <SBUDNIC.h>
#include <Wire.h>

#define PREFIX_LENGTH 7
#define DATA_LENGTH 200

ReceiverInterface* receiver;
LinkInterface* radioLink;
SensorsInterface* sensors;
DeploymentInterface* deployment;
uint8_t data[50000];
uint8_t packet[256];

#define TEMP_DATA_BUFFER_SIZE 1440
float tempSensor1Data[TEMP_DATA_BUFFER_SIZE];
float tempSensor2Data[TEMP_DATA_BUFFER_SIZE];
long tempSensorTimes[TEMP_DATA_BUFFER_SIZE];
int tempSensorHead = 0;

int photoId = 0;
int telemetryId = 0;
int loopCount = 0;

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
  Config::load();
  Config::data.restartCount += 1;
  Config::save();
  
  Serial.begin(9600);
  receiver = new Receiver(data, 50000);
  receiver->disable();
  WDT::wddelay(1000);
  Wire.begin(44);
  Wire.onReceive(Receiver::receive_data);
  Wire.onRequest(Receiver::send_settings);

  WDT::reload();
  radioLink = new RFM96LoRALink();
  WDT::reload();
  radioLink->disable();
  WDT::reload();
  sensors = new Sensors();
  WDT::reload();
  sensors->disable();
  WDT::reload();
  
  if (!Config::data.safeToOperate) {
    WDT::wddelay(30000);
    Config::data.safeToOperate = true;
    Config::save();
    deployment = new Deployment();
    deployment->deploy();
  }
  WDT::reload();
  radioLink->enable();
  LinkProtocol::getRestartPacket(packet);
  WDT::reload();
  radioLink->transmit(packet, HEADER_LENGTH);
  WDT::reload();
  radioLink->disable();
  WDT::reload();
  receiveAndProcessCommands();
}

void loop() {
  long loopStart = millis();
  getAndTransmitPhoto();
  getAndTransmitTelemetry();
  receiveAndProcessCommands();
  
  long sleepMs = loopStart + Config::data.downlinkInterval * 1000 - millis();
  if (sleepMs > 0) {
    WDT::wddelay(sleepMs);
  }
  loopCount += 1;
  if (loopCount > 50) {
    LinkProtocol::tacticalRestart();
  }
  
}

void getAndTransmitPhoto() {
  if (!Config::data.camera1Enabled && !Config::data.camera2Enabled) {
    return;
  }
  if (!Config::data.camera1Enabled && Receiver::useCam1) {
    Receiver::useCam1 = false;
  }
  if (!Config::data.camera2Enabled && !Receiver::useCam1) {
    Receiver::useCam1 = true;
  }
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

void getTelemetry() {
  WDT::reload();
  sensors->enable();
  delay(1000);
  WDT::reload();
  tempSensor1Data[tempSensorHead] = sensors->readTempSensor(0);
  tempSensor2Data[tempSensorHead] = sensors->readTempSensor(1);
  delay(1000);
  tempSensor1Data[tempSensorHead] = sensors->readTempSensor(0);
  tempSensor2Data[tempSensorHead] = sensors->readTempSensor(1);
  tempSensorHead = (tempSensorHead + 1) % TEMP_DATA_BUFFER_SIZE;
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

void receiveAndProcessCommands() {
  WDT::reload();
  byte command[256] = {};
  radioLink->enable();
  WDT::reload();
  LinkProtocol::getUplinkStart(packet);
  radioLink->transmit(packet, HEADER_LENGTH);
  WDT::reload();
  Serial.println("starting receive");
  long loopStart = millis();
  long lastCommand = millis();
  while (millis() - loopStart < Config::data.uplinkDuration * 1000 * 5 && millis() - lastCommand < Config::data.uplinkDuration * 1000) {
    radioLink->receive(command);
    WDT::reload();
    if (LinkProtocol::processCommand((char*)command, radioLink) == 0) {
      lastCommand = millis();
      memset(command, 0, 256);
    }
    WDT::reload();
  }
  LinkProtocol::getUplinkEnd(packet);
  radioLink->transmit(packet, HEADER_LENGTH);
  radioLink->disable();
}
