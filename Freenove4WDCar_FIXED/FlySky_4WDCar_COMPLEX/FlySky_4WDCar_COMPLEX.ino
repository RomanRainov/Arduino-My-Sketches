#include <iBus.h>
#include "RR_Freenove4WDCar_Buzzer.h"
#include "RR_Freenove4WDCar_Driver.h"
#include "RR_Freenove4WDCar_Servo.h"
#include "RR_Freenove4WDCar_Sonar.h"
#include "Freenove_WS2812B_RGBLED_Controller.h"

// --------------------- Pins & constants (match your wiring)
#define MOTOR_DIRECTION 1
#define PIN_SERVO 2
#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT 4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT 6
#define PIN_SONIC_TRIG 7
#define PIN_SONIC_ECHO 8
#define PIN_BATTERY A0
#define PIN_BUZZER A0   // keep as in your project if used; adjust if needed

// Servo
#define SERVO_STEP 10
#define SERVO_OFFSET 5
#define SERVO_START_POS (90 + SERVO_OFFSET)

// Sonar
#define MAX_DISTANCE 1000
#define SONIC_TIMEOUT (MAX_DISTANCE * 60L)

// --------------------- Global hardware objects
RRFreenove4WDCarSonar  sonar(PIN_SONIC_TRIG, PIN_SONIC_ECHO, MAX_DISTANCE);
RRFreenove4WDCarBuzzer buzzer(PIN_BUZZER);
RRFreenove4WDCarDriver driver(MOTOR_DIRECTION,
                              PIN_DIRECTION_LEFT, PIN_DIRECTION_RIGHT,
                              PIN_MOTOR_PWM_LEFT, PIN_MOTOR_PWM_RIGHT,
                              PIN_BATTERY);
RRFreenove4WDCarServo  servo(PIN_SERVO, SERVO_OFFSET, SERVO_START_POS);

// FlySky receiver on main Serial (Uno)
iBus receiver(Serial, MAX_CHANNELS);

// --------------------- Channels container
struct FlySkyChannelsValues {
  int Throttle = 0;  // ch3
  int Rudder   = 0;  // ch4
  int Aileron  = 0;  // ch1
  int Elevator = 0;  // ch2
  int SWC      = 1000; // ch5 (3-position switch)
  int SWA      = 1000; // ch6
} flySkyChannelsValues;

// --------------------- Modes
enum class CarMode {
  FreeDrive,
  Automatic_Stay_Close_To_Obstacle,
  Automatic_Obstacle_Avoidance
};

CarMode currentMode = CarMode::FreeDrive;

// --------------------- Prototypes for mode modules (implemented in .ino files)
void initFreeDrive();
void loopFreeDrive();

void initAutomatic_Stay_Close_To_Obstacle();
void loopAutomatic_Stay_Close_To_Obstacle();

void init_Automatic_Obstacle_Avoidance();
void loop_Automatic_Obstacle_Avoidance();

void readFlySkyChannels();

// ===================== Mode indication (sound + LEDs) =====================

// Blink helper using driver LEDs (turn all LEDs to color and off)
static inline void blinkColor(LedColor c, uint8_t times, uint16_t onMs=200, uint16_t offMs=150) {
  for (uint8_t i = 0; i < times; ++i) {
    driver.setAllLedsColor(c);
    delay(onMs);
    driver.setAllLedsColor(LedColor::Dark); // off
    if (i + 1 < times) delay(offMs);
  }
}

// Play sound + LED pattern for the selected mode
void indicateMode(CarMode mode) {
  switch (mode) {
    case CarMode::FreeDrive:
      // 1 short beep + 1 green blink
      buzzer.beep(180);
      blinkColor(LedColor::Green, 1);
      break;

    case CarMode::Automatic_Stay_Close_To_Obstacle:
      // 2 short beeps + 2 yellow blinks
      buzzer.beep(150); delay(90); buzzer.beep(150);
      blinkColor(LedColor::Yellow, 2);
      break;

    case CarMode::Automatic_Obstacle_Avoidance:
      // 1 long beep + 3 red blinks
      buzzer.beep(450);
      blinkColor(LedColor::Red, 3, 220, 150);
      break;
  }
}

// ===================== SWC → CarMode with hysteresis =======================
// Tune these thresholds to your radio if needed
const int SWC_LOW   = 1250;  // exit MID back to LOW
const int SWC_HIGH = 1750;  // enter HIGH from MID

// Stateful mapping: returns desired mode from SWC value with hysteresis
// Bands: (-inf..1250)->LOW, [1250..1750)->MID, [1750..inf)->HIGH
static inline CarMode desiredModeFromSWC(int swcValue) {
  if (swcValue >= SWC_HIGH) return CarMode::Automatic_Stay_Close_To_Obstacle;   // 2000
  if (swcValue >= SWC_LOW) return CarMode::Automatic_Obstacle_Avoidance;       // 1500
  return CarMode::FreeDrive;                                                // 1000
}


// ===================== Setup / Loop ========================================
void setup() {
  // Do NOT start Serial here (Serial is used by iBus)
  pinMode(LED_BUILTIN, OUTPUT);

  receiver.begin();
  driver.setup();
  servo.setup();
  sonar.setup();

  currentMode = CarMode::FreeDrive;
  initFreeDrive();
  indicateMode(currentMode);  // indicate startup mode
}

void loop() {
  // Small activity indicator on builtin LED (lights when serial data present)
  digitalWrite(LED_BUILTIN, Serial.available());

  receiver.process();
  if (receiver.get(0) == 0)  // receiver not ready
    return;

  readFlySkyChannels();

  // 1) Compute desired mode directly (with hysteresis)
  CarMode desired = desiredModeFromSWC(flySkyChannelsValues.SWC);

  // 2) If mode changed → run init and indicate
  if (desired != currentMode) {
    // Stop the previous mode before blocking sound/LED indication.
    driver.stop();
    buzzer.off();

    currentMode = desired;

    switch (currentMode) {
      case CarMode::FreeDrive:
        initFreeDrive();
        break;
      case CarMode::Automatic_Stay_Close_To_Obstacle:
        initAutomatic_Stay_Close_To_Obstacle();
        break;
      case CarMode::Automatic_Obstacle_Avoidance:
        init_Automatic_Obstacle_Avoidance();
        break;
    }
    indicateMode(currentMode);
  }

  // 3) Run active mode loop
  switch (currentMode) {
    case CarMode::FreeDrive:
      loopFreeDrive();
      break;
    case CarMode::Automatic_Stay_Close_To_Obstacle:
      loopAutomatic_Stay_Close_To_Obstacle();
      break;
    case CarMode::Automatic_Obstacle_Avoidance:
      loop_Automatic_Obstacle_Avoidance();
      break;
  }

  delay(10); // small loop pacing
}

// ===================== Channels read =======================================
void readFlySkyChannels() {
  // Read channels from iBus (1..6 as per your mapping)
  flySkyChannelsValues.Aileron  = receiver.get(1);
  flySkyChannelsValues.Elevator = receiver.get(2);
  flySkyChannelsValues.Throttle = receiver.get(3);
  flySkyChannelsValues.Rudder   = receiver.get(4);
  flySkyChannelsValues.SWC      = receiver.get(5);
  flySkyChannelsValues.SWA      = receiver.get(6);
}
