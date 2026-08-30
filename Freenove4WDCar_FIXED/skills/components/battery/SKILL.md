# Battery Skill

Pin A0 is used for battery measurement and is shared with the buzzer/tone path.

Local implementation formula: `batteryADC / 1023.0 * 5.0 * 4`.

The local `RR_Freenove4WDCar_Battery.h` keeps Serial debug disabled because hardware Serial belongs to FlySky iBus. The upstream `RomanRainov/RR_Arduino_Libraries` copy currently enables that debug macro, so local and upstream behavior differ.

Electrical divider ratio, ADC-reference tolerance and safe current/voltage limits beyond the software formula are NEEDS VERIFICATION.
