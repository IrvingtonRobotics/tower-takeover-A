#include "main.h"

using namespace okapi;

class Rails {
  bool isBack;
  const double railsBackTicks = 0;
  const double railsForwardTicks = 250;
  const int port = 2;
  AsyncPosIntegratedController controller = AsyncControllerFactory::posIntegrated(port);

public:
  void move(double ticks) {
    controller.setTarget(ticks);
  }

  void move(bool isBack) {
    isBack = isBack;
    if (isBack) {
      move(railsBackTicks);
    } else {
      move(railsForwardTicks);
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
};
