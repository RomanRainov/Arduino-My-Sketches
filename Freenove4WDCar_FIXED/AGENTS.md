# Freenove4WDCar_FIXED — AI agent rules

Этот каталог является зафиксированной рабочей версией проекта Freenove 4WD Car для Arduino Uno.

Перед hardware-dependent изменением:
1. читать `docs/hardware/hardware-manifest.yaml`;
2. читать `skills/board/SKILL.md`;
3. читать Skill затрагиваемого компонента;
4. проверять фактический код в `FlySky_4WDCar_COMPLEX/` и локальные копии библиотек в `Libraries/`;
5. не переносить выводы из старых `FlySky_4WDCar_COMPLEX`/`FlySky_4WDCar_FreeDrive` без повторной проверки.

## Источник истины

Для этого каталога локальные `Libraries/` имеют приоритет над `RomanRainov/RR_Arduino_Libraries`: сейчас между ними есть подтверждённый drift. В частности, локальный Sonar имеет timeout в `pulseIn()`, а upstream-версия пока нет; локальный Battery держит Serial debug выключенным, а upstream-версия включает его.

## Критические правила

- target: Arduino Uno / ATmega328P, `arduino:avr:uno`;
- hardware `Serial` D0/D1 принадлежит FlySky iBus: не добавлять `Serial.print*`;
- D2 Servo занимает Timer1; PWM D9/D10 считать недоступным при Servo;
- `tone()`/Buzzer использует Timer2; не включать `RR_Freenove4WDCar_TurnSignal` с Timer2 без переработки;
- D13 используется как activity LED и одновременно является SPI SCK;
- A0 одновременно указан как battery ADC и buzzer/tone: это реальный конфликт ресурсов, а не просто duplicate pin;
- A4/A5 заняты I2C RGB controller 0x20;
- не менять Timer0 mode/prescaler: D5/D6 — motor PWM, а Timer0 также обслуживает `millis()/delay()`.

## Известные ограничения текущего кода

- при `receiver.get(0) == 0` main loop делает `return` без `driver.stop()`; failsafe должен рассматриваться отдельно перед эксплуатацией;
- `RRFreenove4WDCarServo::setup()` устанавливает `_startPosition + _offset`, хотя main уже передаёт `SERVO_START_POS = 90 + SERVO_OFFSET`; startup offset применяется дважды;
- `RRFreenove4WDCarDriver::setup()` бесконечно ждёт `_strip.begin()`;
- комментарий про SWC hysteresis неточен: текущая функция использует два статических порога без stateful hysteresis;
- режимы реализованы state-machine стилем, но каждый sonar read всё равно может блокировать до bounded `pulseIn()` timeout;
- индикация смены режима блокирующая (`beep`/`delay`), но перед ней моторы явно останавливаются.

Если факт не подтверждён исходниками/официальной документацией, использовать `UNKNOWN`, `NEEDS VERIFICATION` или `LIKELY`, а не придумывать наиболее удобную версию аппаратуры.
