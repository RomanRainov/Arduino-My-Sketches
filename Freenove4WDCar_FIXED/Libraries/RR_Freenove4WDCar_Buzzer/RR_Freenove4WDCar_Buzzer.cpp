/*
  RR_Freenove4WDCar_Buzzer.cpp - Library for Arduino Buzzer.
  Created by Roman Rainov, Janaury 29, 2024.
  Released into the private domain.
*/

#include "Arduino.h"
#include "RR_Freenove4WDCar_Buzzer.h"

RRFreenove4WDCarBuzzer::RRFreenove4WDCarBuzzer()
{
}

RRFreenove4WDCarBuzzer::RRFreenove4WDCarBuzzer(byte pinBuzzer)
{
	_pinBuzzer = pinBuzzer;
}

void RRFreenove4WDCarBuzzer::on()
{
	pinMode(_pinBuzzer, OUTPUT);
	digitalWrite(_pinBuzzer, HIGH); // turn on buzzer
}

void RRFreenove4WDCarBuzzer::off()
{
	digitalWrite(_pinBuzzer, LOW); // turn off buzzer
}

void RRFreenove4WDCarBuzzer::beep(unsigned int delaySeconds)
{
	pinMode(_pinBuzzer, OUTPUT);
	digitalWrite(_pinBuzzer, HIGH); // turn on buzzer
	delay(delaySeconds);
	digitalWrite(_pinBuzzer, LOW); // turn off buzzer
}

void RRFreenove4WDCarBuzzer::toneOn(unsigned int frequency)
{
	tone(_pinBuzzer, frequency);
}

void RRFreenove4WDCarBuzzer::toneOn(unsigned int frequency, unsigned long duration)
{
	tone(_pinBuzzer, frequency, duration);
}

void RRFreenove4WDCarBuzzer::toneOff()
{
	noTone(_pinBuzzer);
}