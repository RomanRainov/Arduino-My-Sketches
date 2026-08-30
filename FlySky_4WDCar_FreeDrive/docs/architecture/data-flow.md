# Data flow

- ch1 → turn value -100..100;
- ch2 → speed -100..100;
- ch4 → servo angle mapping -90..90;
- receiver zero check → early return.

Motion calls: forwardLeft/forwardRight/backwardLeft/backwardRight/rotateLeft/rotateRight/stop.

Current setup also sends calibration values to hardware Serial, conflicting with iBus transport.
