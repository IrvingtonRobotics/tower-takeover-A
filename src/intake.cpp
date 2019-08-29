#include "main.h"
#include "ports.hpp"

/**
 * Main class for the Intake subsystem
 * Thinly wraps around AsyncVelIntegratedController
 **/
class Intake {
  double SPEED = 100;
  AsyncVelIntegratedController controller =
    AsyncControllerFactory::velIntegrated({-INTAKE_LEFT_PORT, INTAKE_RIGHT_PORT});

public:
  void move(double speed) {
    controller.setTarget(speed);
  }

  void intake() {
    move(SPEED);
  }

  void outtake() {
    move(-SPEED);
  }

  void stop() {
    move(0);
  }
};
