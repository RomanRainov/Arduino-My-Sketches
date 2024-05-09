#include "RR_Freenove4WDCar_Driver.h"
#include "RR_Freenove4WDCar_Servo.h"

#define MOTOR_DIRECTION 1  //If the direction is reversed, change 0 to 1
#define PIN_DIRECTION_LEFT 4
#define PIN_DIRECTION_RIGHT 3
#define PIN_MOTOR_PWM_LEFT 6
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_SERVO 2  //define servo pin
#define SERVO_STEP 10
#define SERVO_OFFSET 5
#define SERVO_START_POS 90 + SERVO_OFFSET
#define PIN_BATTERY A0

RRFreenove4WDCarDriver driver(MOTOR_DIRECTION, PIN_DIRECTION_LEFT,PIN_DIRECTION_RIGHT, PIN_MOTOR_PWM_LEFT, PIN_MOTOR_PWM_RIGHT, PIN_BATTERY);
RRFreenove4WDCarServo servo(PIN_SERVO, SERVO_OFFSET, SERVO_START_POS);

#define CH1 10
#define CH2 11
#define CH3 0

#define CH1_MIN 20
#define CH1_MAX 120
#define CH2_MIN 20
#define CH2_MAX 220
#define CH3_MIN 5
#define CH3_MAX 90

#define TRACK_WIDTH 125   //mm
#define MIN_RADIUS 500    //mm
#define MAX_RADIUS 1125  //mm

int channel1Value = 0;
int channel2Value = 0;
int channel3Value = 0;

float ch1K = (float)(MAX_RADIUS - MIN_RADIUS) / (float)(CH1_MAX - CH1_MIN);
float ch1B = MIN_RADIUS - ch1K * CH1_MIN;
float ch2K = (float)(RRFreenove4WDCarMotor::MAX_SPEED - RRFreenove4WDCarMotor::MIN_SPEED) / (float)(CH2_MAX - CH2_MIN);
float ch2B = RRFreenove4WDCarMotor::MIN_SPEED - ch2K * CH2_MIN;

byte speedLeft = 0;
byte speedRight = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);

  driver.setup();
  servo.setup();

  //Serial.print("ch1K: ");
  //Serial.print(String(ch1K));
  //Serial.print('\t');
  //Serial.print("ch1B: ");
  //Serial.print(String(ch1B));
  //Serial.print('\t');
  //Serial.print("ch2K: ");
  //Serial.print(String(ch2K));
  //Serial.print('\t');
  //Serial.print("ch2B: ");
  //Serial.print(String(ch2B));
  //Serial.print('\t');
  //Serial.println();
}

void loop() {
  channel1Value = readChannel(CH1, -1 * CH1_MAX, CH1_MAX, 0);
  channel2Value = readChannel(CH2, -1 * CH2_MAX, CH2_MAX, 0);
  channel3Value = readChannel(CH3, -1 * CH3_MAX, CH3_MAX, 0);

proccessChannel3();

  float speedMove = abs(channel2Value) < CH2_MIN ? 0 : abs(channel2Value) * ch2K + ch2B;
  //Serial.print("speedMove: ");
  //Serial.print(String(speedMove));
  //Serial.print('\t');

  if (speedMove > 0) {
    speedLeft = speedRight = speedMove;
    /*if (abs(channel1Value) >= CH1_MIN) {
      float radius = abs(channel1Value) < CH1_MIN ? 0 : abs(channel1Value) * ch1K + ch1B;
      float speedTurn = speedMove * radius / (radius + TRACK_WIDTH);
      if (channel1Value > 0) {
        speedRight = speedTurn;
      } else {
        speedLeft = speedTurn;
      }

      //Serial.print("radius: ");
      //Serial.print(String(radius));
      //Serial.print('\t');

      //Serial.print("speedTurn: ");
      //Serial.print(String(speedTurn));
      //Serial.print('\t');

      //Serial.print("speedLeft: ");
      //Serial.print(String(speedLeft));
      //Serial.print('\t');
      //Serial.print("speedRight: ");
      //Serial.print(String(speedRight));
      //Serial.print('\t');
    }*/
    if (channel2Value > 0) {
      speedLeft = speedRight = constrain(speedMove, 100, 250);
      driver.forward(speedLeft);
    } else {
      driver.backward(speedLeft);
    }
  } else if (abs(channel1Value) >= CH1_MIN) {
    if (channel1Value > 0) {
      driver.rotateRight(abs(channel1Value) + 100);
    } else {
      driver.rotateLeft(abs(channel1Value) + 100);
    }
  } else {
    driver.stop();
  }

  
  // put your main code here, to run repeatedly:
  //digitalWrite(LED_BUILTIN, Serial.available());
  //Serial.println();

  //delay(1000);
}

void proccessChannel3() {
  int position = SERVO_START_POS;
  channel3Value = readChannel(CH3, -90, 90, 0);
  if (channel3Value > 5) {
    position = SERVO_START_POS - channel3Value;
  } else if (channel3Value < -5) {
    position = SERVO_START_POS + abs(channel3Value);
  }
  servo.setPosition(position);
}


// Read the number of a specified channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue) {
  int ch = pulseIn(channelInput, HIGH, 30000);
  //Serial.print(channelInput);
  //Serial.print(" - readChannel pulseIn: ");
  //Serial.print(ch);
  //Serial.print('\t');
  if (ch < 100) return defaultValue;
  int result = map(ch, 1000, 2000, minLimit, maxLimit);
  //Serial.print("readChannel result: ");
  //Serial.print(result);
  //Serial.print('\t');
  return result;
}

// Read the switch channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}
