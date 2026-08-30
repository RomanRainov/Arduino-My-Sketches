# Modules — Freenove4WDCar_FIXED

- `FlySky_4WDCar_COMPLEX/FlySky_4WDCar_COMPLEX.ino`: composition root, hardware objects, FlySky receiver, SWC mode selection, mode transitions and dispatch.
- `FlySky_4WDCar_COMPLEX/FreeDrive.ino`: manual drive from channels 1/2; channel 3 controls tone on A0; channel 4 controls servo angle.
- `FlySky_4WDCar_COMPLEX/Automatic_Obstacle_Avoidance.ino`: non-blocking state machine around bounded sonar reads; cruise, scan, optional reverse, turn and clear-path confirmation.
- `FlySky_4WDCar_COMPLEX/Automatic_Stay_Close_To_Obstacle.ino`: continuous five-angle sonar scan, moving-target candidate scoring, target continuity and 30–50 cm distance control.
- `Libraries/RR_Freenove4WDCar_Driver`: high-level motion and RGB indication.
- `Libraries/RR_Freenove4WDCar_Motor`: motor PWM, ramp/slew and battery compensation.
- `Libraries/RR_Freenove4WDCar_Battery`: A0 battery ADC conversion.
- `Libraries/RR_Freenove4WDCar_Buzzer`: digital beep and Arduino tone API.
- `Libraries/RR_Freenove4WDCar_Servo`: Servo wrapper on D2.
- `Libraries/RR_Freenove4WDCar_Sonar`: HC-SR04 wrapper with bounded `pulseIn()` timeout.
- `Libraries/RR_Freenove4WDCar_TurnSignal`: currently not used by the sketch; uses Timer2 and Serial and conflicts with active project resources.

Arduino concatenates `.ino` files of one sketch. Keep global/static names unique and prefer file-local `static` helpers for mode internals.
