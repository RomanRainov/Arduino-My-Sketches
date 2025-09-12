#include <iBus.h>
#include "RR_Freenove4WDCar_Driver.h"
#include "RR_Freenove4WDCar_Servo.h"
#include "Freenove_WS2812B_RGBLED_Controller.h"


void initFreeDrive() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loopFreeDrive() {

  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, Serial.available());

  receiver.process();
  if (receiver.get(0) == 0)
    return;

  proccessChannel12();
  proccessChannel3();
  proccessChannel4();

  delay(10);
}

void proccessChannel12() {
  int channel1Value = map(receiver.get(1), 1000, 2000, -1 * RRFreenove4WDCarDriver::MAX_TURN_VALUE, RRFreenove4WDCarDriver::MAX_TURN_VALUE);
  int channel2Value = map(receiver.get(2), 1000, 2000, -1 * RRFreenove4WDCarDriver::MAX_SPEED, RRFreenove4WDCarDriver::MAX_SPEED);

 
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
  } else if (abs(channel1Value) >= RRFreenove4WDCarDriver::MAX_TURN_VALUE) {
    if (channel1Value > 0) {
      driver.rotateRight(abs(channel1Value));
    } else {
      driver.rotateLeft(abs(channel1Value));
    }
  } else {
    driver.stop();
  }

  //Serial.println('\t');
}

void proccessChannel3() {
  int channelValue = receiver.get(3);
  int value = map(channelValue, 1000, 2000, 0, 3000);
  //Serial.println("Channel3 - " + String(channelValue) + "; map - " + String(value));
  if (value < 150) {
    noTone(A0);
  } else {
    tone(A0, value);
  }
}

void proccessChannel4() {
  int position = SERVO_START_POS;
  int channelValue = receiver.get(4);
  int value = map(channelValue, 1000, 2000, -90, 90);
  if (value > 5) {
    position = SERVO_START_POS - value;
  } else if (value < -5) {
    position = SERVO_START_POS + abs(value);
  }
  servo.setPosition(position);
}
