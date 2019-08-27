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

extern Lift lift;
extern Drive drive;
extern Rails rails;
extern Intake intake;

extern bool is_red;
extern bool is_back;

#endif
