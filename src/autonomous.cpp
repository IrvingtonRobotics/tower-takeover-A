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
  intake.outtake();
  lift.waitUntilSettled();
  // return
  lift.move(0);
  intake.stop();
  drive.moveDistance(3.5_in);
  drive.moveDistance(-3.5_in);
}

void foldin() {
  printf("fold in\n");
  // lift.lowerToButton(120);
  lift.move(12_in);
  lift.waitUntilSettled();
  rails.backToButton();
  intake.intake();
  pros::delay(3000);
  intake.stop();
  lift.lowerToButton(160);
}


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
  foldout();
  pros::delay(100);
  intake.move(600);
  if (isSmallSide) {
    // drive forward and suck
    travelProfile({
      Point{11.0_in, -116.0_in, 0_deg},
      Point{49.0_in, -116.0_in, 0_deg}
    }, false, 0.3);
    intake.stop();
    // drive backward and turn
    travelProfile({
      Point{49.0_in, 116.0_in, 0_deg},
      Point{31.5_in, 91.5_in, 90.0_deg}
    }, true, 0.45);
    // drive forward to endzone
    travelProfile({
      Point{31.5_in, -91.5_in, -90.0_deg},
      Point{19.14215_in, -119.228_in, -135.0_deg}
    }, false, 0.35);
  } else {
    drive.moveDistance(-1_in);
    travelProfile({
      Point{9.5127_in, -46.0127_in, -0.0910796_deg},
      Point{27.0_in, -46.5_in, 0_deg}
    }, false, 0.35);
    travelProfile({
      Point{27.0_in, 46.5_in, 0_deg},
      Point{14.788_in, 57.288_in, -41.1375_deg}
    }, true, 0.35);

    travelProfile({
      Point{14.788_in, -57.288_in, 41.1375_deg},
      Point{28.57565_in, -31.57565_in, 107.945_deg},
    }, false, 0.35);
    intake.stop();
    travelProfile({
      Point{28.57565_in, -31.57565_in, 107.945_deg},
      Point{21.80325_in, -19.30325_in, 132.535_deg}
    }, false, 0.35);
  }
  // release stack
  intake.move(-40);
  rails.moveForward(150);
  rails.waitUntilSettled();
  intake.stop();
  // backup
  travelProfile({
    Point{19.14215_in, 119.228_in, 135.0_deg},
    Point{23.3137_in, 115.3135_in, 135.0_deg}
  }, true, 1.0);
}
