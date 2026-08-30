#include "RR_Freenove4WDCar_Buzzer.h"

#define PIN_BUZZER      A0

RRFreenove4WDCarBuzzer buzzer(PIN_BUZZER);

void setup() {  
}

void loop() {
  for (int i = 0; i < 4; i++) {
    buzzer.beep(100);
    delay(100);
  }
  delay(500);
}
