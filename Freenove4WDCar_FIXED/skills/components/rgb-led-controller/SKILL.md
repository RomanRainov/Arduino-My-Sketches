# RGB LED Controller Skill

Freenove WS2812B RGB LED controller is accessed through I2C address `0x20` with 10 LEDs in GRB order.

Arduino Uno bus: A4 SDA, A5 SCL.

`RRFreenove4WDCarDriver::setup()` currently executes `while (!_strip.begin());`, so missing/unresponsive RGB hardware can block startup forever.

Driver API includes:
- `setLedColor()`
- `setLedsColor()`
- `setAllLedsColor()`

Do not assume the RGB LEDs are driven directly from an Arduino GPIO. They are behind the Freenove I2C controller.
