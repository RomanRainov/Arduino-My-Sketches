# Architecture overview

```text
FlySky receiver
  → iBus / Serial
  → channel mapping
  → RR Driver → motors + RGB
  → RR Servo → steering/sonar mount servo
```

This standalone sketch has one manual-control mode. No sonar object, obstacle mode or buzzer object is instantiated.
