#include "main.h"
#include "ports.hpp"

pros::ADIUltrasonic left_ultra (LEFT_ULTRA_PING_PORT, LEFT_ULTRA_ECHO_PORT);
pros::ADIUltrasonic right_ultra (RIGHT_ULTRA_PING_PORT, RIGHT_ULTRA_ECHO_PORT);
pros::ADIUltrasonic side_ultra (SIDE_ULTRA_PING_PORT, SIDE_ULTRA_ECHO_PORT);

#define ULTRA_SPACING 311_mm

QAngle getAngle() {
  // CCW Angle
  // only works if looking at a flat surface
  // mm
  int lv = left_ultra.get_value();
  int rv = right_ultra.get_value();
  QLength ld = lv*1_mm;
  QLength rd = rv*1_mm;
  float r = ((rd-ld)/ULTRA_SPACING).getValue();
  return atan(r)*1_rad;
}

QLength getSideDist() {
  // distance of center of robot from wall
  // only works on left side looking at a flat surface
  return side_ultra.get_value()*1_mm+183_mm;
}
