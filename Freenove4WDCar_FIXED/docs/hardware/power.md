# Power — Freenove4WDCar_FIXED

Код измерения батареи использует формулу:

`batteryADC / 1023.0 * 5.0 * 4`

Это означает программное предположение о 5 V ADC reference и внешнем делителе примерно 1:4. Это не является подтверждением реальной схемы без документации платы.

`MAX_VOLTAGE = 7.4` в Motor library используется для программной компенсации PWM и не является electrical absolute maximum.

`UNKNOWN / NEEDS VERIFICATION`:
- точная модель и схема motor-driver;
- topology и ток регуляторов питания;
- motor stall current;
- servo peak current;
- точные аккумуляторные элементы и protection/BMS;
- допустимость совместного использования A0 для battery ADC и buzzer на реальной Freenove board.

Не выводить допустимые токи/напряжения из PWM диапазонов или названий программных констант.
