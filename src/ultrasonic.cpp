#include "main.h"
#include "ports.hpp"

pros::ADIUltrasonic left_ultra (LEFT_ULTRA_PING_PORT, LEFT_ULTRA_ECHO_PORT);
pros::ADIUltrasonic right_ultra (RIGHT_ULTRA_PING_PORT, RIGHT_ULTRA_ECHO_PORT);
pros::ADIUltrasonic side_ultra (SIDE_ULTRA_PING_PORT, SIDE_ULTRA_ECHO_PORT);

#define ULTRA_SPACING 311_mm

float getAngle(int lv, int rv) {
  QLength ld = lv*1_mm;
  QLength rd = rv*1_mm;
  float r = ((rd-ld)/ULTRA_SPACING).getValue();
  return atan(r);
}

QAngle getAngle() {
  // CCW Angle
  // only works if looking at a flat surface
  // mm
  int lv = left_ultra.get_value();
  int rv = right_ultra.get_value();
  return getAngle(lv, rv)*1_rad;
}

QLength getSideDist() {
  // distance of center of robot from wall
  // only works on left side looking at a flat surface
  return side_ultra.get_value()*1_mm+183_mm;
}

QLength getBackDist() {
  int lv = left_ultra.get_value();
  int rv = right_ultra.get_value();
  float angle = getAngle(lv, rv);
  if (angle < 0) angle *= -1;
  QLength ld = lv*1_mm;
  QLength rd = rv*1_mm;
  if (ld < rd) {
    ld = rd;
  }
  // now ld is greater, angle is positive
  return 1_in;
  // now I realize this is harder than I thought
  // SHould CALCULATE X, Y, THETA at same time based on 3 sensors
  // TODO
  // TODO
  // TODO
  // TODO

  // time for me to sleep
}
