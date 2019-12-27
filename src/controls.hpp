/**
 * Definitions of joystick controls and controller button mappings
 */

#ifndef SRC_CONTROLS_H_
#define SRC_CONTROLS_H_

#include "main.h"

ControllerButton buttonLiftUp(ControllerDigital::up);
ControllerButton buttonLiftDown(ControllerDigital::down);
ControllerButton buttonGlide1(ControllerDigital::right);
ControllerButton buttonGlide2(ControllerDigital::left);
ControllerButton buttonRailsToggle(ControllerDigital::R1);
ControllerButton buttonRailsToggleMid(ControllerDigital::R2);
ControllerButton buttonRetareLift(ControllerDigital::B);
ControllerButton buttonKill(ControllerDigital::X);
ControllerButton buttonTareRails(ControllerDigital::Y);
ControllerButton buttonRunIntake(ControllerDigital::L1);
ControllerButton buttonRunOuttake(ControllerDigital::L2);
Controller masterController;

// Macros expanded to get X and Y joystick values
#define DRIVE_X_CONTROL masterController.getAnalog(ControllerAnalog::rightX)
#define DRIVE_Y_CONTROL masterController.getAnalog(ControllerAnalog::rightY)

float slowDistance() {
  float x = masterController.getAnalog(ControllerAnalog::leftX);
  float y = masterController.getAnalog(ControllerAnalog::leftY);
  return x*x + y*y;
}

#endif
