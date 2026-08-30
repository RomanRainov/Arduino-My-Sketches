#include "RR_Freenove4WDCar_Servo.h"

#define PIN_SERVO  2    //define servo pin
#define SERVO_OFFSET 5
#define SERVO_START_POS 90 + SERVO_OFFSET

RRFreenove4WDCarServo servo(PIN_SERVO, SERVO_OFFSET, SERVO_START_POS);

void setup() {
  servo.setup();
}

void loop() {
  long position = random(SERVO_START_POS - 30, SERVO_START_POS + 30);
  servo.setPosition(position);
  delay(1000);
}
