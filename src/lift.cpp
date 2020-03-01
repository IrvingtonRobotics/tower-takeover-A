#include "main.h"
#include "ports.hpp"

extern bool doClearArm;
extern bool doUnclearArm;

/**
 * Main class for the Lift subsystem.
 * Wraps around both AsyncPosIntegratedController and AsyncVelIntegratedController
 * to provide movement to precise locations and smooth movement when lowering
 * to the button/limit switch
 *
 * TODO: Use raw pros::MOTOR to utilize separate velocity and position controls
 * Maybe try Gyro (do we have one?)
 *   https://www.robotc.net/wikiarchive/VEX2_Sensors_Overview
 * This'll denecessitate thorough calibration
 */
class Lift {
  /* ---- CONFIG ---- */
  const QLength ARM_LENGTH = 22_in;
  // height of arm pivot above ground
  const QLength ARM_ELEVATION = 16_in;
  // 720 ticks/rev with 36:1 gears -- high torque
  // 360 ticks/rev with 18:1 gears
  // 180 ticks/rev with 6:1 gears -- high speed
  const double GEAR_RATIO = 7;
  const int TICKS_PER_REV = 720 * GEAR_RATIO;
  // height the arm caps out at
  const QLength MAX_ARM_HEIGHT = 31_in;
  // height need to start clearing
  const QLength CLEAR_ARM_HEIGHT = 5_in;
  // height the arm starts at
  const QLength MIN_ARM_HEIGHT = 2.5_in;
  // tolerance of position when calculating new targets
  const QLength POS_TOLERANCE = 0.5_in;
  // size of opcontrol small movements
  const QLength SMALL_MOVE_SIZE = 0.5_in;
  // just leave this very small
  const QLength MIN_THRESHOLD = -1000_in;
  // tolerance when doing small movements: just reach within this of the threshold
  const QLength SMALL_MOVE_TOLERANCE = 0.2_in;
  // give up on lowering to button after this time if not hit limit switch
  const QTime LOWER_TO_BUTTON_TIMEOUT = 5_s;
  const int DEFAULT_LOWER_SPEED = 100;
  static const int NUM_HEIGHTS = 3;
  // WARNING: targetHeights MUST be sorted
  // {min, small tower, med tower}
  const QLength targetHeights[NUM_HEIGHTS] = {MIN_ARM_HEIGHT, 24.5_in, MAX_ARM_HEIGHT};
  const QLength MID_HEIGHT = (targetHeights[NUM_HEIGHTS-1] + targetHeights[0])/2;
  // ticks per second
  const int DEFAULT_MAX_VELOCITY = 500;

  /* ---- No need to edit ---- */
  double tareTicks = 0;
  // threshold until the target is reached for opcontrol small movements
  QLength smallMoveThreshold;
  // direction currently (or last) moved in for small movements
  int smallMoveDir = 1;
  const int PORT = -LIFT_PORT;
  // is this currently doing a hard stop?
  bool isLowering = false;
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated(PORT);
  AsyncVelIntegratedController velController =
    AsyncControllerFactory::velIntegrated(PORT);
  Timer timeoutTimer;

  /**
   * @param QLength height from ground
   * @returns ticks from center
   */
  double getTicks(QLength height) {
    QLength dy = height - ARM_ELEVATION;
    double ratio = (dy / ARM_LENGTH).getValue();
    // printf("Height sin: %f\n", ratio);
    // radians
    double angle = asin(ratio);
    return getTicks(angle);
  }

  /**
   * @param radians angle delta
   * @return ticks delta
   */
  double getTicks(double angle) {
    double revolutions = angle / PI / 2;
    return revolutions * TICKS_PER_REV;
  }

  /**
   * @param ticks from center
   * @returns radian angle from center
   */
  double getAngle(double taredTicks) {
    double ticks = taredTicks;
    double revolutions = ticks / TICKS_PER_REV;
    // radians
    double angle = revolutions * PI * 2;
    return angle;
  }

  /**
   * @param ticks from center
   * @returns QLength height from ground
   */
  QLength getHeight(double taredTicks) {
    double angle = getAngle(taredTicks);
    double ratio = sin(angle);
    QLength dy = ratio * ARM_LENGTH;
    QLength height = dy + ARM_ELEVATION;
    return height;
  }

  /**
   * To help opcontrol
   * If isIncreaseSmall, adds a small bit in the direction isIncrease
   * Else returns next preset in the direction isIncrease at least
   *  POS_TOLERANCE away
   */
  QLength getChangedHeight(QLength lastHeight, bool isIncrease, bool isIncreaseSmall) {
    // printf("Last height %f\n", lastHeight.getValue());
    int m = boolToSign(isIncrease);
    if (isIncreaseSmall) {
      // just move a bit in one direction
      smallMoveDir = m;
      QLength smallMoveTarget = lastHeight + m * SMALL_MOVE_SIZE;
      smallMoveThreshold = smallMoveTarget * m - SMALL_MOVE_TOLERANCE;
      return smallMoveTarget;
    } else {
      /*
        increasing: find first one greater than current position
        decreasing: find last one less than current position
        switching isIncrease just switches direction and comparison type
      */
      int i = isIncrease ? 0 : NUM_HEIGHTS - 1;
      while (lastHeight * m > targetHeights[i] * m - POS_TOLERANCE) {
        // printf("i %d\n", i);
        i += m;
      }
      i = std::clamp(i, 0, NUM_HEIGHTS - 1);
      return targetHeights[i];
    }
  }

  /**
   * Switch control between position controller and velocity controller
   */
  void flipDisable() {
    controller.flipDisable();
    velController.flipDisable();
  }

public:
  Lift() {
    // turn off velController so it doesn't conflict with posController
    velController.flipDisable();
    resetMaxVelocity();
  }

  void tare() {
    tareHeight(MIN_ARM_HEIGHT);
  }

  void moveToggle() {
    if (getCurrentHeight() > MID_HEIGHT) {
      move(0);
    } else {
      move(-1);
    }
  }

  /**
   * Retare by assuming the current position is at height height
   */
  void tareHeight(QLength height) {
    // calculates tareTicks as ticks from center
    // tare to 0
    controller.tarePosition();
    // printf("Think ticks: %f\n", controller.getTarget());
    // assume lift is height off ground
    // printf("Taring arm to height %f\n", height.getValue());
    // printf("Think ticks: %f\n", controller.getTarget());
    tareTicks = getTicks(height);
    // printf("Height ticks: %f\n", getTicks(height));
    // printf("Think height:%f\n", getHeight(getCurrentTicks()));
    // printf("Tare ticks: %f\n", tareTicks);
  }

  void move(int heightIndex) {
    // printf("Moving lift to height %d\n", heightIndex);
    if (heightIndex < 0) {
      heightIndex = NUM_HEIGHTS + heightIndex;
    }
    QLength targetHeight = targetHeights[heightIndex];
    move(targetHeight);
  }

  void glide(bool isUp) {
    move(isUp ? -1 : 0);
  }

  /**
   * @param ticks from center
   */
  void move(double ticks) {
    double taredTicks = ticks - tareTicks;
    if (getHeight(ticks) > CLEAR_ARM_HEIGHT) {
      doClearArm = true;
    } else {
      doUnclearArm = true;
    }
    controller.setTarget(taredTicks);
  }

  /**
   * Moves, but height is limited between MIN_ARM_HEIGHT and MAX_ARM_HEIGHT
   */
  void move(QLength height) {
    // printf("tare ticks %f\n", tareTicks);
    QLength clampedHeight = std::clamp(height, MIN_ARM_HEIGHT, MAX_ARM_HEIGHT);
    // printf("Clamped height from %f to %f\n", height.getValue(), clampedHeight.getValue());
    double targetTicks = getTicks(clampedHeight);
    // printf("New target ticks %f\n", taredTicks);
    move(targetTicks);
  }

  /**
   * Opcontrol calls this based on button inputs
   *
   * @param isIncrease whether or not the height increases
   * @param isSmall whether to move in small increment or to next preset
   */

  void move(bool isIncrease, bool isSmall) {
    // printf("\n");
    // printf("Moving lift +%d\n", boolToSign(isIncrease) * (isSmall ? 1 : 20));
    // printf("Last height %f\n", lastTargetHeight.getValue());
    if (!isSmall) {
      smallMoveThreshold = MIN_THRESHOLD;
    }
    QLength height = getCurrentHeight();
    if (height * smallMoveDir >= smallMoveThreshold) {
      QLength newHeight = getChangedHeight(getCurrentHeight (), isIncrease, isSmall);
      move(newHeight);
    }
  }

  /**
   * @returns ticks from center
   */
  float getCurrentTicks() {
    // return controller.getTarget() + tareTicks;
    return getTargetTicks() - controller.getError();
  }

  /**
   * @returns height from ground
   */
  QLength getCurrentHeight() {
    // height off ground
    return getHeight(getCurrentTicks());
  }

  /**
   * @returns ticks from center
   */
  float getTargetTicks() {
    return controller.getTarget() + tareTicks;
  }

  /**
   * @returns height from ground
   */
  QLength getTargetHeight() {
    return getHeight(getTargetTicks());
  }


  void resetMaxVelocity() {
    setMaxVelocity(DEFAULT_MAX_VELOCITY);
  }

  void setMaxVelocity(double tps) {
    controller.setMaxVelocity(tps);
  }

  void step() {
    if (isLowering) {
      if (timeoutTimer.getDtFromStart() >= LOWER_TO_BUTTON_TIMEOUT) {
        stopLowering();
      }
    }
  }

  void stopLowering() {
    // hand control back to pos
    flipDisable();
    isLowering = false;
    velController.setTarget(0);
    tare();
    // avoid the controller resuming to its previous location
    controller.setTarget(0);
  }

  void startLowering(int speed) {
    // move control to vel for smooth movement
    flipDisable();
    isLowering = true;
    timeoutTimer = Timer();
    velController.setTarget(-abs(speed));
  }

  void lowerToButton(int speed) {
    if (isLowering) {
      stopLowering();
    } else {
      startLowering(speed);
    }
  }

  void lowerToButton() {
    lowerToButton(DEFAULT_LOWER_SPEED);
  }

  /**
   * Get a quick data readout of where the lift thinks it is
   * Useful for calibration and debugging
   */
  void query() {
    printf("---------\n");
    printf("  query  \n");
    printf("---------\n");
    double ticks = getCurrentTicks();
    printf("ticks %f\n", ticks);
    QLength height = getHeight(ticks);
    printf("Height: %f inches\n", height.getValue() * 1 / 0.0254);
    double angle = getAngle(ticks);
    printf("Height angle: %f degrees\n", angle * 180 / 3.14);
    printf("---------\n");
  }

  /**
   * [Deprecated]
   * Move the lift 90 degrees up from its current position without regard
   * for apparent height limits
   * Used in debugging (Please don't)
   */
  void lift90() {
    printf("---------\n");
    printf("  lift90  \n");
    printf("---------\n");
    double currentTicks = getCurrentTicks();
    printf("current %f\n", currentTicks);
    double nextTicks = currentTicks + getTicks(PI / 2);
    printf("next %f\n", nextTicks);
    controller.setTarget(nextTicks);
    printf("---------\n");
  }

  void waitUntilSettled() {
    controller.waitUntilSettled();
  }

  void stop() {
    if (isLowering) {
      stopLowering();
    }
    move(getCurrentTicks());
  }
};
