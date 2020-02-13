#include "main.h"
#include "ports.hpp"
#include "common.hpp"

pros::ADIUltrasonic left_ultra (LEFT_ULTRA_PING_PORT, LEFT_ULTRA_ECHO_PORT);
pros::ADIUltrasonic right_ultra (RIGHT_ULTRA_PING_PORT, RIGHT_ULTRA_ECHO_PORT);
pros::ADIUltrasonic side_ultra (SIDE_ULTRA_PING_PORT, SIDE_ULTRA_ECHO_PORT);

// half body - measurement
const QLength CENTER_LR_PERP = 17.5_in/2 - 37_mm;
const QLength CENTER_LEFT_HORIZ = 15.5_in/2 - 3.5_in;
const QLength CENTER_RIGHT_HORIZ = 15.5_in/2 - 3.5_in;
// half body - measurement + 4 hole offset
const QLength CENTER_SIDE_PERP = 15.5_in/2 - 107_mm + 2_in;
const QLength CENTER_SIDE_HORIZ = 17.5_in/2 - 5.5_in;

QLength leftDist() {
  return left_ultra.get_value() * 1_mm + CENTER_LR_PERP;
}

QLength rightDist() {
  return right_ultra.get_value() * 1_mm + CENTER_LR_PERP;
}

QLength sideDist() {
  return side_ultra.get_value() * 1_mm + CENTER_SIDE_PERP;
}

void printPAPos() {
  posAngle pa = getPosAngle(WEST);
  printf("Pos x=%f_mm y=%f_mm theta=%f_deg\n", pa.x/1_mm, pa.y/1_mm, pa.theta/1_deg);
}

void printUSReadings() {
  printf("US readings left=%f_mm right=%f_mm side=%f_mm\n", leftDist()/1_mm, rightDist()/1_mm, sideDist()/1_mm);
}

QAngle getAngle(QLength ld, QLength rd) {
  float r = ((rd-ld)/(CENTER_LEFT_HORIZ+CENTER_RIGHT_HORIZ)).getValue();
  return atan(r)*1_rad;
}

QAngle getAngle(int dir) {
  // CCW Angle
  // only works if looking at a flat surface
  return getAngle(leftDist(), rightDist()) + dir*90_deg;
}

posAngle getPosAngle(int dir) {
  // dir refers to the direction the ANGLE pair of ultrasonic sensors are facing: WEST, SOUTH, EAST, NORTH
  // assume the side US sensor is CW to it: NORTH, WEST, SOUTH, EAST
  QLength ld = leftDist();
  QLength rd = rightDist();
  QLength sd = sideDist();
  QAngle theta = getAngle(ld, rd);
  float t = (theta/1_rad).getValue();
  printf("Side dist %f_mm\n", sd/1_mm);
  QLength y0 = (sd - CENTER_SIDE_HORIZ * tan(t)) * cos(t);
  printf("cost %f\n", cos(t));
  printf("tant %f\n", tan(t));
  printf("csh %f\n", CENTER_SIDE_HORIZ/1_mm);
  printf("e1 %f\n", (sd - CENTER_SIDE_HORIZ * tan(t)));
  printf("y0 %f_mm\n", y0/1_mm);
  QLength x0 = (ld - CENTER_LEFT_HORIZ * tan(t)) * cos(t);
  posAngle pa;
  if (dir == WEST) {
    pa.x = x0;
    pa.y = y0;
    pa.theta = theta;
  }
  if (dir == NORTH) {
    pa.x = 144_in - y0;
    pa.y = x0;
    pa.theta = theta + 90_deg;
  }
  if (dir == EAST) {
    pa.x = 144_in - x0;
    pa.y = 144_in - y0;
    pa.theta = theta + 180_deg;
  }
  if (dir == SOUTH) {
    pa.x = y0;
    pa.y = 144_in - x0;
    pa.theta = theta - 90_deg;
  }
  return pa;
}
