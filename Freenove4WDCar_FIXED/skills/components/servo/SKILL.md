# Servo Skill

Signal pin: D2.

Library: Arduino `Servo`, which occupies Timer1 on ATmega328P. Treat PWM D9/D10 as unavailable while Servo is active.

Current constructor receives:
- `servoPin = D2`
- `servoOffset = SERVO_OFFSET`
- `servoStartPos = SERVO_START_POS`

Current `setup()` then calls `setPosition(_startPosition + _offset)`. Because main defines `SERVO_START_POS = 90 + SERVO_OFFSET`, startup applies the configured offset twice.

Do not change this blindly: physical center and mounting orientation must be verified on the actual car before correcting behavior.
