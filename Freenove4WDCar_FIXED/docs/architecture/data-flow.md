# Data flow — Freenove4WDCar_FIXED

FlySky channels:
- CH1 Aileron -> steering / rotation in FreeDrive
- CH2 Elevator -> forward/back speed in FreeDrive
- CH3 Throttle -> buzzer frequency via `tone(A0, ...)`
- CH4 Rudder -> servo angle in FreeDrive
- CH5 SWC -> operating mode
- CH6 SWA -> read into state but currently not used by mode logic

Main loop:
1. process iBus;
2. abort current iteration if receiver data is not ready;
3. read channels;
4. map SWC to desired mode;
5. on change: stop motors and buzzer, init new mode, indicate it;
6. call active mode loop;
7. short pacing delay.

Automatic modes read HC-SR04 through the Servo-mounted sensor, transform measurements into state-machine decisions, and issue high-level driver commands. Driver then maps 0..100 commands to motor PWM domain and updates RGB LEDs.
