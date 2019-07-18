ControllerButton buttonLiftUp(ControllerDigital::up);
ControllerButton buttonLiftDown(ControllerDigital::down);
ControllerButton buttonLiftSmallUp(ControllerDigital::right);
ControllerButton buttonLiftSmallDown(ControllerDigital::left);
ControllerButton buttonRailsToggle(ControllerDigital::R1);
ControllerButton buttonRunAuton(ControllerDigital::Y);
ControllerButton buttonRunIntake(ControllerDigital::L1);
ControllerButton buttonRunOuttake(ControllerDigital::L2);
Controller masterController;

#define DRIVE_LEFT_CONTROL masterController.getAnalog(ControllerAnalog::leftY)
#define DRIVE_RIGHT_CONTROL masterController.getAnalog(ControllerAnalog::rightY)
#define MAX_DRIVE_ACCEL 0.01
#define MAX_DRIVE_DECEL 0.05
