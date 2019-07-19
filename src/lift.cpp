#include "main.h"
#include "ports.hpp"

using namespace okapi;

class Lift {
  const QLength armLength = 20_in;
  // height of arm pivot above ground
  const QLength armElevation = 16_in;
  const int ticksPerRev = 900;
  const QLength maxArmHeight = armElevation + armLength * 0.9;
  const QLength minArmHeight = 1_in;
  int tareTicks = 0;
  const QLength smallMoveSize = 1_in;
  static const int numHeights = 4;
  // targetHeights MUST be sorted
  const QLength targetHeights[numHeights] = {1_in, 18.5_in, 24.5_in, 38.0_in};
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated({-LIFT_LEFT_PORT, LIFT_RIGHT_PORT});

  double getTicks(QLength height) {
    QLength dy = height - armElevation;
    double ratio = (dy / armLength).getValue();
    printf("Height sin: %f\n", ratio);
    // radians
    double angle = asin(ratio);
    printf("Height angle: %f\n", angle);
    double revolutions = angle / PI / 2;
    return revolutions * ticksPerRev;
  }

  QLength getHeight(double ticks) {
    double revolutions = ticks / ticksPerRev;
    // radians
    double angle = revolutions * PI * 2;
    double ratio = sin(angle);
    QLength dy = ratio * armLength;
    QLength height = dy + armElevation;
    return height;
  }

  QLength getChangedHeight(QLength lastHeight, bool isIncrease, bool isIncreaseSmall) {
    printf("Last height %f\n", lastHeight.getValue());
    int m = boolToSign(isIncrease);
    if (isIncreaseSmall) {
      // just move a bit in one direction
      return lastHeight + m * smallMoveSize;
    } else {
      /*
        increasing: find first one greater than current position
        decreasing: find last one less than current position
        switching isIncrease just switches direction and comparison type
      */
      int i = isIncrease ? 0 : numHeights - 1;
      while (lastHeight * m >= targetHeights[i] * m) {
        printf("i %d\n", i);
        i += m;
      }
      i = std::clamp(i + m, 0, numHeights - 1);
      printf("clamped i %d\n", i);
      printf("returning ... %f\n", targetHeights[i]);
      return targetHeights[i];
    }
  }

public:
  void tareHeight(QLength height) {
    // tare to 0
    controller.tarePosition();
    // assume lift is 1 inch off ground
    tareTicks = 0;
    tareTicks = getTicks(height);
  }

  void move(int heightIndex) {
    printf("Moving lift to height %d\n", heightIndex);
    QLength targetHeight = targetHeights[heightIndex];
    move(targetHeight);
  }

  void move(QLength height) {
    printf("tare ticks %f\n", tareTicks);
    QLength clampedHeight = std::clamp(height, minArmHeight, maxArmHeight);
    double targetTicks = getTicks(clampedHeight);
    printf("Setting height %f which corresponds to ticks %f which corresponds to height %f\n",
      clampedHeight, targetTicks, getHeight(targetTicks)
    );
    controller.setTarget(targetTicks - tareTicks);
  }

  void move(bool isIncrease, bool isSmall) {
    QLength lastTargetHeight = getTargetHeight();
    printf("Moving lift ...\n");
    QLength newHeight = getChangedHeight(lastTargetHeight, isIncrease, isSmall);
    printf("New height: %f\n", newHeight.getValue());
    move(newHeight);
  }

  float getTargetTicks() {
    return controller.getTarget() + tareTicks;
  }

  QLength getTargetHeight() {
    return getHeight(getTargetTicks());
  }

  void setMaxVelocity(double tps) {
    controller.setMaxVelocity(tps);
  }
};
