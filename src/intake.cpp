#include "main.h"
#include "ports.hpp"

using namespace okapi;

class Intake {
  AsyncVelIntegratedController controller =
    AsyncControllerFactory::velIntegrated({INTAKE_LEFT_PORT, -INTAKE_RIGHT_PORT});

public:
  void move(double speed) {
    controller.setTarget(speed);
  }
};
