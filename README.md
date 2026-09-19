# Arduino-My-Sketches

Коллекция Arduino-скетчей и экспериментальных проектов.

Основная часть репозитория связана с роботизированной машиной **Freenove 4WD Car for UNO**, управлением через FlySky iBus и собственными библиотеками из репозитория [RR_Arduino_Libraries](https://github.com/RomanRainov/RR_Arduino_Libraries).

## Freenove4WDCar

### [Freenove4WDCar_FIXED](Freenove4WDCar_FIXED)

Основной документированный вариант проекта Freenove4WDCar.

Содержит:

- рабочий sketch `FlySky_4WDCar_COMPLEX`;
- локальные версии используемых библиотек;
- ручной режим `FreeDrive`;
- `Automatic_Obstacle_Avoidance`;
- `Automatic_Stay_Close_To_Obstacle`;
- AI-ready документацию;
- `hardware-manifest.yaml`;
- pinout, interfaces, power и validation docs;
- Board Skill и Component Skills;
- `AGENTS.md` с правилами для AI-агентов.

Подробности: [Freenove4WDCar_FIXED/README.md](Freenove4WDCar_FIXED/README.md).

> Для `Freenove4WDCar_FIXED` локальный каталог `Libraries/` является источником истины. Он может отличаться от standalone-репозитория `RR_Arduino_Libraries`.

### [FlySky_4WDCar_COMPLEX](FlySky_4WDCar_COMPLEX)

Предыдущий/отдельный вариант комплексного управления машиной.

Включает:

- FlySky iBus;
- ручное управление;
- автоматические режимы;
- AI-ready документацию в `docs/` и `skills/`.

При изменениях аппаратно-зависимого кода сначала читать локальный `AGENTS.md` и `docs/hardware/hardware-manifest.yaml`.

### [FlySky_4WDCar_FreeDrive](FlySky_4WDCar_FreeDrive)

Упрощённый вариант ручного управления Freenove4WDCar через FlySky.

Также содержит собственные `docs/`, `skills/` и `AGENTS.md`.

## Другие проекты

- [Radar](Radar) — Arduino-проект радара.
- [TestFlySky_2](TestFlySky_2) — эксперименты с FlySky.
- [Test_RGBLED](Test_RGBLED) — тесты RGB LED.
- [autopoliv](autopoliv) — проект автоматического полива.

## Платформа Freenove4WDCar

Для документированной конфигурации Freenove4WDCar:

```text
Board: Arduino Uno
MCU: ATmega328P
FQBN: arduino:avr:uno
```

Аппаратные параметры конкретного проекта нельзя переносить между скетчами по предположению. Для Freenove-проектов источником истины является локальный `hardware-manifest.yaml`.

## Связанный репозиторий библиотек

[RomanRainov/RR_Arduino_Libraries](https://github.com/RomanRainov/RR_Arduino_Libraries)

Там находятся отдельные библиотеки для:

- battery measurement;
- buzzer;
- motor;
- driver;
- servo;
- sonar;
- turn signals.

Версии библиотек в конкретном sketch могут отличаться от standalone-репозитория, поэтому перед заменой необходимо сравнивать код, а не надеяться, что одинаковое имя каталога означает одинаковое содержимое. Git, как обычно, не читает мысли.

## AI-ready документация

Freenove-проекты постепенно документируются по схеме:

```text
AGENTS.md
    ↓
Board Skill
    ↓
Component Skills
    ↓
Hardware Manifest
    ↓
Project Architecture
    ↓
Code
```

Цель — не позволять AI-агентам угадывать GPIO, интерфейсы, электрические параметры и API библиотек по общим знаниям об Arduino.
