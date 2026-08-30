# Modules

`FlySky_4WDCar_FreeDrive.ino` owns receiver, driver, servo, channel mapping and manual motion decisions.

External dependencies:
- `iBus`;
- `RR_Freenove4WDCar_Driver`;
- `RR_Freenove4WDCar_Servo`;
- `Freenove_WS2812B_RGBLED_Controller` indirectly via Driver.

The helper `readChannel/readSwitch` remains in file but main control uses iBus `receiver.get()`.
