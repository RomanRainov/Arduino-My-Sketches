# Servo Skill

Known signal pin D2. Arduino Servo on ATmega328P uses Timer1.

RR setup executes `setPosition(_startPosition + _offset)`. Main sketch already sets `SERVO_START_POS = 90 + SERVO_OFFSET`, so startup applies offset twice. Physical correct center: NEEDS VERIFICATION before changing behavior.
