# RGB LED Controller Skill

Freenove controller through I2C 0x20, 10 LEDs, GRB; Uno bus A4/A5.

Current RR Driver setup waits forever on `_strip.begin()`. Also, this sketch calls `driver.setAllLedsColor()`, but that method is not present in current RR Driver public header.
