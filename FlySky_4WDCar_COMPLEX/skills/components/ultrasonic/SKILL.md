# Ultrasonic Skill

Model HC-SR04. Pins D7 TRIG, D8 ECHO.

Current RR `getDistance()` uses `pulseIn(_echoPin, HIGH)` without timeout. Main sketch currently does not call `sonar.setup()`, while RR constructor does not configure pin direction.

Automatic modes use repeated blocking sonar reads.
