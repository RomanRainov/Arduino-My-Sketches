# RGB LED Controller Skill

RR Driver internally uses Freenove WS2812B controller over I2C address 0x20, 10 LEDs, GRB; Uno A4/A5.

Driver setup waits forever on `_strip.begin()` if controller is unavailable.
