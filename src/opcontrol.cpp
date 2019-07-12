#include "common.hpp"

ControllerButton buttonLiftUp(ControllerDigital::up);
ControllerButton buttonLiftDown(ControllerDigital::down);
ControllerButton buttonLiftSmallUp(ControllerDigital::right);
ControllerButton buttonLiftSmallDown(ControllerDigital::left);
ControllerButton buttonRailsToggle(ControllerDigital::R1);
ControllerButton buttonRunAuton(ControllerDigital::Y);
ControllerButton buttonRunIntake(ControllerDigital::L1);
ControllerButton buttonRunOuttake(ControllerDigital::L2);
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
