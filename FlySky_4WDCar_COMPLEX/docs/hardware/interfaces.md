# Interfaces

- UART D0/D1: `iBus receiver(Serial, MAX_CHANNELS)`.
- PWM D5/D6: motors.
- Timer1: Servo library.
- Timer2: Arduino `tone()` если используется; текущий RR Buzzer API tone не содержит, но sketch вызывает отсутствующий `toneOn`.
- I2C A4/A5: RGB controller 0x20.
- ADC A0: battery, shared with buzzer.
- Sonar D7/D8: current RR Sonar uses blocking `pulseIn()` without timeout.

`LED_BUILTIN` writes D13 in each loop, so future SPI must remove/relocate this indicator.
