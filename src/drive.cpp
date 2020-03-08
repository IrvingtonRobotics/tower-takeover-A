#include "main.h"
#include "ports.hpp"
#include "feedback.hpp"

extern Feedback feedback;

#define controller (*controllerPtr)

/**
 * Main class for the wheeled driving subsystem
 * Wraps around a ChassisControllerIntegrated with limited acceleration
 */
class Drive {
  /* ---- CONFIG ---- */
  const float MAX_DRIVE_ACCEL = 0.02;
  const float MAX_DRIVE_DECEL = 0.20;
  const float ARMS_UP_TURN_SCALE = 0.8;
  const float DEAD_ZONE_WIDTH = 0.2;
  // range at top where will be full speed
  const float UNDEAD_ZONE_WIDTH = 0.15;
  // wheel diameter and width of wheelbase
  const ChassisScales &scales = ChassisScales({4_in, 13_in});
  //https://pros.cs.purdue.edu/v5/okapi/api/device/motor/abstract-abstract-motor.html#gearset
  static const auto gearset = AbstractMotor::gearset::green;
  Motor motors[4] = {
    Motor(DRIVE_LEFT_BACK_PORT),
    Motor(DRIVE_LEFT_FRONT_PORT),
    Motor(DRIVE_RIGHT_BACK_PORT),
    Motor(DRIVE_RIGHT_FRONT_PORT)
  };

  /* ---- No need to edit ---- */
  float leftDriveSpeed = 0;
  float rightDriveSpeed = 0;
  ChassisControllerIntegrated controllerStraight = getController(false);
  ChassisControllerIntegrated controllerReversed = getController(true);
  ChassisControllerIntegrated* controllerPtr = &controllerStraight;

  ChassisControllerIntegrated controllerBack = ChassisControllerFactory::create(
    DRIVE_LEFT_BACK_PORT,
    -DRIVE_RIGHT_BACK_PORT,
    gearset,
    scales
  );

  ChassisControllerIntegrated getController(bool isReversed) {
    // we must change the controller instead of controller.setTurnsMirrored
    // to let profileController work
    if (isReversed) {
      return ChassisControllerFactory::create(
        {-DRIVE_RIGHT_FRONT_PORT, -DRIVE_RIGHT_BACK_PORT},
        {DRIVE_LEFT_FRONT_PORT, DRIVE_LEFT_BACK_PORT},
        gearset,
        scales
      );
    } else {
      return ChassisControllerFactory::create(
        {DRIVE_LEFT_FRONT_PORT, DRIVE_LEFT_BACK_PORT},
        {-DRIVE_RIGHT_FRONT_PORT, -DRIVE_RIGHT_BACK_PORT},
        gearset,
        scales
      );
    }
  }

  /**
   * Get new drive speed after one tick for one side of the robot,
   * taking into account acceleration limits
   */
  float getNewDriveSpeed(float current, float input) {
    // tried using clamp, but difficulties
    float lo;
    float hi;
    if (current >= 0) {
      lo = current - MAX_DRIVE_DECEL;
      hi = current + MAX_DRIVE_ACCEL;
    } else {
      lo = current - MAX_DRIVE_ACCEL;
      hi = current + MAX_DRIVE_DECEL;
    }
    if (lo > input) return lo;
    if (hi < input) return hi;
    return input;
  }

public:
  void step() {
    for (int i=0; i<4; i++) {
      float e = motors[i].getEfficiency();
      // printf("%f\t", e);
      if (e > 60) {
        feedback.print("Motor loose");
        feedback.attention();
      }
    }
  }

  void moveTank(float left, float right) {
    moveTank(left, right, false);
  }

  void moveTank(float left, float right, bool stopFront) {
    // controller.flipDisable(false);
    // controllerBack.flipDisable(true);
    if (!stopFront) {
      controllerBack.tank(0, 0);
      controller.tank(leftDriveSpeed, rightDriveSpeed);
    } else {
      controller.tank(0, 0);
      controllerBack.tank(leftDriveSpeed, rightDriveSpeed);
    }
  }

  /**
   * Arcade drive based on controllerX and controllerY
   * Pass through controller.tank to use existing acceleration limit code
   */
  void move(float controllerX, float controllerY, float scl, bool stopFront, bool armsUp) {
    // Dead Zone check
    float dist = sqrt(controllerX*controllerX + controllerY*controllerY);
    float distWanted = (dist - DEAD_ZONE_WIDTH)/(1 - DEAD_ZONE_WIDTH - UNDEAD_ZONE_WIDTH);
    if (distWanted < 0) {
      distWanted = 0;
    }
    if (distWanted > 1) {
      distWanted = 1;
    }
    controllerX *= distWanted/dist;
    controllerY *= distWanted/dist;
    float turnRate = controllerX;
    // move to positive value
    int turnMult = turnRate > 0 ? 1 : -1;
    turnRate *= turnMult;
    if (armsUp) {
      turnRate *= ARMS_UP_TURN_SCALE;
    }
    // smoothening curve
    turnRate = turnRate*(0.1+turnRate*(0.3+turnRate*0.6));
    turnRate *= turnMult;
    // compute tank left and right based on arcade x and y
    float left = controllerY + turnRate;
    float right = controllerY - turnRate;
    left *= scl;
    right *= scl;
    leftDriveSpeed = getNewDriveSpeed(leftDriveSpeed, left);
    rightDriveSpeed = getNewDriveSpeed(rightDriveSpeed, right);
    moveTank(leftDriveSpeed, rightDriveSpeed, stopFront);
  }

  /**
   * Synchronous
   *
   * Move a specific distance.
   * No acceleration limits are applied, so this might cause chain to slip
   */
  void moveDistance(QLength length) {
    // might click
    controller.moveDistance(length);
  }

  void stop() {
    moveTank(0, 0);
  }

  /**
   * Turn clockwise some angle, use negative to turn counterclockwise
   */
  void turnAngle(QAngle angle) {
    controller.turnAngle(angle);
  }

  /**
   * Set side to reverse angles
   */
  void setSide(bool isRed) {
    if (isRed) {
      controllerPtr = &controllerReversed;
    } else {
      controllerPtr = &controllerStraight;
    }
  }

  /**
   * Reset to blue
   */
   void straighten() {
     setSide(false);
   }

   auto getProfileController(float speed) {
     return AsyncControllerFactory::motionProfile(
       1.5 * speed,
       8.0,
       20.0,
       controller
     );
   }

   void setMaxVelocity(float rpm) {
     controllerStraight.setMaxVelocity(rpm);
     controllerReversed.setMaxVelocity(rpm);
   }

   void setMaxVelocity(RQuantity<std::ratio<0>, std::ratio<1>, std::ratio<-1>, std::ratio<0>> speed) {
     setMaxVelocity((speed / (4_in * pi) * 1_min).getValue());
   }

   void resetMaxVelocity() {
     setMaxVelocity(600);
   }

};

#undef controller
