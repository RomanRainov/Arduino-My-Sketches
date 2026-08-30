# Limitations and confirmed issues

1. Receiver-loss path in `FlySky_4WDCar_COMPLEX.ino` returns when `receiver.get(0) == 0` without an explicit `driver.stop()`. Treat failsafe behavior as NEEDS VERIFICATION before relying on it.
2. `RRFreenove4WDCarServo::setup()` calls `setPosition(_startPosition + _offset)`, while main constructs it with `SERVO_START_POS = 90 + SERVO_OFFSET`; startup offset is therefore applied twice.
3. `RRFreenove4WDCarDriver::setup()` waits forever in `while (!_strip.begin());` if the RGB controller does not respond.
4. SWC mapping uses two static thresholds. Despite the comment, the current implementation is not stateful hysteresis.
5. Automatic modes avoid long `delay()` chains, but each sonar read can still block until the configured bounded `pulseIn()` timeout.
6. Mode indication is blocking (`beep()` and `delay()`), but the code stops motors before running indication.
7. A0 is shared by battery ADC and buzzer/tone. Battery reads set A0 to INPUT; buzzer methods/tone reuse the same pin. This is a deliberate software multiplex only if the hardware actually supports it; electrical correctness is NEEDS VERIFICATION.
8. `RR_Freenove4WDCar_TurnSignal` uses Timer2 and Serial debug, so it must not be enabled unchanged in this project.
9. Local libraries under `Freenove4WDCar_FIXED/Libraries` differ from `RomanRainov/RR_Arduino_Libraries`; do not silently substitute one set for the other.
10. Exact motor-driver IC, regulator topology/current, motor stall current, servo peak current, receiver failsafe behavior and battery-cell/protection details remain `UNKNOWN` / `NEEDS VERIFICATION`.
