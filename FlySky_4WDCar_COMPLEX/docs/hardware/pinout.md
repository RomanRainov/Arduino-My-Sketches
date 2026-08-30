# Pinout

| Pin | Role | Notes |
|---|---|---|
| D0/D1 | FlySky iBus `Serial` | debug Serial запрещён |
| D2 | Servo | Timer1 via Servo |
| D3 | Right motor direction | GPIO |
| D4 | Left motor direction | GPIO |
| D5 | Right motor PWM | Timer0 output |
| D6 | Left motor PWM | Timer0 output |
| D7 | HC-SR04 TRIG | main currently does not call `sonar.setup()` |
| D8 | HC-SR04 ECHO | same initialization issue |
| D13 | activity LED | also SPI SCK |
| A0 | battery + buzzer | intentional multiplex |
| A4/A5 | I2C RGB controller | address 0x20 |

D9-D12 и A1-A3 нельзя автоматически объявлять свободными: проверить Freenove extension board.
