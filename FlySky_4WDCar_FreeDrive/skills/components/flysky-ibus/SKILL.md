# FlySky iBus Skill

`iBus receiver(Serial, MAX_CHANNELS)` owns hardware UART D0/D1.

Rules:
- no `Serial.print/println` debug;
- no simultaneous Bluetooth on same UART without redesign;
- receiver failsafe behavior = NEEDS VERIFICATION;
- current early return on receiver zero does not explicitly stop motors.

Current standalone sketch violates the first rule in setup by printing calibration values to `Serial`.
