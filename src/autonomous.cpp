/**
 * Autonomous, run during autonomous period (0:15).
 * Each subsystem is not split into tasks because autonomous does one at a time.
 * If you want to multitask autonomous, put the tasks inside the subsystem
 * classes so they initialize when the subsystem classes
 *
 * All of this code assumes blue side and gets automatically flipped
 */

#include "common.hpp"

// set this to true (for setup) to fold in instead of fold out
// use pros upload --slot 2
const bool isFoldin = false;

/**
 * Run foldout at beginning of match
 */
void foldout() {
  printf("fold out\n");
  // home rails while lifting
  rails.backToButton();
  // foldout
  lift.move(27_in);
  intake.intake();
  lift.waitUntilSettled();
  // return
  lift.lowerToButton(120);
  intake.stop();
  drive.moveDistance(-3_in);
  drive.moveDistance(3_in);
}

void foldin() {
  printf("fold in\n");
  // lift.lowerToButton(120);
  lift.move(12_in);
  lift.waitUntilSettled();
  rails.backToButton();
  intake.outtake();
  pros::delay(3000);
  intake.stop();
  lift.lowerToButton(120);
}


void travelProfile(std::initializer_list<okapi::Point> iwaypoints, bool backwards) {
  // hopefully little overhead here
  auto profileController = drive.getProfileController();
  string name = "current";
  profileController.generatePath(iwaypoints, name);
  profileController.setTarget(name, backwards);
  profileController.waitUntilSettled();
  profileController.removePath(name);
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  if (isFoldin) {
    foldin();
    return;
  }
  // foldout();
  pros::delay(300);
  travelProfile({
  Point{11.0_in, 116.0_in, 0_deg},
  Point{53.0_in, 116.0_in, 0_deg}
  }, false);
  travelProfile({
    Point{53.0_in, 116.0_in, 0_deg},
    Point{28.0_in, 89.5_in, 75.0_deg}
  }, true);
  travelProfile({
    Point{28.0_in, 89.5_in, -75.0_deg},
    Point{19.14215_in, 126.728_in, -45.0_deg}
  }, false);
  // push cube into endzone
  // drive.moveDistance(20_in);
  // drive.turnAngle(-10_deg);
  // drive.moveDistance(5_in);
  // drive.moveDistance(-10_in);
}
