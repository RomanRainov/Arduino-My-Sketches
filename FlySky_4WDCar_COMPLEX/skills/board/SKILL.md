# Board Skill — Arduino Uno / ATmega328P

Project target: `arduino:avr:uno`, MCU ATmega328P, 16 MHz, 5 V logic.

Критические ресурсы:
- D0/D1: hardware UART, FlySky iBus;
- D5/D6: motor PWM, Timer0 outputs;
- D2: servo signal; Servo library uses Timer1;
- A0: battery ADC + buzzer multiplex;
- A4/A5: I2C;
- D13: activity LED and SPI SCK.

Rules: не использовать ESP32 assumptions; не менять Timer0 mode/prescaler; при Servo считать Timer1 занятым; при `tone()` считать Timer2 занятым; не использовать `Serial` debug вместе с iBus; exact current limits/PCB revision = NEEDS VERIFICATION.
