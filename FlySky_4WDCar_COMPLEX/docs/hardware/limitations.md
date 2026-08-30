# Current confirmed issues

1. `Automatic_Obstacle_Avoidance.ino` is blocking: three servo angles × five sonar reads + delays.
2. Main sketch does not call `sonar.setup()`, while current RR Sonar constructor does not configure pin directions.
3. RR Sonar uses `pulseIn()` without timeout.
4. Obstacle mode directly controls motor GPIO/PWM via `motorRun()`, bypassing RR Driver.
5. Obstacle mode computes its own battery compensation from `8.2 - getBatteryVoltage()`.
6. `driver.setAllLedsColor()` is called, but current RR Driver public API does not contain this method.
7. `buzzer.toneOn()` is called, but current RR Buzzer public API does not contain this method.
8. RR Battery debug prints to Serial if that class is used; iBus owns Serial.
9. `RRFreenove4WDCarServo::setup()` double-applies configured offset relative to `SERVO_START_POS`.
10. Receiver-loss path `if (receiver.get(0) == 0) return;` does not explicitly call `driver.stop()`.
11. Current `FlySky_4WDCar_COMPLEX.ino` text has a suspicious enum/currentMode declaration and must be compile-validated.
