/**
 * Opcontrol, run during driver control period (1:45)
 */

#include "common.hpp"
#include "controls.hpp"
#include "utils.hpp"

/**
 * Run the Drive subsystem based on the joysticks
 */
void runDrive() {
  drive.move(DRIVE_X_CONTROL, DRIVE_Y_CONTROL);
}

/**
 * Run the Lift subsystem
 * Controls intentional movement (small and non-small)
 * Controls lowering to button during opcontrol
 * Controls querying for lift location
 */
void runLift() {
  lift.checkTare();
  bool smallUp = buttonLiftSmallUp.isPressed();
  bool up = buttonLiftUp.changedToPressed();
  bool smallDown = buttonLiftSmallDown.isPressed();
  bool down = buttonLiftDown.changedToPressed();
  bool isIncrease = smallUp || up;
  bool isSmall = smallUp || smallDown;
  if (smallUp || up || smallDown || down) {
    // printf("moving %d %d\n", isIncrease, isSmall);
    lift.move(isIncrease, isSmall);
  }
  if (buttonRetare.changedToPressed()) {
    lift.lowerToButton();
  }
  if (buttonQuery.changedToPressed()) {
    lift.query();
  }
}

/**
 * Run the Rails subsystem
 * Simply toggle between the two positions based on buttons
 */
void runRails() {
  if (buttonRailsToggle.changedToPressed() || buttonRailsToggle2.changedToPressed()) {
    rails.togglePosition();
  }
}

/**
 * Run the Intake subsytem.
 * Intake, outtake, or stop based on buttons
 */
void runIntake() {
  if (buttonRunIntake.isPressed()) {
    intake.intake();
  } else if (buttonRunOuttake.isPressed()) {
    intake.outtake();
  } else {
    intake.stop();
  }
}

/**
 * Loop control over all the subsystems
 * This is not multi-threaded, so any subsystem can block out movement
 * of the rest
 * TODO (Impatient from single-threading):
 *   See https://pros.cs.purdue.edu/v5/tutorials/topical/multitasking.html
 * to implement multitasking
 */
void opcontrol() {
  // lift.lowerToButton();
  printf("loopy\n");
  printf(buttonFoldout.isPressed() ? "pressed" : "");
	while (true) {
    // will fold out in autonomous
    // TODO (Autonomous ready): move this to autonomous
    if (buttonFoldout.changedToPressed()) {
      printf("folding out\n");
      foldout();
    }
		runDrive();
    runLift();
    runRails();
    runIntake();
	  pros::delay(10);
	}
}
