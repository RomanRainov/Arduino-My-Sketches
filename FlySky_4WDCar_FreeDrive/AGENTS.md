# FlySky_4WDCar_FreeDrive — AI agent rules

Target: Arduino Uno / ATmega328P.

Перед changes read local manifest, pinout, interfaces and component Skills. RR library truth lives in `RomanRainov/RR_Arduino_Libraries`.

Critical current issue: this sketch calls active `Serial.print/println` in `setup()` after `receiver.begin()`, while the same hardware `Serial` is owned by `iBus receiver(Serial, ...)`. Do not add more Serial debug; existing prints are a confirmed conflict.

Other rules:
- D13 activity LED conflicts with future SPI SCK;
- Servo uses Timer1;
- D5/D6 are motor PWM;
- A0 is passed as battery pin;
- preserve direct manual-control architecture unless task requests refactor.
