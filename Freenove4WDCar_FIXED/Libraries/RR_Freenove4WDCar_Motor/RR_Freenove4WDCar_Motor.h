/*
  RR_Freenove4WDCar_Motor.h - Library for Arduino Motor.
  Created by Roman Rainov, Janaury 29, 2024.
  Released into the private domain.
*/
#ifndef RR_Freenove4WDCar_Motor_h
#define RR_Freenove4WDCar_Motor_h

#define SERIAL_DEBUG_RRFreenove4WDCarMotor

#include "Arduino.h"
#include "RR_Freenove4WDCar_Battery.h"

class RRFreenove4WDCarMotor
{
private:
	byte _currentSpeed = 0;
	bool _motorDirection = true;
	byte _pinDirection = 4;
	byte _pinMotorPwm = 6;
	RRFreenove4WDCarBattery _battery;

	float getVoltageCompensation();
	byte getFixedSpeed(byte speed);

public:
	static const byte MIN_SPEED = 80;
	static const byte MAX_SPEED = 250;
	static const byte START_SPEED = 100;
	const float MAX_VOLTAGE = 7.4;
	const byte SPEED_OFFSET_PER_V = 35; //this is tested value. 35 or maybe 20

	RRFreenove4WDCarMotor();
	RRFreenove4WDCarMotor(byte pinDirection, byte pinMotorPwm, byte pinBattery);
	void setup();
	void stop();
	void move(byte speed, bool direction);
	byte currentSpeed();
};

#endif