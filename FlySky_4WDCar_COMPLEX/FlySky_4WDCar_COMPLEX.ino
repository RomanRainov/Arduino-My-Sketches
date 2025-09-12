#include <iBus.h>
#include "RR_Freenove4WDCar_Buzzer.h"
#include "RR_Freenove4WDCar_Driver.h"
#include "RR_Freenove4WDCar_Servo.h"
#include "RR_Freenove4WDCar_Sonar.h"
#include "Freenove_WS2812B_RGBLED_Controller.h"

#define MOTOR_DIRECTION 1  //If the direction is reversed, change 0 to 1
#define PIN_SERVO 2        //define servo pin
#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT 4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT 6
#define PIN_SONIC_TRIG 7
#define PIN_SONIC_ECHO 8
#define PIN_BATTERY A0
//----BUZZER
#define PIN_BUZZER A0
//---LEDS
#define I2C_ADDRESS 0x20
#define LEDS_COUNT 10  //it defines number of lEDs.
//---SERVO
#define SERVO_STEP 10
#define SERVO_OFFSET 5
#define SERVO_START_POS 90 + SERVO_OFFSET
//---SONAR
#define MAX_DISTANCE    1000
#define SONIC_TIMEOUT   (MAX_DISTANCE * 60L)

RRFreenove4WDCarSonar sonar(PIN_SONIC_TRIG, PIN_SONIC_ECHO, MAX_DISTANCE);
RRFreenove4WDCarBuzzer buzzer(A0);
RRFreenove4WDCarDriver driver(MOTOR_DIRECTION, PIN_DIRECTION_LEFT, PIN_DIRECTION_RIGHT, PIN_MOTOR_PWM_LEFT, PIN_MOTOR_PWM_RIGHT, PIN_BATTERY);
RRFreenove4WDCarServo servo(PIN_SERVO, SERVO_OFFSET, SERVO_START_POS);
iBus receiver(Serial, MAX_CHANNELS);  // //Serial on ATMega328 boards(Uno, Nano, etc.)

struct FlySkyChannelsValues {
public:
  int Throttle = 0;  //channel 3
  int Rudder = 0;    // channel 4
  int Aileron = 0;   // channel 1
  int Elevator = 0;  //channel 2
  int SWC = 1000;    // channel 5
  int SWA = 1000;    // channel 6
} flySkyChannelsValues;

enum CarMode {
  FreeDrive = 0,
  Automatic_Stay_Close_To_Obstacle = 1,
  Automatic_Obstacle_Avoidance = 2
}

currentMode = CarMode::FreeDrive;

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

  readFlySkyChannels();
  if (flySkyChannelsValues.SWC == 1500) {
    if (currentMode != CarMode::Automatic_Stay_Close_To_Obstacle) {
      currentMode = CarMode::Automatic_Stay_Close_To_Obstacle;
      initAutomatic_Stay_Close_To_Obstacle();
    }
    loopAutomatic_Stay_Close_To_Obstacle();
  } else if(flySkyChannelsValues.SWC == 2000) {
    if (currentMode != CarMode::Automatic_Obstacle_Avoidance) {
      currentMode = CarMode::Automatic_Obstacle_Avoidance;
      init_Automatic_Obstacle_Avoidance();
    }
    loop_Automatic_Obstacle_Avoidance();
  } else {
    if (currentMode != CarMode::FreeDrive) {
      currentMode = CarMode::FreeDrive;
      initFreeDrive();
    }
    loopFreeDrive();
  }  
  delay(10);
}

void readFlySkyChannels() {
  flySkyChannelsValues.Aileron = receiver.get(1);
  flySkyChannelsValues.Elevator = receiver.get(2);
  flySkyChannelsValues.Throttle = receiver.get(3);
  flySkyChannelsValues.Rudder = receiver.get(4);
  flySkyChannelsValues.SWC = receiver.get(5);
  flySkyChannelsValues.SWA = receiver.get(6);
}

