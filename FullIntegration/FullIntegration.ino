#include <SBUDNIC.h>
#include <Wire.h>

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

void setup(){
  // Low power configuration
  digitalWrite(LED_PWR, LOW);
  digitalWrite(PIN_ENABLE_SENSORS_3V3, HIGH);
  digitalWrite(PIN_ENABLE_I2C_PULLUP, HIGH);
}

void loop() {
  
}
