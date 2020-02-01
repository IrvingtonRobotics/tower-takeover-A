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

void runChecksFn(void* param) {
  while (true) {
    lift.checkTare();
    rails.step();
    pros::delay(10);
  }
}

void timeoutFn(void* param) {
  printf("TIMEOUT STARTED %f\n", autonTimer.getDtFromStart().getValue());
  pros::delay(14200);
  printf("TIMEOUT RESOLVED %f\n", autonTimer.getDtFromStart().getValue());
  // backup
  drive.moveDistance(-8_in);
  intake.stop();
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
  autonTimer = Timer();
  pros::Task runChecksTask(runChecksFn);
  if (isFoldin) {
    foldin();
    return;
  }
  pros::Task timeoutTask(timeoutFn);
  foldout();
  pros::delay(30);
  printf("DONE DELAYING %f\n", autonTimer.getDtFromStart().getValue());
  intake.move(900);
  printf("INTAKE ON %f\n", autonTimer.getDtFromStart().getValue());
  // pick up cubes and go to goal zone
  if (isSmallSide) {
    // small side 2
    travelProfile({
      Point{11.0_in, -116.0_in, 0_deg},
      Point{49.0_in, -116.0_in, 0_deg}
    }, false, 0.16);
    intake.stop();
    drive.turnAngle(180_deg);
    travelProfile({
      Point{49.5_in, -116.0_in, 180_deg},
      Point{18.8137_in, -126.3135_in, -135.0_deg}
    }, false, 0.4);
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
  // keep drive forward
  drive.moveTank(1, 1);
  // release stack
  intake.move(-80);
  rails.moveForward(150);
  rails.waitUntilSettled();
}
