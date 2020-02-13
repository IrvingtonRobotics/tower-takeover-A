#ifndef SRC_ULTRASONIC_H_
#define SRC_ULTRASONIC_H_

struct posAngle;
QAngle getAngle(int dir);
posAngle getPosAngle(int dir);
void printUSStatus();

#endif
