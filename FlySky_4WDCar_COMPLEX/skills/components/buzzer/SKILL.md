# Buzzer Skill

Pin A0, shared with battery ADC.

Current RR public API: `on()`, `off()`, `beep()` only. There is no `toneOn()/toneOff()` in current RR repository, although this sketch calls `buzzer.toneOn()` in Stay Close mode. Treat this as confirmed cross-repository API drift.
