/*
  RR_Freenove4WDCar_Driver.cpp - Library for Arduino Freenove4WDCar.
  Created by Roman Rainov, March 1, 2024.
  Released into the private domain.
*/

#include "Arduino.h"
#include "RR_Freenove4WDCar_Driver.h"


RRFreenove4WDCarDriver::RRFreenove4WDCarDriver(
	bool motorDirection,
	byte pinDirectionLeft,
	byte pinDirectionRight,
	byte pinMotorPwmLeft,
	byte pinMotorPwmRight,
	byte pinBattery)
	: _motorDirection { motorDirection }
	, _motorLeft { RRFreenove4WDCarMotor(pinDirectionLeft, pinMotorPwmLeft, pinBattery) }
	, _motorRight { RRFreenove4WDCarMotor(pinDirectionRight, pinMotorPwmRight, pinBattery) }
	, _strip { I2C_ADDRESS, LEDS_COUNT, TYPE_GRB }
{	
}

void RRFreenove4WDCarDriver::setup()
{
	_motorLeft.setup();
	_motorRight.setup();

	while (!_strip.begin());
	leds(0, 0);
}

byte RRFreenove4WDCarDriver::getMotorSpeed(byte speed)
{
	byte speedFixed = constrain(speed, MIN_SPEED, MAX_SPEED);
	byte motorSpeed = abs(speed) < MIN_SPEED ? 0 : abs(speedFixed) * _speedK + _speedB;
	return motorSpeed;
}

byte RRFreenove4WDCarDriver::getMotorSpeed(byte speed, byte turnSpeed)
{	
	byte speedFixed = constrain(speed, MIN_SPEED, MAX_SPEED);
	byte motorSpeed = abs(speed) < MIN_SPEED ? 0 : speedFixed * _speedK + _speedB;
	motorSpeed = constrain(motorSpeed - turnSpeed, MIN_SPEED, MAX_SPEED);
	return motorSpeed;
}

byte RRFreenove4WDCarDriver::getTurnSpeed(byte turnValue)
{
	turnValue = abs(turnValue);
	byte speedTurn = constrain(turnValue, MIN_TURN_VALUE, MAX_TURN_VALUE);
	byte motorSpeed = turnValue < MIN_TURN_VALUE ? 0 : speedTurn * _turnK + _turnB;
	return motorSpeed;
}

void RRFreenove4WDCarDriver::stop()
{
	_motorLeft.stop();
	_motorRight.stop();

	leds(0, 0);
}

void RRFreenove4WDCarDriver::forward(byte speed)
{
	byte motorSpeed = getMotorSpeed(speed);
	leds(motorSpeed, 0);
	_motorLeft.move(motorSpeed, _motorDirection);
	_motorRight.move(motorSpeed, !_motorDirection);
}

void RRFreenove4WDCarDriver::forwardLeft(byte speed, byte turnValue)
{
	byte turnSpeed = getTurnSpeed(turnValue);
	byte motorSpeedLeft = getMotorSpeed(speed, turnSpeed);
	byte motorSpeedRight = getMotorSpeed(speed);
	leds(motorSpeedRight, turnValue);
	_motorLeft.move(motorSpeedLeft, _motorDirection);
	_motorRight.move(motorSpeedRight, !_motorDirection);
}

void RRFreenove4WDCarDriver::forwardRight(byte speed, byte turnValue)
{
	byte turnSpeed = getTurnSpeed(turnValue);
	byte motorSpeedLeft = getMotorSpeed(speed);
	byte motorSpeedRight = getMotorSpeed(speed, turnSpeed);
	leds(motorSpeedLeft, turnValue);
	_motorLeft.move(motorSpeedLeft, _motorDirection);
	_motorRight.move(motorSpeedRight, !_motorDirection);
}

void RRFreenove4WDCarDriver::backward(byte speed)
{
	byte motorSpeed = getMotorSpeed(speed);
	leds(-1 * speed, 0);
	_motorLeft.move(motorSpeed, !_motorDirection);
	_motorRight.move(motorSpeed, _motorDirection);
}

void RRFreenove4WDCarDriver::backwardLeft(byte speed, byte turnValue)
{	
	byte turnSpeed = getTurnSpeed(turnValue);
	byte motorSpeedLeft = getMotorSpeed(speed, turnSpeed);
	byte motorSpeedRight = getMotorSpeed(speed);
	leds(-1 * speed, turnValue);
	_motorLeft.move(motorSpeedLeft, !_motorDirection);
	_motorRight.move(motorSpeedRight, _motorDirection);
}

void RRFreenove4WDCarDriver::backwardRight(byte speed, byte turnValue)
{
	byte turnSpeed = getTurnSpeed(turnValue);
	byte motorSpeedLeft = getMotorSpeed(speed);
	byte motorSpeedRight = getMotorSpeed(speed, turnSpeed);
	leds(-1 * speed, turnValue);
	_motorLeft.move(motorSpeedLeft, !_motorDirection);
	_motorRight.move(motorSpeedRight, _motorDirection);
}

void RRFreenove4WDCarDriver::rotateLeft(byte speed)
{	
	byte motorSpeed = getMotorSpeed(speed);
	leds(motorSpeed, -1 * MAX_TURN_VALUE);
	_motorLeft.move(motorSpeed, !_motorDirection);
	_motorRight.move(motorSpeed, !_motorDirection);
}

void RRFreenove4WDCarDriver::rotateRight(byte speed)
{
	byte motorSpeed = getMotorSpeed(speed);
	leds(motorSpeed, MAX_TURN_VALUE);
	_motorLeft.move(motorSpeed, _motorDirection);
	_motorRight.move(motorSpeed, _motorDirection);
}

void RRFreenove4WDCarDriver::leds(int speedValue, int turnValue) 
{
	if(abs(speedValue) < MIN_SPEED && abs(turnValue) < MIN_TURN_VALUE){
		_strip.setLedColor(0, Dark);
		_strip.setLedColor(1, Blue);
		_strip.setLedColor(2, Blue);
		_strip.setLedColor(3, Blue);
		_strip.setLedColor(4, Dark);
		_strip.setLedColor(5, Dark);
		_strip.setLedColor(6, Red);
		_strip.setLedColor(7, Red);
		_strip.setLedColor(8, Red);
		_strip.setLedColor(9, Dark);
		return;
	}

  if (speedValue < 0) {
    _strip.setLedColor(1, Green);
    _strip.setLedColor(2, Green);
    _strip.setLedColor(3, Green);
    _strip.setLedColor(6, White);
    _strip.setLedColor(7, RedDark);
    _strip.setLedColor(8, White);
  } else {
    _strip.setLedColor(1, White);
    _strip.setLedColor(2, White);
    _strip.setLedColor(3, White);
    _strip.setLedColor(6, RedDark);
    _strip.setLedColor(7, RedDark);
    _strip.setLedColor(8, RedDark);
  }

  if (turnValue > 1) {
    _strip.setLedColor(0, Yellow);
    _strip.setLedColor(1, Yellow);
    _strip.setLedColor(8, Yellow);
    _strip.setLedColor(9, Yellow);
  } else if (turnValue < -1) {
    _strip.setLedColor(3, Yellow);
    _strip.setLedColor(4, Yellow);
    _strip.setLedColor(5, Yellow);
    _strip.setLedColor(6, Yellow);
  }
}

void RRFreenove4WDCarDriver::setLedColor(byte led, LedColor color){
	_strip.setLedColor(led, color);		
}

void RRFreenove4WDCarDriver::setLedsColor(byte leds[], LedColor color){
	for (int i = 0; i < LedsCount; i++)
	{
		setLedColor(leds[i], color);
	}	
}

void RRFreenove4WDCarDriver::setAllLedsColor(LedColor color){
	_strip.setAllLedsColor(color);
}
