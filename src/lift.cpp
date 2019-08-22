#include "main.h"
#include "ports.hpp"

using namespace okapi;

class Lift {
  const QLength armLength = 22_in;
  // height of arm pivot above ground
  const QLength armElevation = 16_in;
  // 1800 ticks/rev with 36:1 gears -- high torque
  // 900 ticks/rev with 18:1 gears
  // 300 ticks/rev with 6:1 gears -- high speed
  const double arbitraryFactor = 1.16;
  const int ticksPerRev = 1800 * arbitraryFactor;
  const QLength maxArmHeight = armElevation + armLength * 0.7;
  const QLength minArmHeight = 2.5_in;
  double tareTicks = 0;
  const QLength smallMoveSize = 1_in;
  static const int numHeights = 4;
  // targetHeights MUST be sorted
  const QLength targetHeights[numHeights] = {minArmHeight, 16_in, 24.5_in, 38.0_in};
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated(-LIFT_PORT);
  AsyncVelIntegratedController velController =
    AsyncControllerFactory::velIntegrated(-LIFT_PORT);
  ADIButton buttonLimit = ADIButton(BUTTON_LIMIT_PORT);

  double getTicks(QLength height) {
    // takes height from ground
    // gives ticks from center
    QLength dy = height - armElevation;
    double ratio = (dy / armLength).getValue();
    // printf("Height sin: %f\n", ratio);
    // radians
    double angle = asin(ratio);
    return getTicks(angle);
  }

  double getTicks(double angle) {
    double revolutions = angle / PI / 2;
    return revolutions * ticksPerRev;
  }

  double getAngle(double taredTicks) {
    // takes ticks from center
    // gives angle from center
    double ticks = taredTicks;
    double revolutions = ticks / ticksPerRev;
    // radians
    double angle = revolutions * PI * 2;
    return angle;
  }

  QLength getHeight(double taredTicks) {
    // takes ticks from center
    // gives height from ground
    double angle = getAngle(taredTicks);
    double ratio = sin(angle);
    QLength dy = ratio * armLength;
    QLength height = dy + armElevation;
    return height;
  }

  QLength getChangedHeight(QLength lastHeight, bool isIncrease, bool isIncreaseSmall) {
    // printf("Last height %f\n", lastHeight.getValue());
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
        // printf("i %d\n", i);
        i += m;
      }
      i = std::clamp(i + m, 0, numHeights - 1);
      // printf("clamped i %d\n", i);
      // printf("returning ... %f\n", targetHeights[i]);
      return targetHeights[i];
    }
  }

public:
  Lift() {
    velController.flipDisable();
  }

  void checkTare() {
    if(buttonLimit.isPressed()) {
      tare();
    }
  }

  void tare() {
    tareHeight(minArmHeight);
  }

  void tareHeight(QLength height) {
    // calculates tareTicks as ticks from center
    // tare to 0
    controller.tarePosition();
    printf("Think ticks: %f\n", controller.getTarget());
    // assume lift is height off ground
    printf("Taring arm to height %f\n", height.getValue());
    // printf("Think ticks: %f\n", controller.getTarget());
    tareTicks = getTicks(height);
    printf("Height ticks: %f\n", getTicks(height));
    printf("Think height:%f\n", getHeight(getCurrentTicks()));
    printf("Tare ticks: %f\n", tareTicks);
  }

  void move(int heightIndex) {
    // printf("Moving lift to height %d\n", heightIndex);
    QLength targetHeight = targetHeights[heightIndex];
    move(targetHeight);
  }

  void move(double ticks) {
    // takes ticks from center
    double taredTicks = ticks - tareTicks;
    controller.setTarget(taredTicks);
  }

  void move(QLength height) {
    // printf("tare ticks %f\n", tareTicks);
    QLength clampedHeight = std::clamp(height, minArmHeight, maxArmHeight);
    // printf("Clamped height from %f to %f\n", height.getValue(), clampedHeight.getValue());
    double targetTicks = getTicks(clampedHeight);
    // printf("New target ticks %f\n", taredTicks);
    move(targetTicks);
  }

  void move(bool isIncrease, bool isSmall) {
    // printf("\n");
    // printf("Button limit status: %d\n", buttonLimit.isPressed() ? 1 : 0);
    // printf("Moving lift +%d\n", boolToSign(isIncrease) * (isSmall ? 1 : 20));
    const double lastTargetTicks = controller.getTarget();
    // printf("Last target ticks %f\n", lastTargetTicks);
    QLength lastTargetHeight = getHeight(lastTargetTicks);
    // printf("Last height %f\n", lastTargetHeight.getValue());
    QLength newHeight = getChangedHeight(lastTargetHeight, isIncrease, isSmall);
    move(newHeight);
  }

  float getCurrentTicks() {
    // returns ticks from center
    // return controller.getTarget() + tareTicks;
    return getTargetTicks() - controller.getError();
  }

  QLength getCurrentHeight() {
    // height off ground
    return getHeight(getCurrentTicks());
  }

  float getTargetTicks() {
    // returns target ticks from center
    return controller.getTarget() + tareTicks;
  }

  QLength getTargetHeight() {
    // returns target height from center
    return getHeight(getTargetTicks());
  }

  void setMaxVelocity(double tps) {
    controller.setMaxVelocity(tps);
  }

  void lowerToButton() {
    controller.flipDisable();
    velController.flipDisable();
    velController.setTarget(-30);
    while(!buttonLimit.isPressed()) {
      pros::delay(10);
    }
    velController.setTarget(0);
    velController.flipDisable();
    controller.flipDisable();
    tare();
    controller.setTarget(0);
  }

  void query() {
    double ticks = getCurrentTicks();
    printf("ticks %f\n", ticks);
    QLength height = getHeight(ticks);
    printf("Height: %f inches\n", height.getValue() * 1 / 0.0254);
    double angle = getAngle(ticks);
    printf("Height angle: %f degrees\n", angle * 180 / 3.14);
  }

  void lift90() {
    double currentTicks = getCurrentTicks();
    printf("current %f\n", currentTicks);
    double nextTicks = currentTicks + getTicks(PI / 2);
    printf("next %f\n", nextTicks);
    controller.setTarget(nextTicks);
  }
};
