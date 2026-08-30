# FlySky_4WDCar_COMPLEX — AI agent rules

Перед hardware-dependent изменением:
1. читать `docs/hardware/hardware-manifest.yaml`;
2. читать `skills/board/SKILL.md`;
3. читать Skill затрагиваемого компонента;
4. сверять RR library API с `RomanRainov/RR_Arduino_Libraries`;
5. сохранять существующую архитектуру режима, если задача не требует её изменения.

Критические правила:
- target: Arduino Uno / ATmega328P;
- hardware `Serial` принадлежит FlySky iBus;
- D13 сейчас используется как `LED_BUILTIN`, поэтому SPI SCK конфликтует;
- A0 = battery + buzzer intentional multiplex;
- Servo использует Timer1;
- obstacle modes используют blocking delays/pulseIn в текущем `main`;
- не считать документацию старых patch-файлов текущим кодом.

Подтверждённый repository drift:
- sketch вызывает `driver.setAllLedsColor()`, которого нет в текущем RR Driver header;
- sketch вызывает `buzzer.toneOn()`, которого нет в текущем RR Buzzer header;
- main не вызывает `sonar.setup()`, хотя Sonar constructor не задаёт pinMode;
- текущий main содержит синтаксически подозрительный `enum CarMode`/`currentMode` фрагмент; сборку нужно проверять реально.
