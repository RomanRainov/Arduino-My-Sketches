# FlySky iBus Skill

Current sketch uses `iBus receiver(Serial, MAX_CHANNELS)`.

Hardware UART pins D0/D1 are reserved for the receiver.

Channel mapping:
- CH1 Aileron
- CH2 Elevator
- CH3 Throttle
- CH4 Rudder
- CH5 SWC
- CH6 SWA

Rules:
- no `Serial.print*` debug while iBus is active;
- do not attach another UART consumer to `Serial` without redesign;
- current receiver-loss check returns from loop when channel 0 is zero, but does not explicitly stop the motors;
- exact receiver model and configured failsafe behavior = NEEDS VERIFICATION.
