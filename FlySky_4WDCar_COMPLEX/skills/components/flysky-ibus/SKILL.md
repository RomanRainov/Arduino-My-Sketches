# FlySky iBus Skill

Current sketch: `iBus receiver(Serial, MAX_CHANNELS)`. Hardware UART D0/D1 is reserved.

Channel mapping: 1 Aileron, 2 Elevator, 3 Throttle, 4 Rudder, 5 SWC, 6 SWA.

Rules: no Serial debug; no simultaneous Bluetooth on same UART without redesign; receiver failsafe behavior = NEEDS VERIFICATION; `return` on zero channel does not itself stop motors.
