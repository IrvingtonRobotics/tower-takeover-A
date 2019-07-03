#include "common.hpp"

using namespace okapi;

extern const QLength armLength = 20_in;
// height of arm above ground
extern const QLength armElevation = 16_in;
extern const int numHeights = 3;
extern const QLength targetHeights[] = {0_in, 18.5_in, 24.5_in, 38.0_in};
extern const int ticksPerRev = 900;
extern const QLength maxArmHeight = armElevation + armLength * 0.9;
int liftTareTicks = 0;

int getLiftTicks(QLength height) {
  QLength dy = height - armElevation;
  if (height > maxArmHeight) {
    printf(
      "Height %f is too high --> rounding down to %f\n",
      height, maxArmHeight
    );
    dy = maxArmHeight;
  }
  double ratio = (dy / armLength).getValue();
  printf("Height sin: %f\n", ratio);
  // radians
  double angle = asin(ratio);
  printf("Height angle: %f\n", angle);
  double revolutions = angle / PI / 2;
  return revolutions * ticksPerRev;
}

void liftTareHeight(QLength height) {
  // tare to 0
  lift.tarePosition();
  // assume lift is 1 inch off ground
  liftTareTicks = getLiftTicks(height);
}

void moveLift(int heightIndex) {
  printf("Moving lift to height %d\n", heightIndex);
  QLength targetHeight = targetHeights[heightIndex];
  moveLift(targetHeight);
}

void moveLift(QLength height) {
  int targetTicks = getLiftTicks(height);
  lift.setTarget(targetTicks - liftTareTicks);
}
