# VEX Tower Takeover Code
**Team 44730A Bluescreens**

[Code joined together](cat.md)

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
| ADI B | Rails limit switch |
| ADI E | Lift limit switch |


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

## Project Structure
Inside the `src/` folder is all of the juicy stuff.

Of course, `autonomous.cpp` is where most of the autonomous lies, but the foldout
routine is in `routines.cpp`. The default color is in `initialize.cpp`.

Subsystem configuration is available at the top of the class declaration in
`drive.cpp`, `intake.cpp`, `lift.cpp`, and `rails.cpp`. The portion directly
under a `---- CONFIG ----` heading is all you need to edit to tweak values.
