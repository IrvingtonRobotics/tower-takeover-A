ControllerButton buttonLiftUp(ControllerDigital::up);
ControllerButton buttonLiftDown(ControllerDigital::down);
ControllerButton buttonLiftSmallUp(ControllerDigital::right);
ControllerButton buttonLiftSmallDown(ControllerDigital::left);
ControllerButton buttonRailsToggle(ControllerDigital::R1);
ControllerButton buttonRailsToggle2(ControllerDigital::R2);
ControllerButton buttonRunAuton(ControllerDigital::Y);
ControllerButton buttonRetare(ControllerDigital::B);
ControllerButton buttonQuery(ControllerDigital::X);
ControllerButton buttonLift90(ControllerDigital::A);
ControllerButton buttonRunIntake(ControllerDigital::L1);
ControllerButton buttonRunOuttake(ControllerDigital::L2);
Controller masterController;

#define DRIVE_X_CONTROL masterController.getAnalog(ControllerAnalog::rightX)
#define DRIVE_Y_CONTROL masterController.getAnalog(ControllerAnalog::rightY)
