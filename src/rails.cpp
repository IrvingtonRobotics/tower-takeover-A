#include "main.h"
#include "ports.hpp"

/**
 * Main class for the Rails angling subsystem
 * Wraps around AsyncPosIntegratedController and AsyncVelIntegratedController
 * in the manner of Lift (some of the code is transferred)
 * Need a limit because the rails can now start in the middle for space concerns
 **/
class Rails {
  const double RAILS_BACK_TICKS = 0;
  const double RAILS_FORWARD_TICKS = 1050*7/5;
  const double RAILS_MIDPOINT_TICKS = (RAILS_BACK_TICKS + RAILS_FORWARD_TICKS) / 2;
  const double MOVE_BACK_SPEED = 45;
  const double MOVE_FORWARD_SPEED = 30;
  const QTime BACK_TO_BUTTON_TIMEOUT = 5_s;
  const int DEFAULT_BACK_SPEED = 20;
  const int PORT = ANGLE_RAILS_PORT;
  bool stopping = false;
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated(PORT);
  AsyncVelIntegratedController velController =
    AsyncControllerFactory::velIntegrated(PORT);
  ADIButton buttonLimit = ADIButton(RAILS_LIMIT_PORT);


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

  /**
   * Switch control between position controller and velocity controller
   */
  void flipDisable() {
    controller.flipDisable();
    velController.flipDisable();
  }

  bool isBack() {
    if (getTargetTicks() < RAILS_MIDPOINT_TICKS) {
      return true;
    } else {
      return false;
    }
  }

public:
  Rails() {
    velController.flipDisable();
    setMaxVelocity(30);
  }

  float getTargetTicks() {
    return controller.getTarget();
  }

  void moveBack() {
    moveBack(MOVE_BACK_SPEED);
  }

  void moveBack(double maxSpeed) {
    move(RAILS_BACK_TICKS, maxSpeed);
  }

  void moveForward() {
    printf("Moving rails to front\n");
    move(RAILS_FORWARD_TICKS, MOVE_FORWARD_SPEED);
  }

  void moveForward(double weight, double maxSpeed) {
    double target = weight * RAILS_FORWARD_TICKS + (1 - weight) * RAILS_BACK_TICKS;
    move(target, maxSpeed);
  }

  void move(bool _isBack) {
    if (_isBack) {
      moveBack();
    } else {
      moveForward();
    }
  }

  /**
   * BLOCKING
   */
  void backToButton(int speed) {
    // move control to vel for smooth movement
    flipDisable();
    velController.setTarget(-abs(speed));
    Timer timeoutTimer = Timer();
    // delay whole code
    while(!buttonLimit.isPressed() && timeoutTimer.getDtFromStart() < BACK_TO_BUTTON_TIMEOUT) {
      if (stopping) {
        stopping = false;
        return;
      }
      pros::delay(10);
    }
    velController.setTarget(0);
    // hand control back to pos
    flipDisable();
    tare();
    // avoid the controller resuming to its previous location
    controller.setTarget(0);
  }

  void backToButton() {
    backToButton(DEFAULT_BACK_SPEED);
  }

  /**
   * Switch between forward and backward
   * Assume rails have not moved and remain being isBackdrive
   */
  void togglePosition() {
    move(!isBack());
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

  float getCurrentTicks() {
    // return controller.getTarget() + tareTicks;
    return getTargetTicks() - controller.getError();
  }

  void stop() {
    move(getCurrentTicks());
    stopping = true;
  }
};
