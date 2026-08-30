#include "RR_Freenove4WDCar_Motor.h"
/**********************************************************************/
#define MOTOR_DIRECTION     1 //If the direction is reversed, change 0 to 1
#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT  4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT  6

RRFreenove4WDCarMotor motor(MOTOR_DIRECTION, PIN_DIRECTION_LEFT, PIN_DIRECTION_RIGHT, PIN_MOTOR_PWM_LEFT, PIN_MOTOR_PWM_RIGHT);

void setup() {
  motor.setup();
}

void loop() {
  //Move forward
  motor.forward(200);
  //motorRun(200, 200);
  delay(1000);

  //Move back
  motor.backward(200);
  //motorRun(-200, -200);
  delay(1000);

  //Turn left
  motor.rotateLeft(120);
  //motorRun(-200, 200);
  delay(700);

  //Turn right
  motor.rotateRight(120);
  //motorRun(200, -200);
  delay(700);

  //Stop
  motor.stop();
  //motorRun(0, 0);
  delay(2000);
}
