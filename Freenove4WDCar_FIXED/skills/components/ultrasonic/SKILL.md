# Ultrasonic Skill

Model: HC-SR04.

Pins:
- D7 TRIG
- D8 ECHO

Local `RRFreenove4WDCarSonar` requires `sonar.setup()` to configure pin directions. The current fixed main sketch calls it during setup.

Local `getDistance()` uses bounded `pulseIn(_echoPin, HIGH, timeoutUs)` where timeout is derived from configured max distance, then returns `_maxDistance` on timeout.

Automatic modes still perform repeated sonar reads, so each read is blocking until echo or timeout. State machines remove long delay chains but do not make `pulseIn()` asynchronous.
