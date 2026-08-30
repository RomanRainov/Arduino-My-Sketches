# Buzzer Skill

Pin A0, shared with battery ADC.

Local public API:
- `on()` / `off()`
- `beep(unsigned int)`
- `toneOn(frequency)`
- `toneOn(frequency, duration)`
- `toneOff()`

`tone*` uses Arduino `tone()`/`noTone()`, therefore Timer2 is a project resource while tone is active.

Do not enable another Timer2 owner such as the current `RR_Freenove4WDCar_TurnSignal` without redesign.
