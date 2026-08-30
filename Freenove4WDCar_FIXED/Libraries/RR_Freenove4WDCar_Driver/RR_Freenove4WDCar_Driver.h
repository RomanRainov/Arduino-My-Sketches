/*
  RR_Freenove4WDCar_Driver.h - Library for Arduino Freenove4WDCar.
  Created by Roman Rainov, Janaury 29, 2024.
  Released into the private domain.
*/
#ifndef RR_Freenove4WDCar_Driver_h
#define RR_Freenove4WDCar_Driver_h



#include "Arduino.h"
#include "Freenove_WS2812B_RGBLED_Controller.h"
#include "RR_Freenove4WDCar_Motor.h"

enum LedColor 
{
	Dark = 0x000000,
	White = 0xFFFFFF,
	Yellow = 0xFFFF00,
	Red = 0xFF0000,
	RedDark = 0x770000,
	Blue = 0x0000FF,
	Green = 0x00FF00
};

const byte LedsCount = 10;

class RRFreenove4WDCarDriver
{
private:
	bool _motorDirection = 1;
	RRFreenove4WDCarMotor _motorLeft;
	RRFreenove4WDCarMotor _motorRight;	
	Freenove_WS2812B_Controller _strip;
	float _speedK = (float)(RRFreenove4WDCarMotor::MAX_SPEED - RRFreenove4WDCarMotor::MIN_SPEED) / (float)(MAX_SPEED - MIN_SPEED);
	float _speedB = RRFreenove4WDCarMotor::MIN_SPEED - _speedK * MIN_SPEED;
	float _turnK = (float)(RRFreenove4WDCarMotor::MAX_SPEED - RRFreenove4WDCarMotor::MIN_SPEED) / (float)(MAX_TURN_VALUE - MIN_TURN_VALUE);
	float _turnB = RRFreenove4WDCarMotor::MIN_SPEED - _turnK * MIN_TURN_VALUE;

	byte getMotorSpeed(byte speed);
	byte getMotorSpeed(byte speed, byte turnSpeed);
	byte getTurnSpeed(byte turnValue);
	
	void leds(int speedValue, int turnValue);

public:
	static const byte MIN_SPEED = 5;
	static const byte MAX_SPEED = 100;
	static const byte MIN_TURN_VALUE = 5;
	static const byte MAX_TURN_VALUE = 100;
	static const byte I2C_ADDRESS = 0x20;
	static const byte LEDS_COUNT = 10;
		
	RRFreenove4WDCarDriver(
		bool motorDirection,
		byte pinDirectionLeft,
		byte pinDirectionRight,
		byte pinMotorPwmLeft,
		byte pinMotorPwmRight,
		byte pinBattery);

	void setup();
	void stop();

	void forward(byte speed);
	void forwardLeft(byte speed, byte turnValue);
	void forwardRight(byte speed, byte turnValue);

	void backward(byte speed);	
	void backwardLeft(byte speed, byte turnValue);
	void backwardRight(byte speed, byte turnValue);

	void rotateLeft(byte speed);
	void rotateRight(byte speed);

	void setLedColor(byte led, LedColor color);
	void setLedsColor(byte leds[], LedColor color);
	void setAllLedsColor(LedColor color);
};

#endif