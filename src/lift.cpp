#include "main.h"

using namespace okapi;

class Lift {
  const QLength armLength = 20_in;
  // height of arm pivot above ground
  const QLength armElevation = 16_in;
  const int ticksPerRev = 900;
  const QLength maxArmHeight = armElevation + armLength * 0.9;
  const QLength minArmHeight = 1_in;
  int liftTareTicks = 0;
  const QLength smallMoveSize = 0.25_in;
  const int numHeights = 4;
  const QLength targetHeights[4] = {1_in, 18.5_in, 24.5_in, 38.0_in};
  const int port = 9;
  AsyncPosIntegratedController controller = AsyncControllerFactory::posIntegrated(port);

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
    if (isIncreaseSmall) {
      // just move a bit in one direction
      return lastHeight + boolToSign(isIncrease) * smallMoveSize;
    } else {
      // move to the nearest preset in that direction
      int i = 0;
      while (lastHeight > targetHeights[i]) {
        ++i;
      }
      if (isIncrease) {
        if (targetHeights[i] != lastHeight && i < numHeights - 1) {
          return targetHeights[i+1];
        }
      } else {
        if (i != 0) {
          return targetHeights[i-1];
        }
      }
      return lastHeight;
    }
  }

public:
  void tareHeight(QLength height) {
    // tare to 0
    controller.tarePosition();
    // assume lift is 1 inch off ground
    liftTareTicks = getTicks(height);
  }

  void move(int heightIndex) {
    printf("Moving lift to height %d\n", heightIndex);
    QLength targetHeight = targetHeights[heightIndex];
    move(targetHeight);
  }

  void move(QLength height) {
    QLength clampedHeight = std::clamp(height, minArmHeight, maxArmHeight);
    double targetTicks = getTicks(clampedHeight);
    controller.setTarget(targetTicks - liftTareTicks);
  }

  void move(bool isIncrease, bool isSmall) {
    const double lastTargetTicks = controller.getTarget();
    QLength lastTargetHeight = getHeight(lastTargetTicks);
    QLength newHeight = getChangedHeight(lastTargetHeight, isIncrease, isSmall);
    move(newHeight);
  }

  void setMaxVelocity(double tps) {
    controller.setMaxVelocity(tps);
  }
};
