# Pinout — Freenove4WDCar_FIXED

| Pin | Role | Interface / resource | Notes |
|---|---|---|---|
| D0/D1 | FlySky iBus | hardware UART | `Serial` reserved; no debug output |
| D2 | Servo | Servo / Timer1 | PWM on D9/D10 unavailable while Servo is active |
| D3 | Right motor direction | GPIO | output |
| D4 | Left motor direction | GPIO | output |
| D5 | Right motor PWM | PWM / Timer0 | do not change Timer0 mode/prescaler |
| D6 | Left motor PWM | PWM / Timer0 | do not change Timer0 mode/prescaler |
| D7 | HC-SR04 TRIG | GPIO | configured by `sonar.setup()` |
| D8 | HC-SR04 ECHO | GPIO | bounded `pulseIn()` in local Sonar library |
| D13 | activity LED | GPIO + SPI SCK | current loop writes `LED_BUILTIN`; conflicts with SPI SCK |
| A0 | battery ADC + buzzer/tone | ADC + GPIO/Timer2 | confirmed shared resource; use requires coordination |
| A4/A5 | RGB controller | I2C | address `0x20`, 10 LEDs, GRB |

D9-D12 and A1-A3 must not be declared free automatically. Verify the Freenove extension-board schematic before assigning them.
