# Interfaces

UART: D0/D1, iBus over `Serial`. Current setup also writes diagnostic text to the same Serial, which is a confirmed conflict.

PWM: D5/D6 motors, Timer0 outputs.

Servo: D2 signal; Arduino Servo uses Timer1.

I2C: Driver internally initializes RGB controller at 0x20 on A4/A5.

SPI: inactive, but D13 is driven as LED_BUILTIN.
