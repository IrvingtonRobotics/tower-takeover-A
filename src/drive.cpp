#include "main.h"
#include "ports.hpp"

/**
 * Main class for the wheeled driving subsystem
 * Wraps around a ChassisControllerIntegrated with limited acceleration
 */
class Drive {
  float leftDriveSpeed = 0;
  float rightDriveSpeed = 0;
  const float MAX_DRIVE_ACCEL = 0.01;
  const float MAX_DRIVE_DECEL = 0.05;
  ChassisControllerIntegrated controller = ChassisControllerFactory::create(
    {DRIVE_LEFT_FRONT_PORT, DRIVE_LEFT_BACK_PORT},
    {-DRIVE_RIGHT_FRONT_PORT, -DRIVE_RIGHT_BACK_PORT},
    AbstractMotor::gearset::green,
    {4_in, 11.5_in}
  );

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
    controller.tank(leftDriveSpeed, rightDriveSpeed);
  }

public:
  /**
   * Arcade drive based on controllerX and controllerY
   * Pass through controller.tank to use existing acceleration limit code
   */
  float move(float controllerX, float controllerY) {
    // compute tank left and right based on arcade x and y
    float left = controllerY + controllerX;
    float right = controllerY - controllerX;
    leftDriveSpeed = getNewDriveSpeed(leftDriveSpeed, left);
    rightDriveSpeed = getNewDriveSpeed(rightDriveSpeed, right);
    moveTank(leftDriveSpeed, rightDriveSpeed);
  }

  /**
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
};
