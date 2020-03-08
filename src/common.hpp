/**
 * Common definitions shared between initialize, autonomous, and opcontrol
 * Includes and makes instances of each subsystem
 */


#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#define SMALL_SIDE 1
#define BIG_SIDE 2
#define SKILLS 3
#define DRIVER 4

#include "main.h"
#include "config.hpp"
#include "feedback.hpp"
#include "drive.cpp"
#include "rails.cpp"
#include "lift.cpp"
#include "intake.cpp"
#include "routines.hpp"
#include "ultrasonic.hpp"

extern Drive drive;
extern Rails rails;
extern Intake intake;
extern Lift lift;
extern Feedback feedback;

extern Timer autonTimer;
extern Timer cumTimer;

extern bool isRed;

extern bool doClearArm;
extern bool doUnclearArm;

struct posAngle {
  QLength x;
  QLength y;
  QAngle theta;
};

#define WEST 0
#define SOUTH 1
#define EAST 2
#define NORTH 3

#endif
