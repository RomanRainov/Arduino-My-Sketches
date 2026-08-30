/**********************************************************************
  Filename    : Automatic_Obstacle_Avoidance.ino
  Description : Obstacle avoidance for FlySky_4WDCar_COMPLEX.

  Behaviour:
    - cruise with sonar pointed forward;
    - stop when the obstacle is 25 cm or nearer;
    - scan left and right while fully stopped;
    - reverse only when the front distance is below 10 cm;
    - rotate toward the clearer side until the forward path is clear.
**********************************************************************/

// Distances are measured from the sonar, not from the bumper.
#define OA_STOP_DISTANCE_CM             25
#define OA_RESUME_DISTANCE_CM           35
#define OA_EMERGENCY_DISTANCE_CM        10
#define OA_BACK_RELEASE_DISTANCE_CM     16
#define OA_TURN_CLEAR_DISTANCE_CM       40

// RR driver command scale is 5..100.
#define OA_CRUISE_SPEED                 15
#define OA_BACK_SPEED                   60
#define OA_TURN_SPEED                   85

#define OA_CENTER_SETTLE_MS            120
#define OA_STOP_SETTLE_MS               80
#define OA_SIDE_SETTLE_MS              170
#define OA_SAMPLE_INTERVAL_MS           32
#define OA_SAMPLES_PER_DIRECTION         3

// Give the motors time to overcome the static friction of 4WD skid steering.
#define OA_BACK_MIN_TIME_MS             280
#define OA_BACK_MAX_TIME_MS             800
#define OA_TURN_MIN_TIME_MS             650
#define OA_TURN_MAX_TIME_MS            2800
#define OA_TURN_SAMPLE_INTERVAL_MS       70
#define OA_CLEAR_CONFIRM_SAMPLES          2

static const uint8_t OA_ANGLE_LEFT   = (uint8_t)(SERVO_START_POS + 55);
static const uint8_t OA_ANGLE_CENTER = (uint8_t)(SERVO_START_POS);
static const uint8_t OA_ANGLE_RIGHT  = (uint8_t)(SERVO_START_POS - 55);

enum OAState : uint8_t {
  OA_CENTERING_FOR_CRUISE,
  OA_CRUISE,
  OA_STOP_SETTLE,
  OA_CONFIRM_FRONT,
  OA_WAIT_LEFT,
  OA_READ_LEFT,
  OA_WAIT_RIGHT,
  OA_READ_RIGHT,
  OA_CENTER_BEFORE_MANEUVER,
  OA_BACKING,
  OA_TURNING_LEFT,
  OA_TURNING_RIGHT
};

static OAState oa_state = OA_CENTERING_FOR_CRUISE;
static uint32_t oa_deadline = 0;
static uint32_t oa_motion_started_at = 0;
static uint32_t oa_next_sample_at = 0;

static uint8_t oa_sample_index = 0;
static uint8_t oa_clear_sample_count = 0;
static uint16_t oa_samples[OA_SAMPLES_PER_DIRECTION] = {
  MAX_DISTANCE, MAX_DISTANCE, MAX_DISTANCE
};

static uint16_t oa_front_cm = MAX_DISTANCE;
static uint16_t oa_left_cm  = MAX_DISTANCE;
static uint16_t oa_right_cm = MAX_DISTANCE;
static bool oa_turn_left = true;
static bool oa_back_before_turn = false;

static inline bool oa_time_reached(uint32_t now, uint32_t deadline) {
  return (int32_t)(now - deadline) >= 0;
}

static inline uint32_t oa_elapsed(uint32_t now, uint32_t startedAt) {
  return (uint32_t)(now - startedAt);
}

static uint16_t oa_read_distance_cm() {
  const float value = sonar.getDistance();
  if (value <= 0.0f || value >= MAX_DISTANCE) {
    return MAX_DISTANCE;
  }
  return (uint16_t)(value + 0.5f);
}

static uint16_t oa_median3(uint16_t a, uint16_t b, uint16_t c) {
  if (a > b) { const uint16_t t = a; a = b; b = t; }
  if (b > c) { const uint16_t t = b; b = c; c = t; }
  if (a > b) { const uint16_t t = a; a = b; b = t; }
  return b;
}

static void oa_begin_samples(uint32_t now) {
  oa_sample_index = 0;
  oa_next_sample_at = now;
}

static bool oa_collect_median(uint32_t now, uint16_t *result) {
  if (!oa_time_reached(now, oa_next_sample_at)) {
    return false;
  }

  oa_samples[oa_sample_index++] = oa_read_distance_cm();
  oa_next_sample_at = millis() + OA_SAMPLE_INTERVAL_MS;

  if (oa_sample_index < OA_SAMPLES_PER_DIRECTION) {
    return false;
  }

  *result = oa_median3(oa_samples[0], oa_samples[1], oa_samples[2]);
  return true;
}

static void oa_begin_centering(uint32_t now) {
  driver.stop();
  servo.setPosition(OA_ANGLE_CENTER);
  oa_state = OA_CENTERING_FOR_CRUISE;
  oa_deadline = now + OA_CENTER_SETTLE_MS;
}

static void oa_begin_obstacle_scan(uint32_t now, uint16_t firstFrontReading) {
  driver.stop();
  oa_front_cm = firstFrontReading;
  servo.setPosition(OA_ANGLE_CENTER);
  oa_state = OA_STOP_SETTLE;
  oa_deadline = now + OA_STOP_SETTLE_MS;
  driver.setAllLedsColor(LedColor::Red);
}

static void oa_begin_turn(uint32_t now) {
  oa_state = oa_turn_left ? OA_TURNING_LEFT : OA_TURNING_RIGHT;
  oa_motion_started_at = now;
  oa_next_sample_at = now + OA_TURN_MIN_TIME_MS;
  oa_clear_sample_count = 0;
  driver.setAllLedsColor(LedColor::Yellow);
}

static void oa_begin_backing(uint32_t now) {
  oa_state = OA_BACKING;
  oa_motion_started_at = now;
  oa_next_sample_at = now + OA_BACK_MIN_TIME_MS;
  driver.setAllLedsColor(LedColor::Red);
}

static void oa_prepare_manoeuvre(uint32_t now) {
  oa_turn_left = oa_left_cm >= oa_right_cm;

  // The user-requested rule is strict: reverse only below 10 cm.
  oa_back_before_turn = oa_front_cm < OA_EMERGENCY_DISTANCE_CM;

  servo.setPosition(OA_ANGLE_CENTER);
  oa_state = OA_CENTER_BEFORE_MANEUVER;
  oa_deadline = now + OA_CENTER_SETTLE_MS;
}

static void oa_apply_motion() {
  switch (oa_state) {
    case OA_CRUISE:
      driver.forward(OA_CRUISE_SPEED);
      break;

    case OA_BACKING:
      driver.backward(OA_BACK_SPEED);
      break;

    case OA_TURNING_LEFT:
      driver.rotateLeft(OA_TURN_SPEED);
      break;

    case OA_TURNING_RIGHT:
      driver.rotateRight(OA_TURN_SPEED);
      break;

    default:
      driver.stop();
      break;
  }
}

void init_Automatic_Obstacle_Avoidance() {
  buzzer.off();
  driver.stop();

  oa_front_cm = MAX_DISTANCE;
  oa_left_cm = MAX_DISTANCE;
  oa_right_cm = MAX_DISTANCE;
  oa_turn_left = true;
  oa_back_before_turn = false;
  oa_clear_sample_count = 0;

  oa_begin_centering(millis());
}

void loop_Automatic_Obstacle_Avoidance() {
  uint32_t now = millis();

  switch (oa_state) {
    case OA_CENTERING_FOR_CRUISE:
      if (oa_time_reached(now, oa_deadline)) {
        oa_state = OA_CRUISE;
        oa_next_sample_at = now;
        driver.setAllLedsColor(LedColor::Green);
      }
      break;

    case OA_CRUISE:
      if (oa_time_reached(now, oa_next_sample_at)) {
        const uint16_t distance = oa_read_distance_cm();
        now = millis();
        oa_next_sample_at = now + OA_SAMPLE_INTERVAL_MS;

        if (distance <= OA_STOP_DISTANCE_CM) {
          oa_begin_obstacle_scan(now, distance);
        }
      }
      break;

    case OA_STOP_SETTLE:
      if (oa_time_reached(now, oa_deadline)) {
        oa_state = OA_CONFIRM_FRONT;
        oa_begin_samples(now);
      }
      break;

    case OA_CONFIRM_FRONT:
      if (oa_collect_median(now, &oa_front_cm)) {
        now = millis();

        // Ignore one false short echo and resume only with a useful margin.
        if (oa_front_cm >= OA_RESUME_DISTANCE_CM) {
          oa_begin_centering(now);
        } else {
          servo.setPosition(OA_ANGLE_LEFT);
          oa_state = OA_WAIT_LEFT;
          oa_deadline = now + OA_SIDE_SETTLE_MS;
        }
      }
      break;

    case OA_WAIT_LEFT:
      if (oa_time_reached(now, oa_deadline)) {
        oa_state = OA_READ_LEFT;
        oa_begin_samples(now);
      }
      break;

    case OA_READ_LEFT:
      if (oa_collect_median(now, &oa_left_cm)) {
        now = millis();
        servo.setPosition(OA_ANGLE_RIGHT);
        oa_state = OA_WAIT_RIGHT;
        oa_deadline = now + OA_SIDE_SETTLE_MS;
      }
      break;

    case OA_WAIT_RIGHT:
      if (oa_time_reached(now, oa_deadline)) {
        oa_state = OA_READ_RIGHT;
        oa_begin_samples(now);
      }
      break;

    case OA_READ_RIGHT:
      if (oa_collect_median(now, &oa_right_cm)) {
        oa_prepare_manoeuvre(millis());
      }
      break;

    case OA_CENTER_BEFORE_MANEUVER:
      if (oa_time_reached(now, oa_deadline)) {
        if (oa_back_before_turn) {
          oa_begin_backing(now);
        } else {
          oa_begin_turn(now);
        }
      }
      break;

    case OA_BACKING: {
      const uint32_t elapsed = oa_elapsed(now, oa_motion_started_at);

      if (oa_time_reached(now, oa_next_sample_at)) {
        oa_front_cm = oa_read_distance_cm();
        now = millis();
        oa_next_sample_at = now + OA_SAMPLE_INTERVAL_MS;
      }

      if ((elapsed >= OA_BACK_MIN_TIME_MS &&
           oa_front_cm >= OA_BACK_RELEASE_DISTANCE_CM) ||
          elapsed >= OA_BACK_MAX_TIME_MS) {
        oa_begin_turn(now);
      }
      break;
    }

    case OA_TURNING_LEFT:
    case OA_TURNING_RIGHT: {
      const uint32_t elapsed = oa_elapsed(now, oa_motion_started_at);

      // Do not ping during the initial turn. This lets the motor ramp reach
      // useful torque instead of spending most loops waiting in pulseIn().
      if (elapsed >= OA_TURN_MIN_TIME_MS &&
          oa_time_reached(now, oa_next_sample_at)) {
        oa_front_cm = oa_read_distance_cm();
        now = millis();
        oa_next_sample_at = now + OA_TURN_SAMPLE_INTERVAL_MS;

        if (oa_front_cm >= OA_TURN_CLEAR_DISTANCE_CM) {
          if (oa_clear_sample_count < 255) {
            oa_clear_sample_count++;
          }
        } else {
          oa_clear_sample_count = 0;
        }
      }

      if (elapsed >= OA_TURN_MIN_TIME_MS &&
          oa_clear_sample_count >= OA_CLEAR_CONFIRM_SAMPLES) {
        oa_begin_centering(now);
      } else if (elapsed >= OA_TURN_MAX_TIME_MS) {
        // The selected side did not clear. Stop, rescan, and choose again.
        // This timeout prevents endless spinning if the car is physically wedged.
        oa_begin_obstacle_scan(now, oa_front_cm);
      }
      break;
    }
  }

  // Repeated commands are required by RRFreenove4WDCarMotor's smooth ramp.
  oa_apply_motion();
}
