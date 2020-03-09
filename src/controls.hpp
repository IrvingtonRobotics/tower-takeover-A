/**
 * Definitions of joystick controls and controller button mappings
 */

#ifndef SRC_CONTROLS_H_
#define SRC_CONTROLS_H_

#include "main.h"

ControllerButton buttonLiftUp(ControllerDigital::right);
ControllerButton buttonLiftDown(ControllerDigital::left);
ControllerButton buttonGlideUp(ControllerDigital::up);
ControllerButton buttonGlideDown(ControllerDigital::down);
ControllerButton buttonRailsToggle(ControllerDigital::R1);
ControllerButton buttonRailsToggleMid(ControllerDigital::R2);
ControllerButton buttonScootRails(ControllerDigital::B);
ControllerButton buttonKill(ControllerDigital::X);
ControllerButton buttonFakeTareRails(ControllerDigital::Y);
ControllerButton buttonRunIntake(ControllerDigital::L1);
ControllerButton buttonRunOuttake(ControllerDigital::L2);
Controller masterController;

// Macros expanded to get X and Y joystick values
#define DRIVE_X_CONTROL masterController.getAnalog(ControllerAnalog::rightX)
#define DRIVE_Y_CONTROL masterController.getAnalog(ControllerAnalog::rightY)

#endif
