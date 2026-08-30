/*
  RR_Freenove4WDCar_Servo.cpp - Library for Arduino Sonar.
  Created by Roman Rainov, Janaury 29, 2024.
  Released into the private domain.
*/

#include "Arduino.h"
#include "RR_Freenove4WDCar_Servo.h"

RRFreenove4WDCarServo::RRFreenove4WDCarServo(byte servoPin, byte servoOffset, byte servoStartPos)
{
    _servoPin = servoPin;
    _offset = servoOffset;
    _startPosition = servoStartPos;
}

void RRFreenove4WDCarServo::setup()
{
    _servo.attach(_servoPin);
    setPosition(_startPosition + _offset);
}

void RRFreenove4WDCarServo::setPosition(byte position)
{
    _position = position;
    setPosition();
}

void RRFreenove4WDCarServo::setPosition()
{
    _servo.write(_position);
}

byte RRFreenove4WDCarServo::getOffset()
{
    return _offset;
}

byte RRFreenove4WDCarServo::getStartPosition()
{
    return _startPosition;
}

byte RRFreenove4WDCarServo::getPosition()
{
    return _position;
}