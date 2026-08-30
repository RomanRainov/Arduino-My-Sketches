# Current confirmed issues

1. Active `Serial.print/println` in setup conflicts with iBus on hardware Serial.
2. Receiver-loss path returns without explicit `driver.stop()`.
3. Driver setup can block forever on missing RGB controller (`while (!_strip.begin());`).
4. RR Motor voltage compensation is currently unreachable after an early return.
5. Servo setup double-applies offset relative to `SERVO_START_POS`.
6. Several geometry constants/calculation variables are present but not used in actual steering path.
7. D13 activity LED prevents treating SPI SCK as free.
