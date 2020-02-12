#include "main.h"
#include "ports.hpp"
extern Timer cumTimer;

/**
 * Main class for the Intake subsystem
 * Thinly wraps around AsyncVelIntegratedController
 **/
class Intake {
  /* ---- CONFIG ---- */
  double SPEED = 600;
  AsyncVelIntegratedController controller =
    AsyncControllerFactory::velIntegrated({INTAKE_LEFT_PORT, -INTAKE_RIGHT_PORT});
  Motor leftMotor = Motor(INTAKE_LEFT_PORT);
  Motor rightMotor = Motor(-INTAKE_RIGHT_PORT);
  FILE* fout = fopen("/usd/log.txt", "w");

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

  void logState() {
    float current = (leftMotor.get_current_draw() + rightMotor.get_current_draw())/2;
    float velocity = (leftMotor.get_actual_velocity() + rightMotor.get_actual_velocity())/2;
    float t = (cumTimer.getDtFromStart()/1_s).getValue();
    if (fout!=NULL) {
      fprintf(fout, "IntakeState: %f %f %f\n", t, current, velocity);
      printf("Wrote\n");
    }
  }
};
