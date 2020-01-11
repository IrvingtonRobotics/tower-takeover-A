#include "common.hpp"

void foldout() {
  printf("fold out\n");
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
  drive.moveDistance(2_in);
  drive.moveDistance(-2_in);
  lift.resetMaxVelocity();
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
