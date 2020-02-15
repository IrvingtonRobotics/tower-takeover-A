#include "main.h"
#include "ports.hpp"

/**
 * Main class for the Rails angling subsystem
 * Wraps around AsyncPosIntegratedController and AsyncVelIntegratedController
 * in the manner of Lift (some of the code is transferred -- sad)
 * Need a limit because the rails can now start in the middle for space concerns
 **/
class Rails {
  /* ---- CONFIG ---- */
  // ticks all the way back
  const double RAILS_BACK_TICKS = 0;
  // ticks to rest in middle (moveMid)
  const double RAILS_MID_TICKS = 750;
  // ticks all the way forward
  const double RAILS_FORWARD_TICKS = 2500;
  const double MOVE_BACK_SPEED = 320;
  const double MOVE_FORWARD_SPEED = 130;
  const double MOVE_MID_SPEED = 160;
  const QTime BACK_TO_BUTTON_TIMEOUT = 8_s;
  const int DEFAULT_BACK_SPEED = 220;

  /* ---- No need to edit ---- */
  // ticks half way between back and forward
  const double RAILS_MIDPOINT_TICKS = (RAILS_BACK_TICKS + RAILS_FORWARD_TICKS) / 2;
  // ticks half way between back and mid
  const double RAILS_MID_MIDPOINT_TICKS = (RAILS_BACK_TICKS + RAILS_MID_TICKS) / 2;
  const int PORT = -ANGLE_RAILS_PORT;
  bool isBacking = false;
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated(PORT);
  AsyncVelIntegratedController velController =
    AsyncControllerFactory::velIntegrated(PORT);
  ADIButton buttonLimit = ADIButton(RAILS_LIMIT_PORT);
  Timer timeoutTimer;

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
    return getTargetTicks() < RAILS_MIDPOINT_TICKS;
  }

  bool isBackMid() {
    return getTargetTicks() < RAILS_MID_MIDPOINT_TICKS;
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

  void moveMid() {
    moveMid(MOVE_MID_SPEED);
  }

  void moveMid(double maxSpeed) {
    move(RAILS_MID_TICKS, maxSpeed);
  }

  void moveForward() {
    moveForward(MOVE_FORWARD_SPEED);
  }

  void moveForward(double maxSpeed) {
    moveForward(1, maxSpeed);
  }

  void moveForward(double weight, double maxSpeed) {
    if (weight < 0) {
      return;
    }
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

  void moveMid(bool _isBack) {
    if (_isBack) {
      moveBack();
    } else {
      moveMid();
    }
  }

  void step() {
    if (isBacking) {
      if (buttonLimit.isPressed() || timeoutTimer.getDtFromStart() >= BACK_TO_BUTTON_TIMEOUT) {
        stopBacking();
      }
    }
  }

  void stopBacking() {
    // hand control back to pos
    flipDisable();
    isBacking = false;
    velController.setTarget(0);
    tare();
    // avoid the controller resuming to its previous location
    controller.setTarget(0);
  }

  void startBacking(int speed) {
    // move control to vel for smooth movement
    flipDisable();
    isBacking = true;
    timeoutTimer = Timer();
    velController.setTarget(-abs(speed));
  }

  void backToButton(int speed) {
    if (isBacking) {
      stopBacking();
    } else {
      startBacking(speed);
    }
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

  void togglePositionMid() {
    moveMid(!isBackMid());
  }

  /**
   * Retare by assuming current position is at ticks 0
   */
  void tare() {
    controller.tarePosition();
    move((float) 0);
  }

  void waitUntilSettled() {
    controller.waitUntilSettled();
  }

  float getCurrentTicks() {
    // return controller.getTarget() + tareTicks;
    return getTargetTicks() - controller.getError();
  }

  void stop() {
    if (isBacking) {
      stopBacking();
    }
    move(getCurrentTicks());
  }
};
