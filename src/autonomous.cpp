/**
 * Autonomous, run during autonomous period (0:15).
 * Each subsystem is not split into tasks because autonomous does one at a time.
 * If you want to multitask autonomous, put the tasks inside the subsystem
 * classes so they initialize when the subsystem classes
 *
 * All of this code assumes blue side and gets automatically flipped
 */

#include "common.hpp"
#include "ultrasonic.hpp"

#define SMALL_SIDE 1
#define BIG_SIDE 2
#define SKILLS 3

#define MODE SMALL_SIDE

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

void smallSideAuton() {
  // small side 2
  travelProfile({
    Point{11.0_in, -116.0_in, getAngle()},
    Point{49.0_in, -116.0_in, 0_deg}
  }, false, 0.16);
  intake.stop();
  drive.turnAngle(180_deg);
  travelProfile({
    Point{49.5_in, -116.0_in, 180_deg},
    Point{19.3137_in, -125.3135_in, -135.0_deg}
  }, false, 0.4);
}

void bigSideAuton() {

}

void skillsAuton() {

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
  pros::Task timeoutTask(timeoutFn);
  foldout();
  pros::delay(50);
  intake.move(900);
  // pick up cubes and go to goal zone
  switch (MODE) {
    case SMALL_SIDE:
      smallSideAuton();
      break;
    case BIG_SIDE:
      bigSideAuton();
      break;
    case SKILLS:
      skillsAuton();
      break;
  }
  // keep drive forward
  drive.moveTank(1, 1);
  // release stack
  intake.move(-80);
  pros::delay(50);
  rails.moveForward(190);
  rails.waitUntilSettled();
}
