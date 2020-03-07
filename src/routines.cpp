#include "common.hpp"

Timer autonTimer;

void foldout(bool doBackward) {
  printf("FOLD OUT START %f\n", autonTimer.getDtFromStart().getValue());
  // home rails while lifting
  if (MODE != DRIVER) {
    rails.backToButton();
  }
  // foldout
  lift.setMaxVelocity(700);
  lift.move(29_in);
  lift.waitUntilSettled();
  pros::delay(400);
  // return
  lift.setMaxVelocity(1200);
  lift.move(1_in);
  lift.waitUntilSettled();
  lift.resetMaxVelocity();
  lift.move(0);

  // time to wobble to a stop ig
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

void stack(int cubes, bool outtakeLoad) {
  /**
   * Start with cube above goal
   * End with intake outtaking:
   *  - need to back up instantly
   */
  // release stack
  if (outtakeLoad) {
    // outtake prep while stacking
    intake.move(-150);
    pros::delay(800);
  }
  intake.move(-100);
  // TODO: incorporate #cubes to go faster with few
  rails.stackForward();
  pros::delay(150);
  intake.stop();
  rails.waitUntilSettled();
  // let the stack wobble
  pros::delay(1000);
  // outtake after stacking
  intake.move(-120);
}

void stack(int cubes) {
  stack(cubes, true);
}
