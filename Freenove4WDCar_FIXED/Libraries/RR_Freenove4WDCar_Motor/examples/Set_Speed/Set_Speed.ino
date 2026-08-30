#include "RR_Freenove4WDCar_Motor.h"
/**********************************************************************/
#define MOTOR_DIRECTION 1  //If the direction is reversed, change 0 to 1
#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT 4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT 6
#define PIN_BATTERY A0

#define SERIAL_DEBUG_RRFreenove4WDCarMotor

RRFreenove4WDCarMotor motorLeft(PIN_DIRECTION_LEFT, PIN_MOTOR_PWM_LEFT, PIN_BATTERY);
RRFreenove4WDCarMotor motorRight(PIN_DIRECTION_RIGHT, PIN_MOTOR_PWM_RIGHT, PIN_BATTERY);

byte speed = 0;
byte leftSpeed = 0;
byte rightSpeed = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.setTimeout(100);
  Serial.println("Starting.....");
  motorLeft.setup();
  motorRight.setup();
  Serial.println("Started!");
}

void loop() {
  Serial.println("loop...");
  String input;
  while (Serial.available()) {
    input = Serial.readString();
    input.trim();
    Serial.println(input);
  }
  
  if (input != nullptr && input.length() > 0) {
    int spaceIndex = input.indexOf(' ');
    if (spaceIndex > 0) {
      leftSpeed = input.substring(0, spaceIndex).toInt();
      rightSpeed = input.substring(spaceIndex + 1, input.length()).toInt();
    } else {
      speed = input == nullptr ? speed : input.toInt();
      leftSpeed = speed;
      rightSpeed = speed;
    }
  }
  //speed = input == nullptr ? speed : input.toInt();
  Serial.print("leftSpeed: ");
  Serial.print(leftSpeed);
  Serial.print(";\t rightSpeed: ");
  Serial.println(rightSpeed);
  //Move forward
  if (rightSpeed > 0 && motorRight.currentSpeed() == 0) {
    motorRight.move(RRFreenove4WDCarMotor::START_SPEED, !MOTOR_DIRECTION);
  }
  if (leftSpeed > 0 && motorRight.currentSpeed() == 0) {
    motorLeft.move(RRFreenove4WDCarMotor::START_SPEED, MOTOR_DIRECTION);
  }
  delay(10);
  motorRight.move(rightSpeed, !MOTOR_DIRECTION);
  motorLeft.move(leftSpeed, MOTOR_DIRECTION);
  //motorRun(200, 200);
  delay(1000);
}
