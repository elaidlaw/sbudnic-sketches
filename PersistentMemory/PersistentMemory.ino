//https://os.mbed.com/docs/mbed-os/v6.12/apis/kvstore.html
//KVStore. TDBStore - Default implementation of the KVStore API. It provides static wear-leveling and quick access for when you have a small number of KV pairs.
//https://os.mbed.com/docs/mbed-os/v6.12/apis/data-architecture.html

#include "KVStore.h"
#include "kvstore_global_api.h"

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("--- Mbed OS KVStore Simplified example--- ");

  /* Start By Resetting the KV Storage */
  Serial.println("kv_reset");
//  kv_reset("/kv/");//comment out when retreiving saved data
  Serial.println();

  const char* const Key1 = "store_cString";
  const char* const Key2 = "store_longArray";
  const char* const Key3 = "store_Struct";

  /* Set First Key/Value pair*/ 
  const char* const cStringToStore = "Hello World/Goodnight Moon";//const pointer to constant string
  Serial.print("kv_set key: ");
  Serial.println(Key1);
//  kv_set(Key1, cStringToStore, strlen(cStringToStore), 0); //0 for flags?

  /* Read the KV Pair you've just set */
  char Key1Out[strlen(cStringToStore) + 1] = "";
  memset(Key1Out, '\0', strlen(cStringToStore) + 1); //clear and null terminate
  kv_get(Key1, Key1Out, strlen(cStringToStore) + 1 , 0);
  Serial.print("kv_get key: ");
  Serial.println(Key1);
  Serial.print("kv_get key Value: ");
  Serial.println(Key1Out);
  Serial.println();

  /* Set Second Key/Value pair*/
  uint32_t km[3];
  km[0] = 123456789;
  km[1] = 234567891;
  km[2] = 345678912;

  Serial.print("kv_set key: ");
  Serial.println(Key2);
//  kv_set(Key2, (uint8_t*)&km, sizeof(km), 0);

  /* Read the KV Pair you've just set */
  uint32_t kmOut[3];
  kmOut[0] = 0;
  kmOut[1] = 0;
  kmOut[2] = 0;

  Serial.print("kv_get key: ");
  Serial.println(Key2);
  kv_get(Key2, (byte*)&kmOut, sizeof(kmOut), 0);

  Serial.print("kv_get key Value: ");
  for (byte i = 0; i < 3; i++)
  {
    Serial.print(kmOut[i]);
    if (i != 2)
      Serial.print(',');
  }
  Serial.println();
  Serial.println();

  /* Set Third Key/Value pair*/
  typedef struct {
    unsigned long timeStamp;
    float accel;
    char pres[4];
  } structData;


  structData dataSaved;
  structData dataRetreived;

  dataSaved.timeStamp = 445566;
  dataSaved.accel = 1.23;
  strcpy(dataSaved.pres, "789");

  dataRetreived.timeStamp = 0;
  dataRetreived.accel = 0;
  strcpy(dataRetreived.pres, "---");
  
  Serial.print("kv_set key: ");
  Serial.println(Key3);
//  kv_set(Key3, (uint8_t*)&dataSaved, sizeof(dataSaved), 0);

  Serial.print("kv_get key: ");
  Serial.println(Key3);
  kv_get(Key3, (byte*)&dataRetreived, sizeof(dataRetreived), 0);

  Serial.print("kv_get key Value: ");
  Serial.print(dataRetreived.timeStamp);
  Serial.print(',');
  Serial.print(dataRetreived.accel);
  Serial.print(',');
  Serial.print(dataRetreived.pres);
  Serial.println();

  /* Finally, reset will format kvstore and remove All Keys (including write-once keys) */
  //Serial.println("kv_reset");
  //kv_reset("/kv/");
}

void loop() {}
