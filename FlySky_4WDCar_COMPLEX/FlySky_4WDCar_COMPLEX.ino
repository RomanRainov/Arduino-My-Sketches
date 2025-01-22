#include <iBus.h>
#include "RR_Freenove4WDCar_Driver.h"
#include "RR_Freenove4WDCar_Servo.h"
#include "Freenove_WS2812B_RGBLED_Controller.h"

#define MOTOR_DIRECTION 1  //If the direction is reversed, change 0 to 1
#define PIN_SERVO 2  //define servo pin
#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT 4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT 6
#define PIN_SONIC_TRIG      7
#define PIN_SONIC_ECHO      8
#define PIN_BATTERY A0
//----BUZZER
#define PIN_BUZZER      A0
//---LEDS
#define I2C_ADDRESS  0x20
#define LEDS_COUNT   10  //it defines number of lEDs. 


#define SERVO_STEP 10
#define SERVO_OFFSET 5
#define SERVO_START_POS 90 + SERVO_OFFSET

RRFreenove4WDCarDriver driver(MOTOR_DIRECTION, PIN_DIRECTION_LEFT, PIN_DIRECTION_RIGHT, PIN_MOTOR_PWM_LEFT, PIN_MOTOR_PWM_RIGHT, PIN_BATTERY);
RRFreenove4WDCarServo servo(PIN_SERVO, SERVO_OFFSET, SERVO_START_POS);
iBus receiver(Serial, MAX_CHANNELS);  // //Serial on ATMega328 boards(Uno, Nano, etc.)



void setup() {
  // put your setup code here, to run once:
  //Serial.begin(115200);
  receiver.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  driver.setup();
  servo.setup();
}

void loop() {

  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, Serial.available());

  receiver.process();
  if (receiver.get(0) == 0)
    return;

  //proccessChannel12();
  //proccessChannel3();
  //proccessChannel4();
  proccessChannel5();
  proccessChannel6();

  delay(1000);
}


void proccessChannel5(){
  int channelValue = receiver.get(5);
  Serial.println("Channel5 - " + String(channelValue));
}

void proccessChannel6(){
  int channelValue = receiver.get(6);
  Serial.println("Channel6 - " + String(channelValue));
}

