#include "main.h"
#include "ports.hpp"

using namespace okapi;

class Rails {
  bool isBack;
  const double RAILS_BACK_TICKS = 0;
  const double RAILS_FORWARD_TICKS = 750;
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated(-ANGLE_RAILS_PORT);

  void move(double ticks) {
    printf("Moving rails to %f ticks\n", ticks);
    controller.setTarget(ticks);
  }

public:
  void moveBack() {
    setMaxVelocity(45);
    printf("Moving rails to back\n");
    move(RAILS_BACK_TICKS);
  }

  void moveForward() {
    setMaxVelocity(30);
    printf("Moving rails to front\n");
    move(RAILS_FORWARD_TICKS);
  }

  void move(bool _isBack) {
    isBack = _isBack;
    if (isBack) {
      moveBack();
    } else {
      moveForward();
    }
  }

  void togglePosition() {
    move(!isBack);
  }

  void setMaxVelocity(double ticks) {
    controller.setMaxVelocity(ticks);
  }

  void tarePosition() {
    controller.tarePosition();
  }

  void waitUntilSettled() {
    controller.waitUntilSettled();
  }
};
