# Battery Skill

Pin A0, shared with buzzer. Formula: `batteryADC / 1023.0 * 5.0 * 4`.

Current RR Battery header enables Serial debug, conflicting with FlySky iBus if the class reads voltage. Obstacle Avoidance also has its own local battery-reading function.

Electrical limits beyond this software formula: NEEDS VERIFICATION.
