# Motor Driver Skill

Software: `RRFreenove4WDCarDriver` + `RRFreenove4WDCarMotor`. Exact physical driver IC: UNKNOWN.

Known pins: left DIR D4/PWM D6; right DIR D3/PWM D5.
Driver input scale: 5..100. Motor constants: MIN 80, MAX 250, START 100.

Current RR implementation has no ramp/slew; voltage compensation code is unreachable due early return. Obstacle Avoidance also bypasses Driver via local `motorRun()`.
