#include "main.h"
#include "ports.hpp"

/**
 * Main class for the Rails angling subsystem
 * Wraps around AsyncPosIntegratedController
 **/
class Rails {
  bool isBack = true;
  const double RAILS_BACK_TICKS = 0;
  const double RAILS_FORWARD_TICKS = 750;
  const double MOVE_BACK_SPEED = 45;
  const double MOVE_FORWARD_SPEED = 30;
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated(-ANGLE_RAILS_PORT);

  void move(double ticks) {
    printf("Moving rails to %f ticks\n", ticks);
    controller.setTarget(ticks);
  }

  void move(double ticks, double maxSpeed) {
    setMaxVelocity(maxSpeed);
    move(ticks);
  }

  void setMaxVelocity(double ticks) {
    controller.setMaxVelocity(ticks);
  }

public:
  void moveBack() {
    printf("Moving rails to back\n");
    move(RAILS_BACK_TICKS, MOVE_BACK_SPEED);
  }

  void moveForward() {
    printf("Moving rails to front\n");
    move(RAILS_FORWARD_TICKS, MOVE_FORWARD_SPEED);
  }

  void move(bool _isBack) {
    isBack = _isBack;
    if (isBack) {
      moveBack();
    } else {
      moveForward();
    }
  }

  /**
   * Switch between forward and backward
   * Assume rails have not moved and remain being isBack
   */
  void togglePosition() {
    move(!isBack);
  }

  /**
   * Retare by assuming current position is at ticks 0
   */
  void tare() {
    controller.tarePosition();
  }

  void waitUntilSettled() {
    controller.waitUntilSettled();
  }
};
