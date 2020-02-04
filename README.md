# VEX Tower Takeover Code
**Team 44730A Bluescreens**

## Uploading
You can make and upload just with the standard `pros make all` and `pros upload`, but to utilize default slots, do the following:

```bash
# make quick, then upload small side auton after a confirmation
./proj.py small
# make all, then upload big side auton after a confirmation
./proj.py big -a
# make all, then upload programming skills side auton after no confirmation
./proj.py skills -au
# make all, then upload driver skills (flipout in auton) after no confirmation, then open terminal
./proj.py driver -aut
```

The flags can of course be mixed and matched.

## Controls

### Primary Controller
| Position | Description |
|----------|-------------|
| Right Joystick | Drive control (arcade) |
| Left Joystick | Move to toggle slow mode |
| R1 or R2 | Toggle Rails Angle |
| L1 | Run intake |
| L2 | Run outtake |
| Up/Down arrows | Lift up/down to presets |
| Right arrow | Flip out |
| Left arrow | Help fold in |
| A | Smooth lift to top position |
| B | Re-tare lift |
| X | Stop all movement (Emergency kill). Pressing this again will enable movement |
| Y | Tare Rails |

Note: The emergency kill will order all motors to stop moving and hold their position.
It will not cut power to the motors. To do that, hold the power off button.

## Wiring
| Port | Description |
|------------|----------|
| ADI E | Lift limit switch |
| ADI C | Rails limit bumper |
| ADI F | Auton selector potentiometer |
| ADI A | Left ultra PING (OUTPUT) |
| ADI B | Left ultra ECHO (INPUT) |
| ADI G | Right ultra PING (OUTPUT) |
| ADI H | Right ultra ECHO (INPUT) |

### Motors
| Motor Port | Function |
|------------|----------|
| 10 | Left front drive motor |
| 2 | Left back drive motor |
| 9 | Right front drive motor |
| 1 | Right back drive motor |
| 15 | Lift motor |
| 18 | Rails angling motor |
| 12 | Left intake motor |
| 13 | Right intake motor  |
