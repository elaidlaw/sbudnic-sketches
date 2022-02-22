#include<SBUDNIC.h>
#include <Wire.h>

RecieverInterface* recobj;
uint8_t data[10000];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//  while(!Serial.available());
  Serial.println("abc");
  delay(5000);
  recobj = new Reciever(data, 10000);
  recobj->disable();
  delay(500);
  Wire.begin(44);
  Wire.onReceive(Reciever::recieve_data);
  Wire.onRequest(Reciever::send_settings);
  recobj->enable();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(recobj->get_output_pos());
  int num;
  while((num = recobj->get_state()) == REC_WAIT){
    Serial.println("waiting");
    delay(500);
//    Serial.println(num);
  }
  Serial.println(num);
//  recobj->disable();
  Serial.println("picture done");
  delay(10000);
  int x = recobj->get_output_pos();
  
  int i;
  for(i=0;i<x;i++){
    Serial.print(data[i]);
  }
}
