/**********************************************************************
  Filename    : 02.4.1_Automatic_Stay_Close_To_Obstacle.ino
  Product     : Freenove 4WD Car for UNO
  Description : Automatic Stay Close To Obstacle mode. 
  Auther      : www.freenove.com
  Modification: 2019/08/15
**********************************************************************/
#include <Servo.h>
#include "Freenove_WS2812B_RGBLED_Controller.h"
#include "RR_Freenove4WDCar_Driver.h"


#define OBSTACLE_DISTANCE_MAX   50
#define OBSTACLE_DISTANCE_MID   25
#define OBSTACLE_DISTANCE_LOW 10

//---SONAR
#define MAX_DISTANCE    1000
#define SONIC_TIMEOUT   (MAX_DISTANCE * 60L)
#define SOUND_VELOCITY    340   //soundVelocity: 340m/s



//Servo servo;
byte servoOffset = -5;
int speedOffset;//batteryVoltageCompensationToSpeed


void init2() {    
  
  Serial.println("Started");

  servo.setPosition(90);
  
  calculateVoltageCompensation();
  
//---blink & pip
  driver.setAllLedsColor(LedColor::White); //Set all LED color to white  
  delay(20);
  pinMode(PIN_BUZZER, OUTPUT);
  //digitalWrite(PIN_BUZZER, HIGH);    
  tone(PIN_BUZZER, 10000, 1000);
  //delay(1000);
  //digitalWrite(PIN_BUZZER, LOW);
  //noTone(PIN_BUZZER);
  //delay(20);
  driver.setAllLedsColor(LedColor::Dark);    //set all LED off .
}

void loop2() {

  updateAutomaticStayCloseToAbstacle();  
  
  //updateAutomaticObstacleAvoidance();
}

void updateAutomaticStayCloseToAbstacle()
{
    int sumDistance = 0;
    for (int j = 0; j < 5; j++) {
      int distance = getSonar();
      delayMicroseconds(2 * SONIC_TIMEOUT);
      sumDistance += distance;
    }      
      int midDistance = sumDistance / 5;
      if(midDistance <= OBSTACLE_DISTANCE_LOW)
      {
        driver.setAllLedsColor(LedColor::Red); //Set all LED color to red  
        delay(20);
        driveBack(150); //Move back         
        //tone(PIN_BUZZER, 2000, 500);
        sound(1500);
        //delay(100);
      } 
      else if( midDistance > OBSTACLE_DISTANCE_LOW && midDistance <= OBSTACLE_DISTANCE_MID)
      {
        driver.setAllLedsColor(LedColor::Blue); //Set all LED color to blue  
        delay(20);
        motorRun(0, 0);
        //tone(PIN_BUZZER, 1300, 500);
        delay(20);
      }
      else if( midDistance > OBSTACLE_DISTANCE_MID && midDistance < OBSTACLE_DISTANCE_MAX)
      {
        driver.setAllLedsColor(LedColor::Green); //Set all LED color to green  
        delay(20);
        driveForward(100);
        //tone(PIN_BUZZER, 400, 500);
        delay(20);
      }
      else if( midDistance >= OBSTACLE_DISTANCE_MAX)
      {
        driver.setAllLedsColor(LedColor::White); //Set all LED color to white
        delay(20);
        motorRun(0, 0);
        //tone(PIN_BUZZER, 500, 500);
        delay(20);
      }
}

void driveForward(int speed)
{
    motorRun((speed + speedOffset), (speed + speedOffset));
}

void driveBack(int speed)
{
    motorRun((-1 * speed + speedOffset), (-1 * speed + speedOffset));
}

void sound(int delayValue)
{
  digitalWrite(PIN_BUZZER, HIGH);    
  delay(delayValue);
  digitalWrite(PIN_BUZZER, LOW);
  delay(20);
}

void updateAutomaticObstacleAvoidance() {
  int distance[3], tempDistance[3][5], sumDistance;
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
      tempDistance[i][j] = getSonar();
      delayMicroseconds(2 * SONIC_TIMEOUT);
      sumDistance += tempDistance[i][j];
    }
    if (leftToRight == 0) {
      distance[i] = sumDistance / 5;
    }
    else {
      distance[2 - i] = sumDistance / 5;
    }
    sumDistance = 0;
  }
  leftToRight = (leftToRight + 1) % 2;

  if (distance[1] < OBSTACLE_DISTANCE_MAX) {        //Too little distance ahead

    driver.setAllLedsColor(0xFF0000); //Set all LED color to red
    delay(20);
    digitalWrite(PIN_BUZZER, HIGH);
    delay(20);
    
    if (distance[0] > distance[2] && distance[0] > OBSTACLE_DISTANCE_MAX) {     //Left distance is greater than right distance
      motorRun(-(150 + speedOffset), -(150 + speedOffset)); //Move back
      delay(100);
      motorRun(-(150 + speedOffset), (150 + speedOffset));  
    }
    else if (distance[0] < distance[2] && distance[2] > OBSTACLE_DISTANCE_MAX) {                   //Right distance is greater than left distance
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

    driver.setAllLedsColor(0x00FF00); //Set all LED color to green
    delay(20);
    digitalWrite(PIN_BUZZER, LOW);
    delay(20);
    
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

float getSonar() {
  unsigned long pingTime;
  float distance;
  digitalWrite(PIN_SONIC_TRIG, HIGH); // make trigPin output high level lasting for 10μs to triger HC_SR04,
  delayMicroseconds(10);
  digitalWrite(PIN_SONIC_TRIG, LOW);
  pingTime = pulseIn(PIN_SONIC_ECHO, HIGH, SONIC_TIMEOUT); // Wait HC-SR04 returning to the high level and measure out this waitting time
  if (pingTime != 0)
    distance = (float)pingTime * SOUND_VELOCITY / 2 / 10000; // calculate the distance according to the time
  else
    distance = MAX_DISTANCE;
  return distance; // return the distance value
}

void calculateVoltageCompensation() {
  float voltageOffset = 8.4 - getBatteryVoltage();
  speedOffset = voltageOffset * 20;
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
