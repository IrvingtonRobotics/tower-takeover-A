#include "common.hpp"

ControllerButton buttonUp(ControllerDigital::R1);
ControllerButton buttonDown(ControllerDigital::R2);
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

int goalHeight = 0;
void runLift() {
	if (buttonUp.changedToPressed() && goalHeight < numHeights - 1) {
		goalHeight++;
    printf("Moved up: new height %d\n", goalHeight);
		moveLift(goalHeight);
	}  else if (buttonDown.changedToPressed() && goalHeight > 0) {
		goalHeight--;
    printf("Moved down: new height %d\n", goalHeight);
    moveLift(goalHeight);
	}
}

void opcontrol() {
	while (true) {
		runAuton();
		runDrive();
    runLift();
	  pros::delay(10);
	}
}
