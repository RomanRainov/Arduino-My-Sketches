# Board Skill — Arduino Uno / ATmega328P

Project target: `arduino:avr:uno`, MCU ATmega328P, 16 MHz, 5 V logic.

Critical resources:
- D0/D1: hardware UART, reserved for FlySky iBus;
- D5/D6: motor PWM, Timer0 outputs;
- D2: servo signal; Servo library occupies Timer1;
- A0: battery ADC + buzzer/tone shared resource;
- A4/A5: I2C RGB controller;
- D13: activity LED and SPI SCK.

Rules:
- do not use ESP32 assumptions;
- do not change Timer0 mode/prescaler;
- with Servo active, treat Timer1 and PWM D9/D10 as unavailable;
- with `tone()` active, treat Timer2 as occupied;
- no `Serial` debug while iBus is active;
- verify Freenove extension-board routing before using apparently free pins;
- exact PCB revision/current limits = NEEDS VERIFICATION.
