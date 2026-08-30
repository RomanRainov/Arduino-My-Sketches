# Architecture overview

```text
FlySky receiver → iBus/Serial → main mode selector
  ├─ FreeDrive
  ├─ Automatic_Stay_Close_To_Obstacle
  └─ Automatic_Obstacle_Avoidance
       ↓
Driver / direct motorRun()
       ↓
motor GPIO + PWM

Automatic modes → Servo → HC-SR04
Status → buzzer / RGB controller
```

Current implementation is mixed architecture: FreeDrive uses RR Driver, while obstacle avoidance also contains direct low-level motor and battery functions.
