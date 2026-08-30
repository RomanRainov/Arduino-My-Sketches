/*
  RR_Freenove4WDCar_Sonar.cpp - Library for Arduino Sonar HC-SR04.
  Created by Roman Rainov, Janaury 29, 2024.
  Released into the private domain.
*/

#include "Arduino.h"
#include "RR_Freenove4WDCar_Sonar.h"

RRFreenove4WDCarSonar::RRFreenove4WDCarSonar(byte trigPin, byte echoPin, float maxDistance)
{
	_trigPin = trigPin;
	_echoPin = echoPin;
	_maxDistance = maxDistance;
}

void RRFreenove4WDCarSonar::setup()
{
	pinMode(_trigPin, OUTPUT); // set trigPin to output mode
	pinMode(_echoPin, INPUT);  // set echoPin to input mode
}

// The speed of sound is 343 m/s or 29.15 microseconds per centimeter.
// The ping travels out and back, so to find the distance of the
// object we take half of the distance travelled.
float RRFreenove4WDCarSonar::getDistance()
{
	unsigned long pingTime;
	float distance;
	digitalWrite(_trigPin, LOW);
	delayMicroseconds(5);
	digitalWrite(_trigPin, HIGH); // make trigPin output high level lasting for 10μs to triger HC_SR04,
	delayMicroseconds(10);
	digitalWrite(_trigPin, LOW);
	const unsigned long timeoutUs = (unsigned long)(_maxDistance * PingTimeToDistanceRatio) + 500UL;
	pingTime = pulseIn(_echoPin, HIGH, timeoutUs); // bounded wait: keep receiver and mode switching responsive
	distance = pingTime != 0
				   ? (float)pingTime / PingTimeToDistanceRatio // calculate the distance according to the time
				   : _maxDistance;
	return distance; // return the distance value
}