#include<math.h>
#include<cstdio>

// horizontal offset of center of push gear from rail pivot
const float x10 = 10; // guess
// vertical offset of center of push gear from rail pivot
const float y20 = 2; // guess
// length of the upper push segment
const float x20 = 7.5; // guess
// length of the lower push segment
const float y10 = 7.5; // guess
// perpendicular offset from rails to push pivot
const float x30 = 1; // guess
// distance of push pivot from rails pivot along rails
const float y30 = 15; // guess
// calculated
const float l10 = sqrt(x30*x30+y30*y30);
const float l20 = sqrt(x10*x10+y20*y20);
const float pi = 3.141592653589793;
const float TICKS_PER_ROT = 1800*15;
const float RAILS_BACK_THETA = 0.9;

float _thetaToTicks(float theta) {
  float eac = theta - atan(y20/x10) - atan(x30/y30);
  // shouldn't suffer from loss of precision because we never get small angles
  float ec = sqrt(l10*l10 + l20*l20 - 2*l10*l20*cos(eac));
  float ace = asin(sin(eac)*l10/ec);
  float ecd = acos((ec*ec+y10*y10-x20*x20)/(2*y10*ec));
  float t = 3*pi/2-ace-ecd-atan(x10/y20);
  return t / (2*pi) * TICKS_PER_ROT;
}

int main() {
  printf("%f\n", _thetaToTicks(45*pi/180));
}
