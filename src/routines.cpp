#include "common.hpp"

Timer autonTimer;

void foldout() {
  printf("FOLD OUT START %f\n", autonTimer.getDtFromStart().getValue());
  // home rails while lifting
  rails.backToButton();
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
  drive.moveDistance(-2_in);
  printf("DRIVE BACKWARD\n");
  lift.resetMaxVelocity();
  printf("FINISHED FOLDOUT\n");
}

void foldin() {
  printf("fold in\n");
  // lift.lowerToButton(120);
  lift.move(12_in);
  lift.waitUntilSettled();
  printf("settled\n");
  rails.backToButton();
  intake.intake();
  pros::delay(3000);
  intake.stop();
  lift.lowerToButton(160);
}
