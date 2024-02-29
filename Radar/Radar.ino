#include "Freenove_WS2812B_RGBLED_Controller.h"
#include <ArduinoSort.h>
#include <Dictionary.h>
#include "RR_Freenove4WDCar_Buzzer.h"
#include "RR_Freenove4WDCar_Sonar.h"
#include "RR_Freenove4WDCar_Servo.h"
#include "RR_Freenove4WDCar_Motor.h"

#define MOTOR_DIRECTION 1  //If the direction is reversed, change 0 to 1
#define PIN_DIRECTION_LEFT 4
#define PIN_DIRECTION_RIGHT 3
#define PIN_MOTOR_PWM_LEFT 6
#define PIN_MOTOR_PWM_RIGHT 5

#define LEDS_COUNT 10  //it defines number of lEDs.

#define PIN_SERVO 2       //define servo pin
#define PIN_SONIC_TRIG 7  //define Trig pin
#define PIN_SONIC_ECHO 8  //define Echo pin
#define PIN_BUZZER A0
#define I2C_ADDRESS 0x20

#define MAX_DISTANCE 5000.0  //cm

#define SERVO_STEP 10
#define SERVO_OFFSET 5
#define SERVO_START_POS 90 + SERVO_OFFSET
#define SERVO_LEFT SERVO_START_POS + 60
#define SERVO_RIGHT SERVO_START_POS - 60
#define SERVO_DELTA 20.0  //cm

RRFreenove4WDCarServo servo(PIN_SERVO, SERVO_OFFSET, SERVO_START_POS);
byte servoPos = SERVO_START_POS;
bool directionRight = true;
//float distances[180];
Dictionary &distances = *(new Dictionary());

Freenove_WS2812B_Controller strip(I2C_ADDRESS, LEDS_COUNT, TYPE_GRB);  //initialization
byte ledStripCounter = 0;

RRFreenove4WDCarSonar sonar(PIN_SONIC_TRIG, PIN_SONIC_ECHO, MAX_DISTANCE);

RRFreenove4WDCarMotor motor;
RRFreenove4WDCarBuzzer buzzer;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  
  while (!strip.begin())
    ;
  
  servo.setup();
  sonar.setup();
  motor.setup();
  
  clearDistances();

  Serial.println("Started");
}

void loop() {

  //strip.setAllLedsColor(0xF010BB);
  strip.setAllLedsColor(strip.Wheel(ledStripCounter));
  ledStripCounter++;

  servoPos += SERVO_STEP * (directionRight ? -1 : 1);
  if (servoPos >= SERVO_LEFT) {
    directionRight = true;
    servoPos -= SERVO_STEP;
  } else if (servoPos <= SERVO_RIGHT) {
    directionRight = false;
    servoPos += SERVO_STEP;
  }
  Serial.print("Servo pos: ");
  Serial.print(servoPos);
  Serial.print("; directionR: ");
  Serial.print(directionRight);

  byte distIndex = servoPos - SERVO_OFFSET;
  float newDistance = getDistance();
  Serial.print("; distances[distIndex]: ");
  float prevDistance = distances[distIndex].toFloat();
  Serial.print(prevDistance);
  Serial.print("; newDistance: ");
  Serial.print(newDistance);
  float delta = prevDistance - newDistance;
  if (prevDistance > 0 && abs(delta) > SERVO_DELTA) {
    Serial.print("; Delta: ");
    Serial.print(delta);
    strip.setAllLedsColor(0xFF0000);
    buzzer.beep(200);
    if (servoPos <= 60){
      strip.setAllLedsColor(0x00FF00);
      motor.rotateRight(100);
      delay(500);
      motor.stop();
    }    
    else if(servoPos >= 120){
      strip.setAllLedsColor(0x0000FF);
      motor.rotateLeft(100);
      delay(500);
      motor.stop();
    }
    else {
      strip.setAllLedsColor(0xF0F0F0);
      motor.forward(180);
      delay(1000);
      motor.stop();
    }

    clearDistances();
    servo.setPosition(servoPos);
    return;
  }
  
  distances[distIndex] = String(newDistance);

  Serial.println();
  servo.setPosition(servoPos);
  delay(25);
}

void clearDistances(){
  //memset(distances, 0, sizeof(distances));
}

float getDistance() {
  byte distIndex = servoPos - SERVO_OFFSET;
  float newDistance = getDistance();  
  float prevDistance = distances[distIndex].toFloat();
  float delta = prevDistance - newDistance;
  if (prevDistance > 0 && abs(delta) > SERVO_DELTA) {

    Serial.print("; delta: ");
    Serial.print(delta);

    return getMedianDistance();
  }
  return 0;
}

float getMedianDistance() {
  float dists[5];
  for (int i = 0; i < 5; i++) {
    strip.setAllLedsColor(0xFFFFFF);
    dists[i] = sonar.getDistance();
    strip.setAllLedsColor(0x000000);
  }
  sortArray(dists, 5);
  return dists[2];
}
