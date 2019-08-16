#include "common.hpp"
#include "controls.hpp"

void runDrive() {
  drive.move(DRIVE_LEFT_CONTROL, DRIVE_RIGHT_CONTROL);
}

void runAuton() {
	if (buttonRunAuton.changedToPressed()) {
		autonomous();
	}
}

void runLift() {
  lift.checkTare();
  bool smallUp = buttonLiftSmallUp.changedToPressed();
  bool up = buttonLiftUp.changedToPressed();
  bool smallDown = buttonLiftSmallDown.changedToPressed();
  bool down = buttonLiftDown.changedToPressed();
  bool isIncrease = smallUp || up;
  bool isSmall = smallUp || smallDown;
  if (smallUp || up || smallDown || down) {
    lift.move(isIncrease, isSmall);
  }
}

void runRails() {
  if (buttonRailsToggle.changedToPressed() || buttonRailsToggle2.changedToPressed()) {
    rails.togglePosition();
  }
}

void runIntake() {
  if (buttonRunIntake.isPressed()) {
    intake.move(100);
  } else if (buttonRunOuttake.isPressed()) {
    intake.move(-100);
  } else {
    intake.move(0);
  }
}

void opcontrol() {
	while (true) {
		runAuton();
		runDrive();
    runLift();
    runRails();
    runIntake();
	  pros::delay(10);
	}
}
