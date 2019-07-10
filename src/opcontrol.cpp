#include "common.hpp"

ControllerButton buttonLiftUp(ControllerDigital::up);
ControllerButton buttonLiftDown(ControllerDigital::down);
ControllerButton buttonLiftSmallUp(ControllerDigital::right);
ControllerButton buttonLiftSmallDown(ControllerDigital::left);
ControllerButton buttonRailsToggle(ControllerDigital::R1);
ControllerButton buttonRunAuton(ControllerDigital::Y);
Controller masterController;

void runDrive() {
	drive.tank(
		masterController.getAnalog(ControllerAnalog::leftY),
		masterController.getAnalog(ControllerAnalog::rightY)
	);
}

void runAuton() {
	if (buttonRunAuton.changedToPressed()) {
		autonomous();
	}
}

const QLength smallLiftMoveSize = 0.25_in;
QLength getChangedLiftHeight(QLength lastHeight, bool isIncrease, bool isIncreaseSmall) {
  if (isIncreaseSmall) {
    // just move a bit in one direction
    return lastHeight + boolToSign(isIncrease) * smallLiftMoveSize;
  } else {
    // move to the nearest preset in that direction
    int i = 0;
    while (lastHeight > targetHeights[i]) {
      ++i;
    }
    if (isIncrease) {
      if (targetHeights[i] != lastHeight && i < numHeights - 1) {
        return targetHeights[i+1];
      }
    } else {
      if (i != 0) {
        return targetHeights[i-1];
      }
    }
    return lastHeight;
  }
}

void runLift() {
  const double lastTargetTicks = lift.getTarget();
  const QLength lastTargetHeight = getLiftHeight(lastTargetTicks);

  bool smallUp = buttonLiftSmallUp.changedToPressed();
  bool up = buttonLiftUp.changedToPressed();
  bool smallDown = buttonLiftSmallDown.changedToPressed();
  bool down = buttonLiftDown.changedToPressed();
  bool isIncrease = smallUp || up;
  bool isSmall = smallUp || smallDown;
  if (smallUp || up || smallDown || down) {
    QLength newHeight = getChangedLiftHeight(lastTargetHeight, isIncrease, isSmall);
    moveLift(newHeight);
  }
}

void runRails() {
  if (buttonRailsToggle.changedToPressed()) {
    toggleRails();
  }
}

void opcontrol() {
	while (true) {
		runAuton();
		runDrive();
    runLift();
    runRails();
	  pros::delay(10);
	}
}
