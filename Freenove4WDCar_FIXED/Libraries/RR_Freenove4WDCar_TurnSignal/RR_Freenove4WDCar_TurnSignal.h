/*
  RR_Freenove4WDCar_TurnSignal.h - Library for Arduino 4WD Car.
  Created by Roman Rainov, May 19, 2024.
  Released into the private domain.
*/
#ifndef RR_Freenove4WDCar_TurnSignal_h
#define RR_Freenove4WDCar_TurnSignal_h

#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_2     true

#include "TimerInterrupt.h"

#include "Arduino.h"
#include "Freenove_WS2812B_RGBLED_Controller.h"

#define I2C_ADDRESS 0x20
#define LEDS_COUNT  10

#define TIMER_INTERVAL_MS 500L

enum TurnLeds{
  LEDS_OFF = 0,
  LEDS_LEFT = 1,
  LEDS_RIGHT = 1 << 1,
  LEDS_ALL = LEDS_LEFT | LEDS_RIGHT
};

class RRFreenove4WDCarTurnSignal
{
  private:
  bool _toggle = false;
  bool _turnedOn = false;
  TurnLeds _leds = LEDS_OFF;
  Freenove_WS2812B_Controller _strip;
  
  static void TimerHandler(RRFreenove4WDCarTurnSignal* objectAddress)
  {
    objectAddress->toggleLeds();
  };

  void toggleLeds()
  {
    if(!_turnedOn)
      return;

     _toggle = !_toggle;
     u32 color = _toggle ? FlashColor : DefaultColor;
     
    if((_leds & LEDS_LEFT) == LEDS_LEFT)
    {
      Serial.println("\t LEDS_LEFT\t");
      //setLeftColor(color);
    }
     
    if((_leds & LEDS_RIGHT) == LEDS_RIGHT)
    {
      Serial.println("\t LEDS_RIGHT\t");
      //setRightColor(color);
    }
  }

  void setLeftColor(u32 color)
  {
    _strip.setLedColor(0, color);
    _strip.setLedColor(1, color);
    _strip.setLedColor(8, color);
    _strip.setLedColor(9, color);
  }

  void setRightColor(u32 color)
  {
    _strip.setLedColor(3, color);
    _strip.setLedColor(4, color);
    _strip.setLedColor(5, color);
    _strip.setLedColor(6, color);
  }

public:
	u32 DefaultColor = 0;
	u32 FlashColor = 0;

	RRFreenove4WDCarTurnSignal(u32 defaultColor = 0, u32 flashColor = 0xFFFF00)
  {
    _strip = new Freenove_WS2812B_Controller(I2C_ADDRESS, LEDS_COUNT, TYPE_GRB);
     DefaultColor = defaultColor;
     FlashColor = flashColor;
  }

  void setup()
  {
    while (!_strip.begin());
    // Select Timer 1-2 for UNO, 0-5 for MEGA, 1,3,4 for 32u4
    // Timer 2 is 8-bit timer, only for higher frequency
    ITimer2.init();

    if (ITimer2.attachInterruptInterval(TIMER_INTERVAL_MS, TimerHandler, this))
    {
      Serial.print(F("Starting  ITimer2 OK, millis() = ")); 
      Serial.println(millis());
    }
    else
      Serial.println(F("Can't set ITimer2. Select another freq. or timer"));
  }
	
	void turnSignalOn(TurnLeds leds)
  {
     //setLeftColor(0xFFFFFF);
    Serial.print(F("\t-turnSignalOn-\t")); 
    _leds = leds;
    _turnedOn = true;
  }

	void turnSignalOff()
  {
    //setLeftColor(0x090107);
    Serial.print(F("\t-turnSignalOff-\t")); 
    _turnedOn = false;
  }
  
};




#endif