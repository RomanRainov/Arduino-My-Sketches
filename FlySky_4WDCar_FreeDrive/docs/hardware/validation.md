# Validation

- compile FQBN `arduino:avr:uno`;
- fail on `Serial.print*` while iBus owns Serial;
- compare pin constants to manifest;
- validate Servo/Timer1;
- reject Timer0 register changes;
- flag D13 if SPI is enabled;
- verify RR Driver API compatibility;
- check receiver-loss path for explicit safe motor state.

Validator not implemented.
