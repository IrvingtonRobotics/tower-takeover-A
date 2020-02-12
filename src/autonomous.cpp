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

// to override MODE
#include "config.hpp"

#ifndef MODE
#define MODE SMALL_SIDE
#endif

#define QUICK_SUCK_SPEED 900

void runChecksFn(void* param) {
  while (true) {
    rails.step();
    pros::delay(10);
  }
}

void timeoutFn(void* param) {
  printf("TIMEOUT STARTED %f\n", autonTimer.getDtFromStart().getValue());
  if (MODE == SMALL_SIDE || MODE == BIG_SIDE) {
    pros::delay(14200);
    printf("TIMEOUT RESOLVED %f\n", autonTimer.getDtFromStart().getValue());
    // backup
    drive.moveDistance(-8_in);
    intake.stop();
  }
}

void smallSideAuton() {
  foldout(true);
  // small side 2
  // suck up cubes
  intake.move(QUICK_SUCK_SPEED);
  travelProfile({
    Point{11.0_in, -116.0_in, getAngle()},
    Point{49.0_in, -116.0_in, 0_deg}
  }, false, 0.16);
  intake.stop();
  // move to stack
  drive.turnAngle(180_deg);
  travelProfile({
    Point{49.5_in, -116.0_in, 180_deg},
    Point{19.3137_in, -125.3135_in, -135.0_deg}
  }, false, 0.4);
  stack();
}

void bigSideAuton() {
  foldout(false);
  intake.move(QUICK_SUCK_SPEED);
  travelProfile({
    Point{11.0063_in, 31.1398_in, 102.635_deg},
    Point{13.9627_in, 21.1192_in, 90.0_deg}
  }, true, 0.5);
  travelProfile({
    Point{13.9627_in, -21.1192_in, -90.0_deg},
    // Point{15.7404_in, -34.2143_in, -83.408_deg},
    // Point{15.2404_in, -34.7143_in, -44.7908_deg},
    Point{25.5_in, -44.5_in, -42.884_deg}
  }, false, 0.5);
  intake.stop();
  drive.turnAngle(-119.0861_deg);
  intake.move(QUICK_SUCK_SPEED);
  travelProfile({
    Point{25.5_in, -44.5_in, 76.2021_deg},
    Point{28.4632_in, -24.3996_in, 88.1446_deg}
  }, false, 0.5);
  intake.stop();
  drive.turnAngle(-43.3394_deg);
  intake.move(QUICK_SUCK_SPEED);
  travelProfile({
    Point{28.4632_in, -24.3996_in, 131.484_deg},
    Point{22.9851_in, -17.7386_in, 134.766_deg}
  }, false, 0.5);
  stack();
}

void skillsAuton() {
  if (isRed) {
    return;
  }
  const QLength EXPECTED_POS = 693_mm;
  if ((getSideDist() - EXPECTED_POS) > 6_in) {
    return;
  }
  printf("Side dist%f", getSideDist().getValue());
  printf("Angle%f", getAngle().getValue());

  // autogen
  foldout(true);
  intake.move(QUICK_SUCK_SPEED);
  // pick up first 8-9 cubes
  travelProfile({
    Point{13.0_in, -117.0_in, 0_deg},
    Point{49.0_in, -117.0_in, 0_deg},
    Point{71.5_in, -118.5_in, 0_deg},
  }, false, 0.18);
  travelProfile({
    Point{71.5_in, -118.5_in, 0_deg},
    Point{89.826_in, -117.738_in, -0.0_deg},
    Point{128.5_in, -117.0_in, -0.0_deg},
  }, false, 0.15);
  travelProfile({
    Point{128.5_in, 117.0_in, 0.0_deg},
    Point{118.0_in, 117.0_in, 0.0_deg},
  }, true, 0.15);
  travelProfile({
    Point{118.0_in, -117.0_in, -0.0_deg},
    Point{133.728_in, -126.728_in, -45.0_deg},
  }, false, 0.14);
  stack();
  travelProfile({
    Point{133.728_in, 126.728_in, 45.0_deg},
    Point{126.228_in, 119.228_in, 45.0_deg},
  }, true, 0.25);
  // stop outtaking from stacking
  intake.stop();
  // tower 1
  drive.turnAngle(-180.0_deg);
  intake.move(QUICK_SUCK_SPEED);
  travelProfile({
    Point{127.228_in, -119.228_in, 135.0_deg},
    Point{82.1275_in, -107.8135_in, -180.0_deg},
  }, false, 0.25);
  travelProfile({
    Point{82.1275_in, 107.8135_in, 180.0_deg},
    Point{88.6275_in, 107.8135_in, 180.0_deg},
  }, true, 0.25);
  // lift cube
  intake.move(-100);
  pros::delay(100);
  intake.stop();
  lift.move(1); // small tower
  lift.waitUntilSettled();
  intake.move(-150);
  travelProfile({
    Point{88.6275_in, -107.8135_in, -180.0_deg},
    Point{84.6275_in, -107.8135_in, -180.0_deg},
  }, false, 0.15);
  intake.stop();
  travelProfile({
    Point{84.6275_in, 107.8135_in, 180.0_deg},
    Point{91.1275_in, 107.8135_in, 180.0_deg},
  }, true, 0.25);

  // end autogen
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
}
