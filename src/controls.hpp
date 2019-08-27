/**
 * Definitions of joystick controls and controller button mappings
 */

#ifndef SRC_CONTROLS_H_
#define SRC_CONTROLS_H_

ControllerButton buttonLiftUp(ControllerDigital::up);
ControllerButton buttonLiftDown(ControllerDigital::down);
ControllerButton buttonLiftSmallUp(ControllerDigital::right);
ControllerButton buttonLiftSmallDown(ControllerDigital::left);
ControllerButton buttonRailsToggle(ControllerDigital::R1);
ControllerButton buttonRailsToggle2(ControllerDigital::R2);
ControllerButton buttonFoldout(ControllerDigital::A);
ControllerButton buttonRetareLift(ControllerDigital::B);
ControllerButton buttonQuery(ControllerDigital::X);
ControllerButton buttonTareRails(ControllerDigital::Y);
ControllerButton buttonRunIntake(ControllerDigital::L1);
ControllerButton buttonRunOuttake(ControllerDigital::L2);
Controller masterController;

// Macros expanded to get X and Y joystick values
#define DRIVE_X_CONTROL masterController.getAnalog(ControllerAnalog::rightX)
#define DRIVE_Y_CONTROL masterController.getAnalog(ControllerAnalog::rightY)

#endif
