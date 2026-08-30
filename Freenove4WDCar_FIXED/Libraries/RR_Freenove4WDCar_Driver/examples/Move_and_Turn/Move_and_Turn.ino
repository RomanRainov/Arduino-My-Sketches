#include "RR_Freenove4WDCar_Driver.h"
/**********************************************************************/
#define MOTOR_DIRECTION 1  //If the direction is reversed, change 0 to 1
#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT 4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT 6
#define PIN_BATTERY A0

RRFreenove4WDCarDriver driver(MOTOR_DIRECTION, PIN_DIRECTION_LEFT, PIN_DIRECTION_RIGHT, PIN_MOTOR_PWM_LEFT, PIN_MOTOR_PWM_RIGHT, PIN_BATTERY);

void setup() {
  driver.setup();
}

void loop() {
  //Move forward
  // driver.forward(50);  
  // delay(3000);

  // driver.forward(50, 25);  
  // delay(3000);

  // driver.forward(25, 50);  
  // delay(3000);

  driver.forwardLeft(25, 25);  
  delay(3000);

  driver.forwardLeft(50, 50);  
  delay(3000);

  driver.forwardLeft(75, 75);  
  delay(3000);

  driver.forwardLeft(100, 100);  
  delay(3000);

  // driver.forwardRight(75, 50);  
  // delay(3000);

  //Move back
  // driver.backward(100);  
  // delay(1000);

  // //Turn left
  // driver.rotateLeft(120);
  // delay(700);

  // //Turn right
  // driver.rotateRight(120);
  // delay(700);

  //Stop
  driver.stop();
  delay(2000);

}
