#include "common.hpp"
#include "config.hpp"

Timer autonTimer;

void foldout(bool doBackward) {
  printf("FOLD OUT START %f\n", autonTimer.getDtFromStart().getValue());
  // home rails while lifting
  if (MODE != DRIVER) {
    rails.backToButton();
  }
  // foldout
  lift.move(27_in);
  intake.outtake();
  lift.waitUntilSettled();
  pros::delay(200);
  // return
  lift.setMaxVelocity(500);
  lift.move(0);
  intake.stop();
  printf("INTAKE STOPPED %f\n", autonTimer.getDtFromStart().getValue());
  drive.moveDistance(2_in);
  printf("DRIVE FORWARD\n");
  if (doBackward) {
    drive.moveDistance(-2_in);
  }
  printf("DRIVE BACKWARD\n");
  lift.resetMaxVelocity();
  printf("FINISHED FOLDOUT\n");

  // time to wobble to a stop
  pros::delay(50);
}

void foldout() {
  foldout(true);
}

/**
 * Helper function to move between points
 */
void travelProfile(std::initializer_list<okapi::Point> iwaypoints,
  bool backwards, float speed
) {
  // hopefully little overhead here
  auto profileController = drive.getProfileController(speed);
  string name = "current";
  profileController.generatePath(iwaypoints, name);
  profileController.setTarget(name, backwards);
  profileController.waitUntilSettled();
  profileController.removePath(name);
}

void stack() {
  /**
   * Start with cube above goal
   * End with intake outtaking:
   *  - need to back up instantly
   */
  // release stack
  // intake while stacking
  intake.move(150);
  pros::delay(50);
  intake.move(-50);
  pros::delay(120);
  rails.moveForward(190);
  pros::delay(200);
  intake.stop();
  rails.waitUntilSettled();
  // let the stack wobble
  pros::delay(200);
  // outtake after stacking
  intake.move(-200);
  rails.moveBack(200);
}
