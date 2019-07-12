#ifndef _COMMON_
#define _COMMON_

#include "main.h"
#include "lift.cpp"
#include "rails.cpp"
#include "intake.cpp"

using namespace okapi;

extern ChassisControllerIntegrated drive;
extern Lift lift;
extern Rails rails;
extern Intake intake;

extern bool is_red;
extern bool is_back;

#endif
