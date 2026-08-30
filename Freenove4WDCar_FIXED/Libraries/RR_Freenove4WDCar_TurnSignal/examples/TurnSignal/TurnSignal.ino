#include "Freenove_WS2812B_RGBLED_Controller.h"
#include "RR_Freenove4WDCar_TurnSignal.h"

RRFreenove4WDCarTurnSignal turnSignal;

void setup() {    
  Serial.begin(9600);

  Serial.print("setup...\t");
  turnSignal.setup();
  Serial.println("setup is done!");
  //turnSignal.turnSignalOn(LEDS_ALL);
}

void loop() {
   Serial.print("loop...\t");
   turnSignal.turnSignalOn(LEDS_ALL);
   delay(5000);
   turnSignal.turnSignalOff();
   delay(2000);
   Serial.println("loop is ended---------------------");
}
