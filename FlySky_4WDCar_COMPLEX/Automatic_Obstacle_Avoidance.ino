/**********************************************************************
  Filename    : Automatic_Obstacle_Avoidance.ino
  Product     : Freenove 4WD Car for UNO
  Description : Automatic Obstacle Avoidance mode.
  Auther      : www.freenove.com
  Modification: 2019/08/15
**********************************************************************/
#include <Servo.h>
#include "Freenove_WS2812B_RGBLED_Controller.h"
#include "RR_Freenove4WDCar_Driver.h"

#define OBSTACLE_DISTANCE   40
#define OBSTACLE_DISTANCE_LOW 15


byte servoOffset = 0;
int speedOffset;//batteryVoltageCompensationToSpeed

void init_Automatic_Obstacle_Avoidance() {    
    
  calculateVoltageCompensation();

//---blink & pip
  driver.setAllLedsColor(LedColor::White); //Set all LED color to white  
  delay(20);
  
  //pinMode(PIN_BUZZER, OUTPUT);
  //digitalWrite(PIN_BUZZER, HIGH);    
  delay(1000);
  //digitalWrite(PIN_BUZZER, LOW);
  delay(20);


  driver.setAllLedsColor(LedColor::Dark);    //set all LED off .
}

void loop_Automatic_Obstacle_Avoidance() {
  updateAutomaticObstacleAvoidance();
}

void updateAutomaticObstacleAvoidance() {
  int distance[3], tempDistance[3][5], sumDisntance;
  static u8 leftToRight = 0, servoAngle = 0, lastServoAngle = 0;  //
  const u8 scanAngle[2][3] = { {150, 90, 30}, {30, 90, 150} };

  for (int i = 0; i < 3; i++)
  {
    servoAngle = scanAngle[leftToRight][i];
    servo.setPosition(servoAngle);
    if (lastServoAngle != servoAngle) {
      delay(130);
    }
    lastServoAngle = servoAngle;
    for (int j = 0; j < 5; j++) {
      tempDistance[i][j] = sonar.getDistance();
      //delayMicroseconds(2 * SONIC_TIMEOUT);
      sumDisntance += tempDistance[i][j];
    }
    if (leftToRight == 0) {
      distance[i] = sumDisntance / 5;
    }
    else {
      distance[2 - i] = sumDisntance / 5;
    }
    sumDisntance = 0;
  }
  leftToRight = (leftToRight + 1) % 2;

  if (distance[1] < OBSTACLE_DISTANCE) {        //Too little distance ahead

    driver.setAllLedsColor(LedColor::Red); //Set all LED color to red
    buzzer.on();
    
    if (distance[0] > distance[2] && distance[0] > OBSTACLE_DISTANCE) {     //Left distance is greater than right distance
      motorRun(-(150 + speedOffset), -(150 + speedOffset)); //Move back
      delay(100);
      motorRun(-(150 + speedOffset), (150 + speedOffset));  
    }
    else if (distance[0] < distance[2] && distance[2] > OBSTACLE_DISTANCE) {                   //Right distance is greater than left distance
      motorRun(-(150 + speedOffset), -(150 + speedOffset)); //Move back 
      delay(100);
      motorRun((150 + speedOffset), -(150 + speedOffset));
    }
    else {                      //Get into the dead corner, move back, then turn.
      motorRun(-(150 + speedOffset), -(150 + speedOffset));
      delay(100);
      motorRun(-(150 + speedOffset), (150 + speedOffset));
    }
  }
  else {                        //No obstacles ahead

    driver.setAllLedsColor(LedColor::Green); //Set all LED color to green
    buzzer.off();
    
    if (distance[0] <  OBSTACLE_DISTANCE_LOW) {      //Obstacles on the left front.
      motorRun(-(150 + speedOffset), -(150 + speedOffset)); //Move back
      delay(100);
      motorRun((180 + speedOffset), (50 + speedOffset));
    }
    else if (distance[2] <  OBSTACLE_DISTANCE_LOW) {     //Obstacles on the right front.
      motorRun(-(150 + speedOffset), -(150 + speedOffset)); //Move back
      delay(100);
      motorRun((50 + speedOffset), (180 + speedOffset));
    }
    else {                        //Cruising
      motorRun((80 + speedOffset), (80 + speedOffset));
    }
  }
}


void calculateVoltageCompensation() {
  float voltageOffset = 8.2 - getBatteryVoltage();
  speedOffset = abs(voltageOffset) * 20;
}

void motorRun(int speedl, int speedr) {
  int dirL = 0, dirR = 0;
  if (speedl > 0) {
    dirL = 0 ^ MOTOR_DIRECTION;
  } else {
    dirL = 1 ^ MOTOR_DIRECTION;
    speedl = -speedl;
  }

  if (speedr > 0) {
    dirR = 1 ^ MOTOR_DIRECTION;
  } else {
    dirR = 0 ^ MOTOR_DIRECTION;
    speedr = -speedr;
  }

  speedl = speedl > 150 ? speedl : 150;

  digitalWrite(PIN_DIRECTION_LEFT, dirL);
  digitalWrite(PIN_DIRECTION_RIGHT, dirR);
  analogWrite(PIN_MOTOR_PWM_LEFT, speedl);
  analogWrite(PIN_MOTOR_PWM_RIGHT, speedr);
}


float getBatteryVoltage() {
  pinMode(PIN_BATTERY, INPUT);
  int batteryADC = analogRead(PIN_BATTERY);
  float batteryVoltage = batteryADC / 1023.0 * 5.0 * 4;
  return batteryVoltage;
}
