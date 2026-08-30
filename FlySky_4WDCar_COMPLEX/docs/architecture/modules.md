# Modules

- `FlySky_4WDCar_COMPLEX.ino`: composition root, receiver processing, mode selection.
- `FlySky_4WDCar_FreeDrive.ino`: manual steering; channel 3 currently drives Arduino `tone(A0, ...)`, channel 4 moves servo.
- `Automatic_Stay_Close_To_Obstacle.ino`: median of five sonar samples, distance bands, forward/back/stop.
- `Automatic_Obstacle_Avoidance.ino`: blocking 3-angle scan, direct `motorRun()`, local battery compensation.
- RR libraries: external repo `RomanRainov/RR_Arduino_Libraries`.

When changing one mode, keep global names unique because Arduino concatenates `.ino` files in a sketch.
