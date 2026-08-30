/*
  RR_Freenove4WDCar_Sonar.h - Library for Arduino Sonar HC-SR04.
  Created by Roman Rainov, Janaury 29, 2024.
  Released into the private domain.
*/
#ifndef RR_Freenove4WDCar_Sonar_h
#define RR_Freenove4WDCar_Sonar_h

#include "Arduino.h"

class RRFreenove4WDCarSonar
{
private:
	// The speed of sound is 343 m/s or 29.15 microseconds per centimeter.
	const float PingTimeToDistanceRatio = 58.3;
	byte _trigPin;
	byte _echoPin;
	float _maxDistance;

public:
	RRFreenove4WDCarSonar(byte trigPin, byte echoPin, float maxDistance);
	void setup();
	float getDistance();
};

#endif