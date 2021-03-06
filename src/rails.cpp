#include "main.h"
#include "ports.hpp"
#include "railscalib.hpp"
#include "feedback.hpp"

extern Feedback feedback;

extern bool doClearArm;
extern bool doUnclearArm;
extern bool doClearDropArm;

/**
 * Main class for the Rails angling subsystem
 * Wraps around AsyncPosIntegratedController and AsyncVelIntegratedController
 * in the manner of Lift (some of the code is transferred -- sad)
 * Need a limit because the rails can now start in the middle for space concerns
 **/
class Rails {
  /* ---- CONFIG ---- */
  const double RAILS_TARE_THETA = 0.75;
  // ticks all the way back
  const double RAILS_BACK_THETA = 0.82;
  const double CLEAR_ARM_THETA = 1.25;
  // ticks to rest in middle (moveMid)
  const double RAILS_MID_THETA = 1.2;
  // ticks all the way forward
  const double RAILS_FORWARD_THETA = 1.57;
  const double RAILS_DONE_NOTIFY_THRESHOLD = 0.05;
  const float SCOOT_DTHETA = 0.01;
  const float SCOOT_SPEED = 400;
  const double CLEAR_ARM_SPEED = 600;
  const double CLEAR_BACK_SPEED = 150;
  const double MOVE_BACK_SPEED = 520;
  const double MOVE_MID_SPEED = 300;
  // different scale
  const float STACKING_SPEED = 0.05;
  const QTime BACK_TO_BUTTON_TIMEOUT = 8_s;
  // for backToButton
  const int DEFAULT_BACK_SPEED = 220;
  const QTime UNCLEAR_ARM_DELAY = 250_ms;
  Timer unclearArmTimer = Timer();
  bool delayingUnclearArm = true;
  // start true so we don't vibrate as soon as match starts
  bool railsDoneNotified = true;

  /* ---- No need to edit ---- */
  // // ticks half way between back and forward
  // const double RAILS_MIDPOINT_TICKS = (RAILS_BACK_TICKS + RAILS_FORWARD_TICKS) / 2;
  // // ticks half way between back and mid
  // const double RAILS_MID_MIDPOINT_TICKS = (RAILS_BACK_TICKS + RAILS_MID_TICKS) / 2;
  const int PORT = -ANGLE_RAILS_PORT;
  bool isBacking = false;
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated(PORT);
  AsyncVelIntegratedController velController =
    AsyncControllerFactory::velIntegrated(PORT);
  ADIButton buttonLimit = ADIButton(RAILS_LIMIT_PORT);
  bool isStacking = false;
  bool isArmCleared = false;

  // horizontal offset of center of push gear from rail pivot
  const float x1 = (8.5_in).getValue();
  // vertical offset of center of push gear from rail pivot
  const float y2 = (2_in).getValue();
  // length of the upper push segment
  const float x2 = (6.0_in).getValue();
  // length of the lower push segment
  const float y1 = (6.0_in).getValue();
  // perpendicular offset from rails to push pivot
  const float x3 = (1_in).getValue();
  // distance of push pivot from rails pivot along rails
  const float y3 = (11.0_in).getValue();
  // calculated
  const float l1 = sqrt(x3*x3+y3*y3);
  const float l2 = sqrt(x1*x1+y2*y2);
  // Torque motor * High strength gears compounded
  const float TICKS_PER_REV = 720 * 15;
  float _thetaToTicks(float theta) {
    float eac = theta - atan(y2/x1) - atan(x3/y3);
    // shouldn't suffer from loss of precision because we never get small angles
    float ec = sqrt(l1*l1 + l2*l2 - 2*l1*l2*cos(eac));
    float ace = asin(sin(eac)*l1/ec);
    float ecd = acos((ec*ec+y1*y1-x2*x2)/(2*y1*ec));
    float t = 3*pi/2-ace-ecd-atan(x1/y2);
    return t / (2*pi) * TICKS_PER_REV;
  }

  float thetaToTicks(float theta) {
    return _thetaToTicks(theta) - _thetaToTicks(RAILS_TARE_THETA);
  }

  float _ticksToTheta(float ticks) {
    float t = ticks / TICKS_PER_REV * (2*pi);
    float acd = 3*pi/2-t-atan(x1/y2);
    float ad = sqrt(y1*y1+l2*l2-2*y1*l2*cos(acd));
    float dac = asin(sin(acd)*y1/ad);
    float ead = acos((l1*l1+ad*ad-x2*x2)/(2*l1*ad));
    float theta = dac+ead+atan(y2/x1)+atan(x3/y3);
    return theta;
  }

  float ticksToTheta(float ticks) {
    return _ticksToTheta(ticks + _thetaToTicks(RAILS_TARE_THETA));
  }

  float dTicksDTheta(float theta) {
    const float h = 0.0001;
    return (_thetaToTicks(theta+h)-_thetaToTicks(theta))/h;
  }

  void move(double ticks) {
    float destAngle = ticksToTheta(ticks);
    printf("Moving rails to %f\n", destAngle);
    if (destAngle < RAILS_BACK_THETA) {
      // would die moving back
      return;
    }
    if (destAngle < CLEAR_ARM_THETA) {
      doClearDropArm = true;
    }
    abortStack();
    printf("Moving rails to %f ticks\n", ticks);
    railsDoneNotified = false;
    controller.setTarget(ticks);
  }

  void move(double ticks, double maxSpeed) {
    setMaxVelocity(maxSpeed);
    move(ticks);
  }

  void abortStack() {
    printf("_---------------------------__STACK ABORT__--------------------------_\n");
    isStacking = false;
    flipDisable(false);
  }

  void setMaxVelocity(double ticks) {
    controller.setMaxVelocity(ticks);
  }

  /**
   * Switch control between position controller and velocity controller
   */
  void flipDisable(bool posDisabled) {
    controller.flipDisable(posDisabled);
    velController.flipDisable(!posDisabled);
  }

  bool isBack() {
    return getTargetTheta() < (RAILS_FORWARD_THETA + RAILS_BACK_THETA)/2;
  }

  bool isBackMid() {
    return getTargetTicks() < (RAILS_FORWARD_THETA + RAILS_BACK_THETA)/2;
  }

  float getTargetTicks() {
    return controller.getTarget();
  }

  float getTargetTheta() {
    return ticksToTheta(getTargetTicks());
  }

  float getCurrentTicks() {
    return getTargetTicks() - controller.getError();
  }

  float getCurrentTheta() {
    return ticksToTheta(getCurrentTicks());
  }

public:
  Rails() {
    flipDisable(false);
    setMaxVelocity(30);
  }

  void moveAngle(float theta, float maxSpeed) {
    move(thetaToTicks(theta), maxSpeed);
  }

  void moveBack() {
    moveBack(MOVE_BACK_SPEED);
  }

  void moveBack(double maxSpeed) {
    moveAngle(RAILS_BACK_THETA, maxSpeed);
  }

  void moveMid() {
    moveMid(MOVE_MID_SPEED);
  }

  void moveMid(double maxSpeed) {
    moveAngle(RAILS_MID_THETA, maxSpeed);
  }

  void stackForward() {
    // moveAngle(RAILS_FORWARD_THETA, 100);
    // for getTargetTicks
    controller.setTarget(thetaToTicks(RAILS_FORWARD_THETA));
    isStacking = true;
    flipDisable(true);
  }

  void step() {
    // printf("isStacking=%s, ticks=%f, theta=%f, goalticks=%f, goaltheta=%f\n", isStacking?"true":"false", getCurrentTicks(), getCurrentTheta(), getTargetTicks(), getTargetTheta());
    float theta = getCurrentTheta();
    if (!railsDoneNotified) {
      bool doneBack = theta < RAILS_BACK_THETA + RAILS_DONE_NOTIFY_THRESHOLD;
      bool doneForward = theta > RAILS_FORWARD_THETA - RAILS_DONE_NOTIFY_THRESHOLD;
      if (doneBack || doneForward) {
        feedback.attention();
        railsDoneNotified = true;
      }
    }
    if (isBacking) {
      if (buttonLimit.isPressed()) {
        stopBacking();
      }
    }
    if (isStacking) {
      float desiredSpeed = getDesiredRailsSpeed(theta);
      printf("desiredSpeed dThetaDT %f\n", desiredSpeed);
      desiredSpeed = desiredSpeed * dTicksDTheta(theta);
      desiredSpeed *= STACKING_SPEED;
      printf("desiredSpeed dTicksDT %f\n", desiredSpeed);
      velController.setTarget(desiredSpeed);
      if (theta > RAILS_FORWARD_THETA-0.001) {
        velController.setTarget(0);
        abortStack();
        controller.setTarget(thetaToTicks(RAILS_FORWARD_THETA));
      }
    }
    if (doClearArm) {
      clearArm();
      doClearArm = false;
    }
    if (doUnclearArm) {
      delayingUnclearArm = true;
      doUnclearArm = false;
      unclearArmTimer.placeHardMark();
    } else if (delayingUnclearArm) {
      if (unclearArmTimer.getDtFromHardMark() > UNCLEAR_ARM_DELAY) {
        delayingUnclearArm = false;
        unclearArm();
      }
    }
  }

  void stopBacking() {
    // hand control back to pos
    flipDisable(false);
    isBacking = false;
    velController.setTarget(0);
    tare();
    // avoid the controller resuming to its previous location
    controller.setTarget(0);
  }

  void startBacking(int speed) {
    // move control to vel for smooth movement
    flipDisable(true);
    isBacking = true;
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
    if (isBack()) {
      stackForward();
    } else {
      moveBack();
    }
  }

  void togglePositionMid() {
    if (isBackMid()) {
      moveMid();
    } else {
      moveBack();
    }
  }

  /**
   * Retare by assuming current position is at ticks 0
   */
  void tare() {
    controller.tarePosition();
    controller.setTarget(0);
  }

  void waitUntilSettled() {
    while (isBacking) {
      pros::delay(10);
    }
    controller.waitUntilSettled();
  }

  void scoot() {
    moveAngle(getCurrentTheta() + SCOOT_DTHETA, SCOOT_SPEED);
  }

  void clearArm() {
    if (getTargetTheta() < CLEAR_ARM_THETA) {
      moveAngle(CLEAR_ARM_THETA, CLEAR_ARM_SPEED);
      isArmCleared = true;
    }
  }

  void unclearArm() {
    if (isArmCleared) {
      isArmCleared = false;
      moveAngle(RAILS_BACK_THETA, CLEAR_BACK_SPEED);
    }
  }

  void stop() {
    if (isBacking) {
      stopBacking();
    }
    move(getCurrentTicks());
  }
};
