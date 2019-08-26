#include "main.h"
#include "ports.hpp"

using namespace okapi;

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

public:
  float move(float x, float y) {
    // don't just use controller.arcade(y, x) in order to take use of existing accel
    float left = y + x;
    float right = y - x;
    leftDriveSpeed = getNewDriveSpeed(leftDriveSpeed, left);
    rightDriveSpeed = getNewDriveSpeed(rightDriveSpeed, right);
    controller.tank(leftDriveSpeed, rightDriveSpeed);
  }

  void moveDistance(QLength length) {
    // might click
    controller.moveDistance(length);
  }
};