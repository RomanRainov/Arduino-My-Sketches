# Architecture overview

```text
FlySky receiver -> iBus / Serial -> main loop
                               |
                               +-> FreeDrive
                               +-> Automatic_Stay_Close_To_Obstacle
                               +-> Automatic_Obstacle_Avoidance

Automatic modes -> Servo + HC-SR04 -> target/obstacle state machines
All drive modes -> RRFreenove4WDCarDriver -> motor objects -> GPIO/PWM
Status -> buzzer + I2C RGB controller
```

`FlySky_4WDCar_COMPLEX.ino` is the composition root: creates hardware objects, receives FlySky channels, selects mode and dispatches the active mode loop.

Mode selection from SWC:
- `< 1250` -> `FreeDrive`
- `1250..1749` -> `Automatic_Obstacle_Avoidance`
- `>= 1750` -> `Automatic_Stay_Close_To_Obstacle`

Before mode switching the current code calls `driver.stop()` and `buzzer.off()`, then initializes the selected mode and runs blocking sound/LED indication.

Automatic Obstacle Avoidance is a state machine for forward cruise, obstacle confirmation, left/right scan, optional reverse, and rotate-until-clear behavior.

Stay Close is a continuous five-angle scan/tracking state machine that estimates a target from distance changes, keeps target continuity, rotates to align, and maintains roughly 30–50 cm distance.
