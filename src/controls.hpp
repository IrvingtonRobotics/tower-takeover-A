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

#define DRIVE_LEFT_CONTROL masterController.getAnalog(ControllerAnalog::leftY)
#define DRIVE_RIGHT_CONTROL masterController.getAnalog(ControllerAnalog::rightY)
