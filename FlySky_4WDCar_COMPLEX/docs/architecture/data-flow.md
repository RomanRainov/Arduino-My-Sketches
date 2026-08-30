# Data flow

Channels: 1 Aileron, 2 Elevator, 3 Throttle, 4 Rudder, 5 SWC, 6 SWA.

Main mode selection currently checks exact SWC values:
- 1500 → Stay Close
- 2000 → Obstacle Avoidance
- otherwise → FreeDrive

FreeDrive maps channels 1/2 to motion, channel 3 to tone frequency, channel 4 to servo angle.

Automatic modes sample sonar and generate motion decisions. Obstacle Avoidance bypasses Driver for actual motor outputs.
