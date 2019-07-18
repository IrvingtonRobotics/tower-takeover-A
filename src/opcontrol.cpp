#include "common.hpp"
#include "controls.hpp"

float getNewDriveSpeed(float current, float input) {
  // tried using clamp, but difficulties
  float lo;
  float hi;
  if (current >= 0) {
    lo = current - MAX_DRIVE_DECEL;
    hi = current + MAX_DRIVE_ACCEL;
  } else {
    lo = current - MAX_DRIVE_ACCEL;
    hi = current + MAX_DRIVE_DECEL;
  }
  if (lo > input) return lo;
  if (hi < input) return hi;
  return input;
}

float leftDriveSpeed = 0;
float rightDriveSpeed = 0;
void runDrive() {
  leftDriveSpeed = getNewDriveSpeed(leftDriveSpeed, DRIVE_LEFT_CONTROL);
  rightDriveSpeed = getNewDriveSpeed(rightDriveSpeed, DRIVE_RIGHT_CONTROL);
	drive.tank(leftDriveSpeed, rightDriveSpeed);
}

void runAuton() {
	if (buttonRunAuton.changedToPressed()) {
		autonomous();
	}
}

void runLift() {
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
  if (buttonRailsToggle.changedToPressed()) {
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
