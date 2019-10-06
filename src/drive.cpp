#include "main.h"
#include "ports.hpp"

#define controller (*controllerPtr)

/**
 * Main class for the wheeled driving subsystem
 * Wraps around a ChassisControllerIntegrated with limited acceleration
 */
class Drive {
  float leftDriveSpeed = 0;
  float rightDriveSpeed = 0;
  const float MAX_DRIVE_ACCEL = 0.01;
  const float MAX_DRIVE_DECEL = 0.05;
  const float TURN_LIMIT_THRESHOLD = 1.20;
  const float TURN_LIMIT_SCALE = 0.85;
  ChassisControllerIntegrated controllerStraight = getController(false);
  ChassisControllerIntegrated controllerReversed = getController(true);
  ChassisControllerIntegrated* controllerPtr = &controllerStraight;

  ChassisControllerIntegrated controllerBack = ChassisControllerFactory::create(
    DRIVE_LEFT_BACK_PORT,
    -DRIVE_RIGHT_BACK_PORT,
    AbstractMotor::gearset::green,
    {4_in, 14.5_in}
  );

  ChassisControllerIntegrated getController(bool isReversed) {
    if (isReversed) {
      return ChassisControllerFactory::create(
        {-DRIVE_RIGHT_FRONT_PORT, -DRIVE_RIGHT_BACK_PORT},
        {DRIVE_LEFT_FRONT_PORT, DRIVE_LEFT_BACK_PORT},
        AbstractMotor::gearset::green,
        {4_in, 14.5_in}
      );
    } else {
      return ChassisControllerFactory::create(
        {DRIVE_LEFT_FRONT_PORT, DRIVE_LEFT_BACK_PORT},
        {-DRIVE_RIGHT_FRONT_PORT, -DRIVE_RIGHT_BACK_PORT},
        AbstractMotor::gearset::green,
        {4_in, 14.5_in}
      );
    }
  }

  /**
   * Get new drive speed after one tick for one side of the robot,
   * taking into account acceleration limits
   */
  float getNewDriveSpeed(float current, float input) {
    // tried using clamp, but difficulties
    float lo;
    float hi;
    if (current >= 0) {
      lo = current - MAX_DRIVE_DECEL;
      hi = current + MAX_DRIVE_ACCEL;
    } else {
      lo = current - MAX_DRIVE_ACCEL;
      hi = current + MAX_DRIVE_DECEL;
    }
    if (lo > input) return lo;
    if (hi < input) return hi;
    return input;
  }

  void moveTank(float left, float right) {
    moveTank(left, right, false);
  }

  void moveTank(float left, float right, bool stopFront) {
    // controller.flipDisable(false);
    // controllerBack.flipDisable(true);
    if (!stopFront) {
      controllerBack.tank(0, 0);
      controller.tank(leftDriveSpeed, rightDriveSpeed);
    } else {
      controller.tank(0, 0);
      controllerBack.tank(leftDriveSpeed, rightDriveSpeed);
    }
  }

public:
  // Drive() {
  //   controllerBack.flipDisable(true);
  // }

  /**
   * Arcade drive based on controllerX and controllerY
   * Pass through controller.tank to use existing acceleration limit code
   */
  float move(float controllerX, float controllerY, float scl, bool stopFront) {
    // compute tank left and right based on arcade x and y
    float left = controllerY + controllerX;
    float right = controllerY - controllerX;
    left *= scl;
    right *= scl;
    leftDriveSpeed = getNewDriveSpeed(leftDriveSpeed, left);
    rightDriveSpeed = getNewDriveSpeed(rightDriveSpeed, right);
    float turnRate = abs(leftDriveSpeed - rightDriveSpeed);
    if (turnRate > TURN_LIMIT_THRESHOLD) {
      leftDriveSpeed *= TURN_LIMIT_SCALE;
      rightDriveSpeed *= TURN_LIMIT_SCALE;
    }
    moveTank(leftDriveSpeed, rightDriveSpeed, stopFront);
  }

  /**
   * Synchronous
   *
   * Move a specific distance.
   * No acceleration limits are applied, so this might cause chain to slip
   */
  void moveDistance(QLength length) {
    // might click
    controller.moveDistance(length);
  }

  void stop() {
    moveTank(0, 0);
  }

  /**
   * Turn clockwise some angle, use negative to turn counterclockwise
   */
  void turnAngle(QAngle angle) {
    controller.turnAngle(angle);
  }

  /**
   * Set side to reverse angles
   */
  void setSide(bool isRed) {
    if (isRed) {
      controllerPtr = &controllerStraight;
    } else {
      controllerPtr = &controllerReversed;
    }
  }

  /**
   * Reset to blue
   */
   void straighten() {
     setSide(false);
   }

   auto getProfileController(float speed) {
     return AsyncControllerFactory::motionProfile(
       1.5 * speed,
       4.0 * speed,
       10.0 * speed,
       controller
     );
   }
};

#undef controller
