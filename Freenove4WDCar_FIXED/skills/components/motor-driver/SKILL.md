# Motor Driver Skill

Software stack: `RRFreenove4WDCarDriver` -> two `RRFreenove4WDCarMotor` objects.

Pins:
- left DIR D4, PWM D6;
- right DIR D3, PWM D5.

Driver command scale is 5..100. Motor layer maps this into PWM domain with MIN_SPEED 80, MAX_SPEED 250, START_SPEED 100.

Current local Motor implementation:
- applies voltage compensation;
- uses per-call ramp/slew limiting;
- starts from rest with a kick when target PWM is below START_SPEED;
- requires repeated high-level driver calls for ramp progression.

Exact physical motor-driver IC and electrical current limits = UNKNOWN / NEEDS VERIFICATION.
