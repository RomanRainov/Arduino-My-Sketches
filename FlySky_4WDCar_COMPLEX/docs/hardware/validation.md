# Validation

Required checks:
- compile with `arduino:avr:uno`;
- verify every method called on RR classes exists in current RR headers;
- reject active `Serial.print*` while iBus uses `Serial`;
- verify `sonar.setup()` before first distance read;
- flag `pulseIn()` without timeout;
- flag D13 use if SPI is enabled;
- validate A0 as allowed multiplex, not duplicate error;
- flag direct motor GPIO code outside driver as architecture bypass;
- validate Servo/Timer1 and tone/Timer2 conflicts;
- compare pin constants with manifest.

Validator not implemented here.
