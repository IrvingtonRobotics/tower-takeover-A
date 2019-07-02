#include "common.cpp"

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

void opcontrol() {
	while (true) {
		runAuton();
		runDrive();
	  pros::delay(10);
	}
}
