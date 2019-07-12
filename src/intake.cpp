#include "main.h"

using namespace okapi;

class Intake {
  AsyncVelIntegratedController controller = AsyncControllerFactory::velIntegrated({3, -8});

public:
  void move(double speed) {
    controller.setTarget(speed);
  }
};
