#ifndef _COMMON_
#define _COMMON_

#include "main.h"

using namespace okapi;

extern ChassisControllerIntegrated drive;

extern bool is_red;
extern bool is_back;

extern AsyncPosIntegratedController lift;
extern const QLength armLength;
extern const QLength armElevation;
extern const int numHeights;
extern const QLength targetHeights[];
extern const int ticksPerRev;
extern int liftTareTicks;

QLength getLiftHeight(double ticks);
double getLiftTicks(QLength height);

void liftTareHeight(QLength height);
void moveLift(int height);
void moveLift(QLength height);

extern AsyncPosIntegratedController rails;
void moveRails(double ticks);

#endif
