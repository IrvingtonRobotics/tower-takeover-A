#ifndef _COMMON_
#define _COMMON_

#include "main.h"
#include "lift.cpp"
#include "drive.cpp"
#include "rails.cpp"
#include "intake.cpp"

using namespace okapi;

extern Lift lift;
extern Drive drive;
extern Rails rails;
extern Intake intake;

extern bool is_red;
extern bool is_back;

#endif
