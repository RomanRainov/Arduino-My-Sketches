/**********************************************************************
  Filename    : Automatic_Stay_Close_To_Obstacle.ino
  Description : Follow a moving obstacle while keeping 30..50 cm.

  Detection principle:
    - continuously scan five directions;
    - compare the current distance map with the previous scan;
    - a large distance change marks a moving-object candidate;
    - once acquired, prefer the same or an adjacent direction so the
      tracker does not jump randomly between walls and furniture;
    - rotate toward the target and control forward/backward distance.
**********************************************************************/

#define SC_BAND_MIN_CM                 30
#define SC_BAND_MAX_CM                 50
#define SC_TARGET_CM                   40
#define SC_EMERGENCY_BACK_CM           16

#define SC_MIN_VALID_CM                 5
#define SC_ACQUIRE_MAX_CM             120
#define SC_TRACK_MAX_CM               180
#define SC_MOVEMENT_THRESHOLD_CM        6
#define SC_TARGET_LOST_MS             1300

#define SC_FORWARD_MIN_SPEED           28  // RR driver scale: 0..100
#define SC_FORWARD_MAX_SPEED           78
#define SC_BACK_MIN_SPEED              32
#define SC_BACK_MAX_SPEED              75
#define SC_TURN_MIN_SPEED              38
#define SC_TURN_MAX_SPEED              76

#define SC_SERVO_SETTLE_MS             95
#define SC_SONAR_SAMPLE_INTERVAL_MS    24
#define SC_SAMPLES_PER_ANGLE            3
#define SC_ALIGN_TOLERANCE_DEG          9

static const uint8_t SC_ANGLE_COUNT = 5;
// High servo angle is left on the Freenove mounting used by the original sketch.
static const uint8_t sc_scan_angles[SC_ANGLE_COUNT] = {
  (uint8_t)(SERVO_START_POS + 50),
  (uint8_t)(SERVO_START_POS + 25),
  (uint8_t)(SERVO_START_POS),
  (uint8_t)(SERVO_START_POS - 25),
  (uint8_t)(SERVO_START_POS - 50)
};

enum class SC_ScanPhase : uint8_t {
  SetAngle,
  WaitForServo,
  ReadSamples
};

static SC_ScanPhase sc_scan_phase = SC_ScanPhase::SetAngle;
static bool sc_scan_left_to_right = true;
static uint8_t sc_scan_order_index = 0;
static uint8_t sc_current_angle_index = 0;
static uint8_t sc_sample_index = 0;

static uint16_t sc_samples[SC_SAMPLES_PER_ANGLE] = { MAX_DISTANCE, MAX_DISTANCE, MAX_DISTANCE };
static uint16_t sc_current_distances[SC_ANGLE_COUNT] = { MAX_DISTANCE, MAX_DISTANCE, MAX_DISTANCE, MAX_DISTANCE, MAX_DISTANCE };
static uint16_t sc_previous_distances[SC_ANGLE_COUNT] = { MAX_DISTANCE, MAX_DISTANCE, MAX_DISTANCE, MAX_DISTANCE, MAX_DISTANCE };

static uint32_t sc_next_action_at = 0;
static uint32_t sc_last_target_seen_at = 0;
static bool sc_have_previous_scan = false;
static bool sc_have_target = false;
static int8_t sc_target_index = 2;
static uint16_t sc_target_distance = MAX_DISTANCE;
static LedColor sc_last_led_color = LedColor::Dark;

static inline bool sc_time_reached(uint32_t now, uint32_t deadline) {
  return (int32_t)(now - deadline) >= 0;
}

static inline bool sc_valid_distance(uint16_t distance, uint16_t maxDistance) {
  return distance >= SC_MIN_VALID_CM && distance <= maxDistance;
}

static uint16_t sc_read_distance_cm() {
  float value = sonar.getDistance();
  if (value <= 0 || value >= MAX_DISTANCE) {
    return MAX_DISTANCE;
  }
  return (uint16_t)(value + 0.5f);
}

static uint16_t sc_median3(uint16_t a, uint16_t b, uint16_t c) {
  if (a > b) { uint16_t t = a; a = b; b = t; }
  if (b > c) { uint16_t t = b; b = c; c = t; }
  if (a > b) { uint16_t t = a; a = b; b = t; }
  return b;
}

static int sc_abs_int(int value) {
  return value < 0 ? -value : value;
}

static int sc_clamp_int(int value, int minValue, int maxValue) {
  if (value < minValue) return minValue;
  if (value > maxValue) return maxValue;
  return value;
}

static void sc_set_led(LedColor color) {
  if (color == sc_last_led_color) {
    return;
  }
  sc_last_led_color = color;
  driver.setAllLedsColor(color);
}

static uint8_t sc_order_to_angle_index(uint8_t orderIndex) {
  return sc_scan_left_to_right
      ? orderIndex
      : (uint8_t)(SC_ANGLE_COUNT - 1 - orderIndex);
}

static void sc_reset_scan() {
  sc_scan_order_index = 0;
  sc_current_angle_index = sc_order_to_angle_index(0);
  sc_sample_index = 0;
  sc_scan_phase = SC_ScanPhase::SetAngle;
  sc_next_action_at = millis();
}

static int sc_distance_change(uint8_t index) {
  if (!sc_have_previous_scan
      || !sc_valid_distance(sc_current_distances[index], SC_TRACK_MAX_CM)
      || !sc_valid_distance(sc_previous_distances[index], SC_TRACK_MAX_CM)) {
    return 0;
  }

  return sc_abs_int((int)sc_current_distances[index] - (int)sc_previous_distances[index]);
}

static int8_t sc_choose_target_index() {
  int8_t bestIndex = -1;
  long bestScore = -32768L;
  int strongestMovement = 0;

  for (uint8_t i = 0; i < SC_ANGLE_COUNT; ++i) {
    const uint16_t distance = sc_current_distances[i];
    const uint16_t maxDistance = sc_have_target ? SC_TRACK_MAX_CM : SC_ACQUIRE_MAX_CM;
    if (!sc_valid_distance(distance, maxDistance)) {
      continue;
    }

    const int movement = sc_distance_change(i);
    if (movement > strongestMovement) {
      strongestMovement = movement;
    }

    const int centerPenalty = sc_abs_int((int)i - 2) * 4;
    const int continuityDistance = sc_abs_int((int)i - (int)sc_target_index);
    const int continuityBonus = sc_have_target
        ? sc_clamp_int(48 - continuityDistance * 22, 0, 48)
        : 0;

    // Movement identifies the moving object. Continuity prevents target hopping.
    // A small distance penalty prefers the nearer object when scores are similar.
    const long score = (long)movement * 8L
                     + (long)continuityBonus
                     - (long)centerPenalty
                     - (long)distance / 6L;

    if (score > bestScore) {
      bestScore = score;
      bestIndex = (int8_t)i;
    }
  }

  if (bestIndex < 0) {
    return -1;
  }

  if (!sc_have_target && strongestMovement < SC_MOVEMENT_THRESHOLD_CM) {
    // No movement yet. Allow initial acquisition of a reasonably close object,
    // but do not lock onto a distant wall just because humans enjoy clutter.
    int8_t nearestIndex = -1;
    uint16_t nearestDistance = MAX_DISTANCE;
    for (uint8_t i = 0; i < SC_ANGLE_COUNT; ++i) {
      const uint16_t distance = sc_current_distances[i];
      if (sc_valid_distance(distance, SC_ACQUIRE_MAX_CM) && distance < nearestDistance) {
        nearestDistance = distance;
        nearestIndex = (int8_t)i;
      }
    }
    return nearestIndex;
  }

  return bestIndex;
}

static void sc_finish_scan(uint32_t now) {
  const int8_t candidateIndex = sc_choose_target_index();

  if (candidateIndex >= 0) {
    sc_target_index = candidateIndex;
    sc_target_distance = sc_current_distances[(uint8_t)candidateIndex];
    sc_have_target = true;
    sc_last_target_seen_at = now;
  } else if (sc_have_target && (uint32_t)(now - sc_last_target_seen_at) > SC_TARGET_LOST_MS) {
    sc_have_target = false;
    sc_target_distance = MAX_DISTANCE;
  }

  for (uint8_t i = 0; i < SC_ANGLE_COUNT; ++i) {
    sc_previous_distances[i] = sc_current_distances[i];
    sc_current_distances[i] = MAX_DISTANCE;
  }
  sc_have_previous_scan = true;

  // Reverse scan direction to avoid a long servo jump after every frame.
  sc_scan_left_to_right = !sc_scan_left_to_right;
  sc_reset_scan();
}

static void sc_update_scan(uint32_t now) {
  switch (sc_scan_phase) {
    case SC_ScanPhase::SetAngle:
      sc_current_angle_index = sc_order_to_angle_index(sc_scan_order_index);
      servo.setPosition(sc_scan_angles[sc_current_angle_index]);
      sc_next_action_at = now + SC_SERVO_SETTLE_MS;
      sc_scan_phase = SC_ScanPhase::WaitForServo;
      break;

    case SC_ScanPhase::WaitForServo:
      if (!sc_time_reached(now, sc_next_action_at)) {
        return;
      }
      sc_sample_index = 0;
      sc_next_action_at = now;
      sc_scan_phase = SC_ScanPhase::ReadSamples;
      break;

    case SC_ScanPhase::ReadSamples:
      if (!sc_time_reached(now, sc_next_action_at)) {
        return;
      }

      sc_samples[sc_sample_index++] = sc_read_distance_cm();
      sc_next_action_at = millis() + SC_SONAR_SAMPLE_INTERVAL_MS;

      if (sc_sample_index < SC_SAMPLES_PER_ANGLE) {
        return;
      }

      sc_current_distances[sc_current_angle_index] = sc_median3(
          sc_samples[0], sc_samples[1], sc_samples[2]);

      sc_scan_order_index++;
      if (sc_scan_order_index >= SC_ANGLE_COUNT) {
        sc_finish_scan(millis());
      } else {
        sc_scan_phase = SC_ScanPhase::SetAngle;
      }
      break;
  }
}

static void sc_apply_tracking_motion() {
  if (!sc_have_target || !sc_valid_distance(sc_target_distance, SC_TRACK_MAX_CM)) {
    driver.stop();
    sc_set_led(LedColor::Yellow);
    return;
  }

  const int targetAngle = sc_scan_angles[(uint8_t)sc_target_index];
  const int angleError = targetAngle - SERVO_START_POS;
  const int distance = (int)sc_target_distance;

  // Distance safety has priority over lateral alignment.
  if (distance <= SC_EMERGENCY_BACK_CM) {
    driver.backward(SC_BACK_MAX_SPEED);
    sc_set_led(LedColor::Red);
    return;
  }

  if (sc_abs_int(angleError) > SC_ALIGN_TOLERANCE_DEG) {
    const int turnSpeed = sc_clamp_int(
        SC_TURN_MIN_SPEED + sc_abs_int(angleError),
        SC_TURN_MIN_SPEED,
        SC_TURN_MAX_SPEED);

    if (angleError > 0) {
      driver.rotateLeft((byte)turnSpeed);
    } else {
      driver.rotateRight((byte)turnSpeed);
    }
    sc_set_led(LedColor::Yellow);
    return;
  }

  if (distance > SC_BAND_MAX_CM) {
    const int speed = sc_clamp_int(
        SC_FORWARD_MIN_SPEED + (distance - SC_BAND_MAX_CM),
        SC_FORWARD_MIN_SPEED,
        SC_FORWARD_MAX_SPEED);
    driver.forward((byte)speed);
    sc_set_led(LedColor::Green);
    return;
  }

  if (distance < SC_BAND_MIN_CM) {
    const int speed = sc_clamp_int(
        SC_BACK_MIN_SPEED + (SC_BAND_MIN_CM - distance) * 2,
        SC_BACK_MIN_SPEED,
        SC_BACK_MAX_SPEED);
    driver.backward((byte)speed);
    sc_set_led(LedColor::Red);
    return;
  }

  driver.stop();
  sc_set_led(LedColor::Blue);
}

void initAutomatic_Stay_Close_To_Obstacle() {
  buzzer.off();
  driver.stop();
  servo.setPosition(SERVO_START_POS);

  sc_scan_left_to_right = true;
  sc_have_previous_scan = false;
  sc_have_target = false;
  sc_target_index = 2;
  sc_target_distance = MAX_DISTANCE;
  sc_last_target_seen_at = millis();
  sc_last_led_color = LedColor::Dark;

  for (uint8_t i = 0; i < SC_ANGLE_COUNT; ++i) {
    sc_current_distances[i] = MAX_DISTANCE;
    sc_previous_distances[i] = MAX_DISTANCE;
  }

  sc_reset_scan();
  sc_set_led(LedColor::White);
}

void loopAutomatic_Stay_Close_To_Obstacle() {
  sc_update_scan(millis());
  sc_apply_tracking_motion();
}
