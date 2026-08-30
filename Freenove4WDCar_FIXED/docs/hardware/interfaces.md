# Interfaces — Freenove4WDCar_FIXED

- UART D0/D1: `iBus receiver(Serial, MAX_CHANNELS)`. Hardware `Serial` полностью занят приёмником FlySky.
- PWM D5/D6: управление моторами; используются выходы Timer0, поэтому менять режим/предделитель Timer0 нельзя.
- Timer1: Arduino Servo library через D2. PWM D9/D10 считать занятым ресурсом.
- Timer2: используется Arduino `tone()` при активном buzzer/tone. Библиотека `RR_Freenove4WDCar_TurnSignal` также требует Timer2 и в текущем виде конфликтует.
- I2C A4/A5: Freenove RGB controller, address `0x20`, 10 LEDs, GRB.
- ADC A0: измерение батареи. Тот же A0 используется buzzer/tone, поэтому переключение режима пина должно контролироваться кодом.
- Sonar D7/D8: HC-SR04. Локальная библиотека задаёт pinMode через `sonar.setup()` и использует bounded `pulseIn()` timeout.
- D13: `LED_BUILTIN` activity indication; одновременно SPI SCK, поэтому SPI требует переноса/отключения индикатора.
