# Battery Skill

Pin A0 is passed to RR Driver/Motor. RR Battery formula is `batteryADC / 1023.0 * 5.0 * 4`.

Current RR Battery header enables Serial debug, which would conflict with FlySky iBus if battery measurement is executed.

Electrical limits: NEEDS VERIFICATION.
