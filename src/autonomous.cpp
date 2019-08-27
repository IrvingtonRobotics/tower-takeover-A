/**
 * Autonomous, run during autonomous period (0:15).
 * Each subsystem is not split into tasks because autonomous does one at a time.
 * If you want to multitask autonomous, put the tasks inside the subsystem
 * classes so they initialize when the subsystem classes
 */

#include "common.hpp"

/**
 * Run foldout at beginning of match
 */
void foldout() {
  printf("fold out\n");
  // push rails
  rails.moveForward();
  rails.waitUntilSettled();
  // return Rails
  rails.moveBack();
  rails.waitUntilSettled();
  // pause
  pros::delay(100);
  // raise arm
  lift.move(15_in);
  lift.waitUntilSettled();
  // lower arm
  lift.lowerToButton();
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
	foldout();
	pros::delay(1000);
	// printf("Moving 1\n");
	drive.moveDistance(12_in);
	// printf("Done moving 1\n");
	// printf("Moving 1\n");
	drive.moveDistance(12_in);
	// printf("Done moving 1\n");
}
