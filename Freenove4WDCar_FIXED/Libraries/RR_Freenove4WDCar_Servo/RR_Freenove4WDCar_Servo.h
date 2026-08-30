/*
  RR_Freenove4WDCar_Servo.h - Library for Arduino Sonar HC-SR04.
  Created by Roman Rainov, Janaury 29, 2024.
  Released into the private domain.
*/
#ifndef RR_Freenove4WDCar_Servo_h
#define RR_Freenove4WDCar_Servo_h

#include "Arduino.h"
#include "Servo.h"  //servo library

class RRFreenove4WDCarServo
{
	private:
        Servo _servo;
		byte _servoPin = 2;
        byte _offset = 0;
		byte _startPosition = 90;
        byte _position;
        void setPosition();
	public:
		RRFreenove4WDCarServo(byte servoPin = 2, byte servoOffset = 0, byte servoStartPos = 90);
		void setup();        
        byte getOffset();
        byte getStartPosition();
        byte getPosition();
        void setPosition(byte position);
};

#endif