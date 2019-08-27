#include "common.hpp"
#include "controls.hpp"
#include "utils.hpp"

void runDrive() {
  drive.move(DRIVE_X_CONTROL, DRIVE_Y_CONTROL);
}

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
  // lift.lowerToButton();
  printf("loopy\n");
  printf(buttonFoldout.isPressed() ? "pressed" : "");
	while (true) {
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
