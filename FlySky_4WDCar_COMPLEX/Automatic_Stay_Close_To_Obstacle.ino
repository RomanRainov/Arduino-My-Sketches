/**********************************************************************
  Filename    : Automatic_Stay_Close_To_Obstacle.ino
  Mode        : Follow a moving object and keep it in 30–50 cm band
  Platform    : Freenove 4WD Car for UNO
  Notes       : - Non-blocking (no long delay())
                - Servo scanning (left <-> right) with limited angular speed
                - Median(3) + EMA filter for sonar distance
                - P-control around target + velocity feed-forward
                - Smooth motor commands via slew limiter
                - Turn-in-place when target is off-center
**********************************************************************/

// ========================= Tunables =========================
// Desired distance band (cm)
#define SC_BAND_MIN_CM        30     // lower bound of desired range
#define SC_BAND_MAX_CM        50     // upper bound of desired range
#define SC_TARGET_CM          40     // band center used by P-control
#define SC_DEADBAND_CM        2      // do nothing if |error| < deadband

// Safety distances (cm)
#define SC_TOO_CLOSE_CM       15     // immediate back if closer than this
#define SC_TOO_FAR_CM         200    // treat as "no target" / far scene

// Speed limits (driver scale 0..255 or your library scale)
#define SC_MAX_FWD_SPEED      150    // forward cap
#define SC_MAX_BACK_SPEED     150    // backward cap
#define SC_TURN_MIN_SPEED     90     // min speed for rotate*
#define SC_TURN_MAX_SPEED     160    // max speed for rotate*

// Controller gains
#define SC_KP_DIST            3      // proportional gain on distance error
#define SC_KV_FF              20     // velocity feed-forward: cm/s -> speed units

// Timing (ms)
#define SC_CTRL_DT_MS         30     // control period
#define SC_SONAR_DT_MS        50     // sonar sampling period
#define SC_SERVO_DT_MS        20     // servo stepping period
#define SC_SAMPLE_SETTLE_MS   10     // wait after servo motion before trusting sample

// Servo scan window (degrees)
#define SC_SERVO_CENTER_ANG   (SERVO_START_POS)
#define SC_SERVO_LEFT_ANG     (SERVO_START_POS + 45)
#define SC_SERVO_RIGHT_ANG    (SERVO_START_POS - 45)
#define SC_SERVO_DPS          180    // max deg/sec for smooth scanning

// Target acquisition / keeping
#define SC_ALIGN_TOL_DEG      12     // if |angle error| > this -> rotate in place to align
#define SC_MAX_VALID_CM       200    // discard readings beyond as "no target"
#define SC_KEEP_TARGET_BIAS   8      // deg cone around previous target considered "same target"

// ========================= Local state =========================
static uint32_t sc_t_ctrl = 0, sc_t_sonar = 0, sc_t_servo = 0, sc_t_sample_gate = 0;

static int16_t  sc_dist_med  = 999;      // median of 3 raw sonar reads
static int16_t  sc_dist_filt = 999;      // EMA filtered distance
static int16_t  sc_last_track_dist = 999;
static uint32_t sc_last_track_ts = 0;    // for velocity estimation

// Servo stepper state
static int      sc_cur_ang = SC_SERVO_CENTER_ANG;
static int      sc_scan_target_ang = SC_SERVO_LEFT_ANG; // ping-pong target
static bool     sc_scan_dir_right = false;              // scan direction flag

// Chosen tracking target (angle where the object is expected)
static int      sc_track_ang = SC_SERVO_CENTER_ANG;
static bool     sc_have_target = false;

// ========================= Helpers (local only) =========================

// Quick median of 3 values (branch-only, no heap)
static inline int16_t sc_median3(int16_t a, int16_t b, int16_t c) {
  if (a > b) { int16_t t=a; a=b; b=t; }
  if (b > c) { int16_t t=b; b=c; c=t; }
  if (a > b) { int16_t t=a; a=b; b=t; }
  return b;
}

// Integer EMA: alpha = num/den (e.g., 3/10 ~ 0.3)
static inline int16_t sc_ema_i16(int16_t prev, int16_t sample, uint8_t num, uint8_t den) {
  return (int16_t)(((int32_t)prev * (den - num) + (int32_t)sample * num) / den);
}

// Clamp int
static inline int sc_clamp(int v, int lo, int hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }

// Non-blocking periodic trigger
static inline bool sc_every(uint32_t &t, uint16_t dt) {
  uint32_t now = millis();
  if (now - t >= dt) { t = now; return true; }
  return false;
}

// Servo stepper: limit angular speed, no blocking delay
struct SC_ServoStepper {
  int cur = SC_SERVO_CENTER_ANG;
  int target = SC_SERVO_CENTER_ANG;
  uint16_t dps = SC_SERVO_DPS;
  uint32_t last = 0;

  void init(int startDeg, uint16_t degPerSec) {
    cur = target = startDeg; dps = degPerSec; last = millis();
    servo.setPosition(cur);
  }
  // returns new angle
  int update() {
    uint32_t now = millis(), dt = now - last; last = now;
    long maxStep = (long)dps * dt / 1000;
    if (maxStep < 1) return cur;
    int diff = target - cur;
    if (diff > 0) diff = (diff >  maxStep) ?  maxStep : diff;
    else          diff = (diff < -maxStep) ? -maxStep : diff;
    cur += diff;
    servo.setPosition(cur);
    return cur;
  }
} sc_servo;

// Time-based slew limiter for motor speed
struct SC_SlewLimiter {
  int cur = 0;                 // current command
  uint16_t upPerSec = 250;     // accel rate (units/s)
  uint16_t downPerSec = 700;   // brake rate (units/s)
  uint32_t last = 0;
  void init(int start, uint16_t up_ps, uint16_t down_ps) {
    cur = start; upPerSec = up_ps; downPerSec = down_ps; last = millis();
  }
  int step(int target) {
    uint32_t now = millis(), dt = now - last; last = now;
    long maxUp = (long)upPerSec * dt / 1000;
    long maxDn = (long)downPerSec * dt / 1000;
    int d = target - cur;
    if (d > 0) { if (d >  maxUp) d =  maxUp; }
    else       { if (-d > maxDn) d = -maxDn; }
    cur += d; return cur;
  }
} sc_slew;

// Read sonar once and normalize invalid values as 999 (far)
static inline int16_t sc_read_sonar_cm() {
  int d = sonar.getDistance();
  if (d <= 0) d = 999;
  return (int16_t)d;
}

// ========================= Lifecycle =========================
void initAutomatic_Stay_Close_To_Obstacle() {
  sc_t_ctrl = sc_t_sonar = sc_t_servo = sc_t_sample_gate = millis();

  sc_dist_med = sc_dist_filt = 999;
  sc_last_track_dist = 999; sc_last_track_ts = millis();

  sc_servo.init(SC_SERVO_CENTER_ANG, SC_SERVO_DPS);
  sc_cur_ang = SC_SERVO_CENTER_ANG;
  sc_scan_dir_right = false;
  sc_scan_target_ang = SC_SERVO_LEFT_ANG;

  sc_track_ang = SC_SERVO_CENTER_ANG;
  sc_have_target = false;

  sc_slew.init(0, 250, 800);
  driver.stop();
}

// ========================= Main loop =========================
void loopAutomatic_Stay_Close_To_Obstacle() {
  const uint32_t now = millis();

  // --- (1) Servo stepping: scan or align to known target
  if (sc_every(sc_t_servo, SC_SERVO_DT_MS)) {
    if (!sc_have_target) {
      // Ping-pong scan between edges
      sc_servo.target = sc_scan_target_ang;
      sc_cur_ang = sc_servo.update();
      if (abs(sc_cur_ang - sc_scan_target_ang) <= 1) {
        if (sc_scan_target_ang == SC_SERVO_LEFT_ANG) {
          sc_scan_target_ang = SC_SERVO_RIGHT_ANG; sc_scan_dir_right = true;
        } else {
          sc_scan_target_ang = SC_SERVO_LEFT_ANG;  sc_scan_dir_right = false;
        }
      }
    } else {
      // Align servo to tracked angle
      sc_servo.target = sc_track_ang;
      sc_cur_ang = sc_servo.update();
    }
    // let sonar settle after servo motion
    sc_t_sample_gate = now;
  }

  // --- (2) Sonar sampling: median(3) + EMA (after small settle)
  if (sc_every(sc_t_sonar, SC_SONAR_DT_MS)) {
    if (now - sc_t_sample_gate >= SC_SAMPLE_SETTLE_MS) {
      int16_t d1 = sc_read_sonar_cm();
      int16_t d2 = sc_read_sonar_cm();
      int16_t d3 = sc_read_sonar_cm();
      sc_dist_med  = sc_median3(d1, d2, d3);
      sc_dist_filt = sc_ema_i16(sc_dist_filt, sc_dist_med, 3, 10); // alpha ~0.3
    }
  }

  // --- (3) Target acquisition/keeping
  bool valid_here = (sc_dist_filt > 0 && sc_dist_filt < SC_MAX_VALID_CM);

  if (valid_here) {
    if (!sc_have_target) {
      // First detection -> acquire target at current angle
      sc_have_target = true;
      sc_track_ang   = sc_cur_ang;
      sc_last_track_dist = sc_dist_filt;
      sc_last_track_ts   = now;
    } else {
      // Keep target near previous angle to avoid jumping
      int ang_err = abs(sc_cur_ang - sc_track_ang);
      if (ang_err <= SC_KEEP_TARGET_BIAS) {
        sc_track_ang = sc_cur_ang;
      }
      // If we see a significantly closer value at another angle -> retarget
      if (sc_dist_filt + 5 < sc_last_track_dist) {
        sc_track_ang = sc_cur_ang;
      }
    }
  } else {
    // If we lose target for long -> drop and resume scanning
    if (sc_have_target && (now - sc_last_track_ts > 600)) {
      sc_have_target = false;
    }
  }

  // --- (4) Control at fixed rate
  if (!sc_every(sc_t_ctrl, SC_CTRL_DT_MS)) return;

  int targetSpeed = 0;     // +forward, -backward
  bool rotateInPlace = false;
  int  rotateSpeed = 0;

  int16_t d = sc_dist_filt;

  if (!sc_have_target) {
    // No target: slow forward search while scanning
    targetSpeed = SC_MAX_FWD_SPEED / 2;
  } else {
    // Angle alignment first
    int ang_err = sc_track_ang - SC_SERVO_CENTER_ANG; // + => target to the left
    if (abs(ang_err) > SC_ALIGN_TOL_DEG) {
      rotateInPlace = true;
      rotateSpeed = map(abs(ang_err), SC_ALIGN_TOL_DEG, 45, SC_TURN_MIN_SPEED, SC_TURN_MAX_SPEED);
      rotateSpeed = sc_clamp(rotateSpeed, SC_TURN_MIN_SPEED, SC_TURN_MAX_SPEED);
    }

    // Distance control with velocity feed-forward
    float v_away_cms = 0.0f; // positive => target moving away
    if (sc_last_track_ts != 0) {
      uint32_t dt_ms = now - sc_last_track_ts;
      if (dt_ms >= 20) {
        v_away_cms = (float)((int)d - (int)sc_last_track_dist) * 1000.0f / (float)dt_ms;
      }
    }
    sc_last_track_dist = d;
    sc_last_track_ts   = now;

    // Safety first
    if (d <= SC_TOO_CLOSE_CM) {
      targetSpeed = -SC_MAX_BACK_SPEED;    // emergency back
      rotateInPlace = false;               // don't rotate while backing off
    }
    else if (d >= SC_TOO_FAR_CM) {
      targetSpeed = SC_MAX_FWD_SPEED / 2;  // lost precise target distance
    }
    else {
      // P-control around target band center
      const int err = (int)SC_TARGET_CM - (int)d; // + => too far -> forward
      if (err > -SC_DEADBAND_CM && err < SC_DEADBAND_CM) {
        targetSpeed = 0;
      } else {
        int u = SC_KP_DIST * (err > 0 ? err : -err);
        if (u > SC_MAX_FWD_SPEED) u = SC_MAX_FWD_SPEED;
        targetSpeed = (err > 0) ? u : -u;
      }

      // Feed-forward if target is moving away
      if (v_away_cms > 0.0f) {
        int ff = (int)(SC_KV_FF * v_away_cms);
        if (ff > 0) {
          targetSpeed += ff;
          if (targetSpeed > SC_MAX_FWD_SPEED) targetSpeed = SC_MAX_FWD_SPEED;
        }
      }
    }
  }

  // --- (5) Apply commands smoothly (no blocking)
  if (rotateInPlace) {
    int spd = sc_slew.step(rotateSpeed);
    if (sc_track_ang > SC_SERVO_CENTER_ANG) driver.rotateLeft(spd);
    else                                    driver.rotateRight(spd);
  } else {
    int spd = sc_slew.step(targetSpeed);
    if      (spd > 0) driver.forward(spd);
    else if (spd < 0) driver.backward(-spd);
    else              driver.stop();
  }
}
