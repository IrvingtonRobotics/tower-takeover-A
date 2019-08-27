#ifndef _UTILS_
#define _UTILS_

#include "common.hpp"

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

#endif
