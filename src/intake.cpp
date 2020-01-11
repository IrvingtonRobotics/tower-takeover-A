#include "main.h"
#include "ports.hpp"

/**
 * Main class for the Intake subsystem
 * Thinly wraps around AsyncVelIntegratedController
 **/
class Intake {
  /* ---- CONFIG ---- */
  double SPEED = 200;
  AsyncVelIntegratedController controller =
    AsyncControllerFactory::velIntegrated({INTAKE_LEFT_PORT, -INTAKE_RIGHT_PORT});

public:
  void move(double speed) {
    controller.setTarget(speed);
  }

  void intake() {
    intake(1);
  }

  void intake(float scl) {
    move(SPEED * scl);
  }

  void outtake() {
    outtake(1);
  }

  void outtake(float scl) {
    move(-SPEED * scl);
  }

  void stop() {
    move(0);
  }
};
