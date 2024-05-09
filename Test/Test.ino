#include <iBus.h>
#include "RR_Freenove4WDCar_Driver.h"
#include "RR_Freenove4WDCar_Servo.h"
#include "Freenove_WS2812B_RGBLED_Controller.h"

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
//#define I2C_ADDRESS 0x20
//#define LEDS_COUNT 10

//Freenove_WS2812B_Controller strip(I2C_ADDRESS, LEDS_COUNT, TYPE_GRB);
RRFreenove4WDCarDriver driver(MOTOR_DIRECTION, PIN_DIRECTION_LEFT, PIN_DIRECTION_RIGHT, PIN_MOTOR_PWM_LEFT, PIN_MOTOR_PWM_RIGHT, PIN_BATTERY);
RRFreenove4WDCarServo servo(PIN_SERVO, SERVO_OFFSET, SERVO_START_POS);
iBus receiver(Serial, MAX_CHANNELS);  // //Serial on ATMega328 boards(Uno, Nano, etc.)


#define CH1_MIN 5
#define CH1_MAX 90
#define CH2_MIN 5
#define CH2_MAX 100
#define CH3_MIN 5
#define CH3_MAX 90

#define TRACK_WIDTH 125  //mm
#define MIN_RADIUS 500   //mm
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

u32 yellow = 0xFFFF00;
u32 red = 0xFF0000;
u32 red_dark = 0x770000;
u32 white = 0xFFFFFF;
u32 blue = 0x0000FF;
u32 green = 0x00FF00;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(115200);
  receiver.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  driver.setup();
  servo.setup();
  
  //while (!strip.begin());
  //stopLeds();

  Serial.print("ch1K: ");
  Serial.print(String(ch1K));
  Serial.print('\t');
  Serial.print("ch1B: ");
  Serial.print(String(ch1B));
  Serial.print('\t');
  Serial.print("ch2K: ");
  Serial.print(String(ch2K));
  Serial.print('\t');
  Serial.print("ch2B: ");
  Serial.print(String(ch2B));
  Serial.print('\t');
  Serial.println();
}

void loop() {

  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, Serial.available());

  receiver.process();
  if (receiver.get(0) == 0)
    return;

  proccessChannel3();
  
  channel1Value = map(receiver.get(1), 1000, 2000, -1 * RRFreenove4WDCarDriver::MAX_TURN_VALUE, RRFreenove4WDCarDriver::MAX_TURN_VALUE);
  channel2Value = map(receiver.get(2), 1000, 2000, -1 * RRFreenove4WDCarDriver::MAX_SPEED, RRFreenove4WDCarDriver::MAX_SPEED);

  //Serial.print("channel1Value: ");
  //Serial.print(String(channel1Value));
  //Serial.print('\t');
  //Serial.print("channel2Value: ");
  //Serial.print(String(channel2Value));
  //Serial.print('\t');
  
  if (abs(channel2Value) > 0) {
    //moveLeds(channel2Value, channel1Value);
    if (channel2Value > 0) {
      if (channel1Value > 0) {
        driver.forwardRight(abs(channel2Value), abs(channel1Value));
      } else {
        driver.forwardLeft(abs(channel2Value), abs(channel1Value));
      }
    } else {
      if (channel1Value > 0) {
        driver.backwardRight(abs(channel2Value), abs(channel1Value));
      } else {
        driver.backwardLeft(abs(channel2Value), abs(channel1Value));
      }
    }
  } else if (abs(channel1Value) >= CH1_MIN) {    
    if (channel1Value > 0) {
      driver.rotateRight(abs(channel1Value));
    } else {
      driver.rotateLeft(abs(channel1Value));
    }
  } else {
    driver.stop();
  }

  //Serial.println('\t');


  delay(10);
}

void proccessChannel3() {
  int position = SERVO_START_POS;
  int val = receiver.get(4);
  channel3Value = map(val, 1000, 2000, -90, 90);
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
  ////Serial.print(channelInput);
  ////Serial.print(" - readChannel pulseIn: ");
  ////Serial.print(ch);
  ////Serial.print('\t');
  if (ch < 100) return defaultValue;
  int result = map(ch, 1000, 2000, minLimit, maxLimit);
  ////Serial.print("readChannel result: ");
  ////Serial.print(result);
  ////Serial.print('\t');
  return result;
}

// Read the switch channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}
