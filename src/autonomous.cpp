/**
 * Autonomous, run during autonomous period (0:15).
 * Each subsystem is not split into tasks because autonomous does one at a time.
 * If you want to multitask autonomous, put the tasks inside the subsystem
 * classes so they initialize when the subsystem classes
 *
 * All of this code assumes blue side and gets automatically flipped
 */

#include "common.hpp"

#define POT_RED_POS 1100
#define POT_BLUE_POS 1600

/* ---- CONFIG ---- */
// default start settings
bool isRed = IS_RED;

#define QUICK_SUCK_SPEED 900

posAngle expectedPA;

void runChecksFn(void* param) {
  while (true) {
    lift.step();
    rails.step();
    pros::delay(10);
  }
}

void timeoutFn(void* param) {
  printf("TIMEOUT STARTED %f\n", autonTimer.getDtFromStart().getValue());
  if (MODE == SMALL_SIDE || MODE == BIG_SIDE) {
    pros::delay(13600);
    printf("TIMEOUT RESOLVED %f\n", autonTimer.getDtFromStart().getValue());
    // backup
    drive.setMaxVelocity(5.7_in/1_s);
    intake.move(-300);
    pros::delay(200);
    drive.moveDistance(-15_in);
    intake.stop();
    drive.resetMaxVelocity(); // lol this fast
  }
}

void angleMeasureFn(void* param) {
  pros::delay(200);
  expectedPA.theta = getAngle(EAST);
}

void smallSideAuton() {
  // begin autogen
  foldout(true);
  intake.move(QUICK_SUCK_SPEED);
  pros::delay(150);
  travelProfile({
    Point{133.7505_in, -116.5_in, -180.0_deg},
    Point{95.7505_in, -118.0_in, 176.942_deg},
  }, false, 0.185);
  travelProfile({
    Point{95.7505_in, -118.0_in, 176.942_deg},
    Point{91.207_in, -114.0735_in, 139.5796_deg},
    Point{86.5415_in, -110.042_in, 170.2791_deg},
  }, false, 0.185);
  travelProfile({
    Point{86.5415_in, 110.042_in, -170.2791_deg},
    Point{92.0415_in, 111.042_in, -170.2791_deg},
  }, true, 0.3);
  drive.turnAngle(-155.693_deg);
  intake.move(-35);
  travelProfile({
    Point{92.0415_in, -111.042_in, -34.0279_deg},
    Point{128.6995_in, -132.7895_in, -31.5202_deg},
  }, false, 0.5);
  stack(6, false);
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
  stack(3);
}

void skillsAuton() {
  // autogen
  foldout(true);
  intake.move(QUICK_SUCK_SPEED);
  // pick up first 8-9 cubes
  pros::delay(150);
  expectedPA = getPosAngle(EAST);
  travelProfile({
    Point{expectedPA.x, -1*expectedPA.y, -1*expectedPA.theta},
    Point{95.0_in, -117.0_in, -180.0_deg},
    Point{77.5_in, -118.5_in, -180.0_deg},
  }, false, 0.18);
  intake.move(-150);
  intake.stop();
  travelProfile({
    Point{77.5_in, -118.5_in, -180.0_deg},
    Point{64.5_in, -118.0_in, 175.475_deg},
  }, false, 0.15);
  intake.move(QUICK_SUCK_SPEED);
  travelProfile({
    Point{64.5_in, -118.0_in, 175.475_deg},
    Point{13.5_in, -115.5_in, -180.0_deg},
  }, false, 0.15);
  travelProfile({
    Point{13.5_in, 115.5_in, 180.0_deg},
    Point{22.0_in, 115.5_in, 180.0_deg},
  }, true, 0.15);
  intake.stop();
  travelProfile({
    Point{22.0_in, -115.5_in, -180.0_deg},
    Point{6.7721_in, -125.728_in, -135.0_deg},
  }, false, 0.14);
  travelProfile({
    Point{6.7721_in, 125.728_in, 135.0_deg},
    Point{7.7721_in, 124.728_in, 135.0_deg},
  }, true, 0.14);
  stack(9);
  drive.resetMaxVelocity();
  travelProfile({
    Point{7.7721_in, 124.728_in, 135.0_deg},
    Point{21.7279_in, 112.228_in, 135.0_deg},
  }, true, 0.1);
  // stop outtaking from stacking
  intake.stop();
  rails.moveBack(200);
  // stabilize position
  drive.turnAngle(-225.0_deg);
  pros::delay(200);
  expectedPA = getPosAngle(SOUTH);
  // tower 1
  intake.move(QUICK_SUCK_SPEED);
  travelProfile({
    Point{expectedPA.x, -1*expectedPA.y, -1*expectedPA.theta},
    Point{24.7761_in, -87.5995_in, 90.0572_deg},
  }, false, 0.25);
  travelProfile({
    Point{24.7761_in, 87.5995_in, -90.0572_deg},
    Point{24.7826_in, 92.5995_in, -90.0572_deg},
  }, true, 0.25);
  intake.move(100);
  pros::delay(500);
  intake.move(-100);
  pros::delay(300);
  intake.stop();
  // lift cube
  lift.move(2); // med tower
  lift.waitUntilSettled();
  travelProfile({
    Point{24.7826_in, -92.5995_in, 90.0572_deg},
    Point{24.7786_in, -90.0995_in, 90.0572_deg},
  }, false, 0.15);
  intake.move(-150);
  pros::delay(100);
  travelProfile({
    Point{24.7786_in, 90.0995_in, -90.0572_deg},
    Point{25.0_in, 99.0_in, -90.0_deg},
  }, true, 0.25);
  intake.stop();
  intake.move(100);
  travelProfile({
    Point{25.0_in, -99.0_in, 90.0_deg},
    Point{16.1061_in, -84.423_in, 107.254_deg},
    Point{13.3796_in, -73.801_in, 92.21_deg},
    Point{13.1331_in, -40.6061_in, 90.7668_deg},
  }, false, 0.4);
  intake.move(-80);
  travelProfile({
    Point{13.1331_in, -40.6061_in, 90.7668_deg},
    Point{12.1478_in, -17.892_in, 96.928_deg},
  }, false, 0.4);
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

  drive.setSide(isRed);

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
