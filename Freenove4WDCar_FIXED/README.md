# Freenove4WDCar_FIXED

Рабочая версия проекта **Freenove 4WD Car for UNO** с управлением от FlySky iBus, ручным режимом и автоматическими режимами движения.

Проект ориентирован на **Arduino Uno / ATmega328P** и использует локальные версии библиотек из каталога `Libraries/`.

## Основной скетч

`FlySky_4WDCar_COMPLEX/`

Режимы работы:

- **FreeDrive** — ручное управление с FlySky.
- **Automatic_Obstacle_Avoidance** — остановка перед препятствием, сканирование сонаром и выбор направления обхода.
- **Automatic_Stay_Close_To_Obstacle** — поиск/сопровождение объекта с удержанием дистанции.

Переключение режимов выполняется каналом **SWC** приёмника FlySky.

## Структура

```text
Freenove4WDCar_FIXED/
├── FlySky_4WDCar_COMPLEX/   # основной Arduino sketch
├── Libraries/               # локальные версии библиотек проекта
├── docs/
│   ├── hardware/            # hardware manifest, pinout, interfaces, power, limits
│   └── architecture/        # architecture, modules, data flow
├── skills/
│   ├── board/               # знания о Arduino Uno / ATmega328P
│   └── components/          # component-specific AI skills
├── AGENTS.md                # правила для AI-агентов
├── README_FIXED_RU.md       # журнал/описание исправлений
└── README.md
```

## Аппаратная конфигурация

Главный источник аппаратной конфигурации:

[`docs/hardware/hardware-manifest.yaml`](docs/hardware/hardware-manifest.yaml)

Дополнительно:

- [Pinout](docs/hardware/pinout.md)
- [Interfaces](docs/hardware/interfaces.md)
- [Power](docs/hardware/power.md)
- [Limitations](docs/hardware/limitations.md)
- [Validation rules](docs/hardware/validation.md)

Критически важные ресурсы:

- D0/D1 — FlySky iBus через hardware `Serial`;
- D2 — servo;
- D3/D4 — направления моторов;
- D5/D6 — PWM моторов;
- D7/D8 — HC-SR04;
- D13 — activity LED и одновременно SPI SCK;
- A0 — battery ADC и buzzer;
- A4/A5 — I2C RGB controller.

Перед изменением GPIO сверяйтесь с `hardware-manifest.yaml`.

## Библиотеки

Для этой версии проекта **локальный каталог `Libraries/` является источником истины**.

Он может отличаться от отдельного репозитория:

`RomanRainov/RR_Arduino_Libraries`

Не заменяйте локальные библиотеки версиями из другого репозитория без сравнения изменений.

## Сборка

Target:

```text
arduino:avr:uno
```

MCU:

```text
ATmega328P
```

Для сборки должны быть доступны библиотеки проекта из `Libraries/` и используемая библиотека FlySky iBus.

## Важные ограничения

- Не использовать `Serial.print*` для отладки: hardware `Serial` занят FlySky iBus.
- Servo занимает Timer1.
- `tone()`/buzzer использует Timer2.
- Не менять режим/делитель Timer0: он связан с D5/D6 и системными `millis()/delay()`.
- A0 используется одновременно для battery measurement и buzzer. Это нужно учитывать при изменениях.
- При hardware-dependent изменениях сначала читать [`AGENTS.md`](AGENTS.md) и соответствующий `SKILL.md`.

## AI-ready документация

Документация проекта организована как:

```text
Board Skill
    ↓
Component Skills
    ↓
Hardware Manifest
    ↓
Project Knowledge
    ↓
Code
```

AI-агент не должен угадывать распиновку, характеристики MCU или электрические параметры по аналогии с другими Arduino-проектами.

Подробные правила: [`AGENTS.md`](AGENTS.md).

## Исправления

Описание исправлений относительно предыдущей версии:

[`README_FIXED_RU.md`](README_FIXED_RU.md)
