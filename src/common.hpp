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
#include "lift.cpp"
#include "drive.cpp"
#include "rails.cpp"
#include "intake.cpp"
#include "routines.hpp"

extern Lift lift;
extern Drive drive;
extern Rails rails;
extern Intake intake;

extern Timer autonTimer;
extern Timer cumTimer;

extern bool isRed;

#endif
