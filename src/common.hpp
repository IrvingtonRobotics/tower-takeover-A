/**
 * Common definitions shared between initialize, autonomous, and opcontrol
 * Includes and makes instances of each subsystem
 */


#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

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

extern bool isRed;
extern bool isSmallSide;

#endif
