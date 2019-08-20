#include "main.h"
#include "ports.hpp"

using namespace okapi;

class Lift {
  const QLength armLength = 20_in;
  // height of arm pivot above ground
  const QLength armElevation = 16_in;
  const int ticksPerRev = 1800;
  const QLength maxArmHeight = armElevation + armLength * 0.9;
  const QLength minArmHeight = 1_in;
  int liftTareTicks = 0;
  const QLength smallMoveSize = 0.25_in;
  const int numHeights = 4;
  const QLength targetHeights[4] = {1_in, 18.5_in, 24.5_in, 38.0_in};
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated(-LIFT_LEFT_PORT);
  ADIButton buttonLimit = ADIButton(BUTTON_LIMIT_PORT);

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

  QLength getHeight(double taredTicks) {
    double ticks = taredTicks + liftTareTicks;
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
  void checkTare() {
    if(buttonLimit.isPressed()) {
      tare();
    }
  }

  void tare() {
    tareHeight(1_in);
  }

  void tareHeight(QLength height) {
    // tare to 0
    controller.tarePosition();
    // assume lift is 1 inch off ground
    printf("Taring arm to height %f\n", height.getValue());
    liftTareTicks = getTicks(height);
  }

  void move(int heightIndex) {
    printf("Moving lift to height %d\n", heightIndex);
    QLength targetHeight = targetHeights[heightIndex];
    move(targetHeight);
  }

  void move(QLength height) {
    QLength clampedHeight = std::clamp(height, minArmHeight, maxArmHeight);
    printf("Clamped height from %f to %f\n", height.getValue(), clampedHeight.getValue());
    double targetTicks = getTicks(clampedHeight);
    double taredTicks = targetTicks - liftTareTicks;
    printf("New target ticks %f\n", taredTicks);
    controller.setTarget(taredTicks);
  }

  void move(bool isIncrease, bool isSmall) {
    printf("\n");
    printf("Button limit status: %d\n", buttonLimit.isPressed() ? 1 : 0);
    printf("Moving lift +%d\n", boolToSign(isIncrease) * (isSmall ? 1 : 20));
    const double lastTargetTicks = controller.getTarget();
    printf("Last target ticks %f\n", lastTargetTicks);
    QLength lastTargetHeight = getHeight(lastTargetTicks);
    printf("Last height %f\n", lastTargetHeight.getValue());
    QLength newHeight = getChangedHeight(lastTargetHeight, isIncrease, isSmall);
    move(newHeight);
  }

  void setMaxVelocity(double tps) {
    controller.setMaxVelocity(tps);
  }
};
