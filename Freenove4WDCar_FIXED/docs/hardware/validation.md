# Validation — Freenove4WDCar_FIXED

Required checks:
- compile target `arduino:avr:uno`;
- reject active `Serial.print*` while FlySky iBus owns hardware Serial;
- verify `sonar.setup()` executes before first distance read;
- require bounded `pulseIn()` timeout in local Sonar implementation;
- flag D13 use if SPI is introduced;
- flag A0 battery+buzzer as shared-resource hazard, not a harmless duplicate;
- validate Servo/Timer1 conflict with PWM D9/D10;
- validate `tone()`/Timer2 conflict with `RR_Freenove4WDCar_TurnSignal`;
- compare pin constants with `hardware-manifest.yaml`;
- verify every library call against the local `Libraries/` headers first;
- detect use of upstream RR library behavior when local copies differ;
- flag receiver-loss paths that can leave previous motor command active;
- flag unbounded waits such as `while (!_strip.begin());`.

Validator is not implemented in this documentation task.
