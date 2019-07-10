#include "common.hpp"

using namespace okapi;

extern const QLength armLength = 20_in;
// height of arm above ground
extern const QLength armElevation = 16_in;
extern const int numHeights = 3;
extern const QLength targetHeights[] = {1_in, 18.5_in, 24.5_in, 38.0_in};
extern const int ticksPerRev = 900;
extern const QLength maxArmHeight = armElevation + armLength * 0.9;
extern const QLength minArmHeight = 1_in;
int liftTareTicks = 0;

double getLiftTicks(QLength height) {
  QLength dy = height - armElevation;
  double ratio = (dy / armLength).getValue();
  printf("Height sin: %f\n", ratio);
  // radians
  double angle = asin(ratio);
  printf("Height angle: %f\n", angle);
  double revolutions = angle / PI / 2;
  return revolutions * ticksPerRev;
}

QLength getLiftHeight(double ticks) {
  double revolutions = ticks / ticksPerRev;
  // radians
  double angle = revolutions * PI * 2;
  double ratio = sin(angle);
  QLength dy = ratio * armLength;
  QLength height = dy + armElevation;
  return height;
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
  QLength clampedHeight = std::clamp(height, minArmHeight, maxArmHeight);
  double targetTicks = getLiftTicks(clampedHeight);
  lift.setTarget(targetTicks - liftTareTicks);
}

bool isRailsBack;
const double railsBackTicks = 0;
const double railsForwardTicks = 250;

void moveRails(double ticks) {
  rails.setTarget(ticks);
}

void moveRails(bool isBack) {
  isRailsBack = isBack;
  if (isRailsBack) {
    moveRails(railsBackTicks);
  } else {
    moveRails(railsForwardTicks);
  }
}

void toggleRails() {
  moveRails(!isRailsBack);
}
