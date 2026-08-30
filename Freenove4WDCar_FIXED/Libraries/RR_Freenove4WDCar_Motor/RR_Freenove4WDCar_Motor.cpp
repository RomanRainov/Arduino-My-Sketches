/*
  RR_Freenove4WDCar_Motor.cpp - Library for Arduino Motor HC-SR04.
  Created by Roman Rainov, Janaury 29, 2024.
  Released into the private domain.
*/

#include "Arduino.h"
#include "RR_Freenove4WDCar_Motor.h"

RRFreenove4WDCarMotor::RRFreenove4WDCarMotor()
{
}

RRFreenove4WDCarMotor::RRFreenove4WDCarMotor(byte pinDirection = 4, byte pinMotorPwm = 6, byte pinBattery = A0)
	: _battery{RRFreenove4WDCarBattery(pinBattery)}
{
	_pinDirection = pinDirection;
	_pinMotorPwm = pinMotorPwm;
}

void RRFreenove4WDCarMotor::setup()
{
	pinMode(_pinDirection, OUTPUT);
	pinMode(_pinMotorPwm, OUTPUT);
}

void RRFreenove4WDCarMotor::stop()
{
	_currentSpeed = 0;
	analogWrite(_pinMotorPwm, 0);
}

// Move motor with smooth ramp (no delay())
// - positive 'speed' means forward PWM request in [0..255] domain expected by the driver (your scale)
// - internal _currentSpeed holds the last actually written PWM value
void RRFreenove4WDCarMotor::move(byte speed, bool motorDirection)
{
  const uint8_t RAMP_UP_STEP   = 6;   // how fast we accelerate per call (tune)
  const uint8_t RAMP_DOWN_STEP = 10;  // how fast we brake per call (tune)

  byte target = getFixedSpeed(speed);      // apply voltage compensation + clamp
  int  cur    = (int)_currentSpeed;
  int  tgt    = (int)target;

  // One-shot kick when starting from full stop (no blocking delay)
  if (cur == 0 && tgt > 0 && tgt < START_SPEED) {
    cur = START_SPEED;                     // short kick to overcome static friction
  }

  // Slew limiting (per-call increment), keeps motion smooth without delay()
  if (cur < tgt) {
    cur += RAMP_UP_STEP;
    if (cur > tgt) cur = tgt;
  } else if (cur > tgt) {
    cur -= RAMP_DOWN_STEP;
    if (cur < tgt) cur = tgt;
  }

  // Apply outputs
  digitalWrite(_pinDirection, motorDirection);
  analogWrite(_pinMotorPwm, (byte)cur);
  _currentSpeed = (byte)cur;
}


byte RRFreenove4WDCarMotor::currentSpeed()
{
	return _currentSpeed;
}

// Return PWM corrected by battery voltage (non-zero only if >= MIN_SPEED)
byte RRFreenove4WDCarMotor::getFixedSpeed(byte speed)
{
  if (speed < MIN_SPEED) return 0;

  // Voltage compensation actually applied (the original early 'return' removed)
  float voltageOffset = getVoltageCompensation();   // e.g., ~ (MAX_VOLTAGE - Vbat) * k
  int   corrected     = (int)speed + (int)voltageOffset;

  byte speedFixed = (byte)constrain(corrected, (int)MIN_SPEED, (int)MAX_SPEED);

#ifdef SERIAL_DEBUG_RRFreenove4WDCarMotor
  // Serial.print("input="); Serial.print(speed);
  // Serial.print(" voff=");  Serial.print(voltageOffset);
  // Serial.print(" fixed="); Serial.println(speedFixed);
#endif

  return speedFixed;
}


float RRFreenove4WDCarMotor::getVoltageCompensation()
{
	// The PWM value represents different speeds when the voltage of battery changes.
	// So we set a speedOffset to compensate the difference.
	float voltage = _battery.getBatteryVoltage();
	float voltageOffset = MAX_VOLTAGE - voltage;
	if(voltageOffset <= 0)
		return 0;

	return voltageOffset * SPEED_OFFSET_PER_V;
}