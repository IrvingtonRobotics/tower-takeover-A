#include "railscalib.hpp"

float interp(float theta, int m) {
  // arr[m] > theta > arr[m+1]
  // linear interp between two points
  return (railsintPoints[m+1].dtheta_dt - railsintPoints[m].dtheta_dt)
    / (railsintPoints[m+1].theta - railsintPoints[m].theta) * (theta - railsintPoints[m].theta)
    + railsintPoints[m].dtheta_dt;
}

float findDesiredRailsSpeed(float theta, int l, int r) {
  // binary search
  int mid = (l+r)/2;
  // not gonna happen?
  float t = railsintPoints[mid].theta;
  if (railsintPoints[mid].theta > theta && theta > railsintPoints[mid+1].theta) {
    return interp(theta, mid);
  }
  if (t > theta) {
    return findDesiredRailsSpeed(theta, mid, r);
  }
  if (t < theta) {
    return findDesiredRailsSpeed(theta, l, mid);
  }
}

float getDesiredRailsSpeed(float theta) {
  // angle in radians
  // gives desired dTheta_dt
  if (theta > railsintPoints[0].theta) {
    // P controller
    return -30 * (theta - 1.57);
  }
  RailsintPoint lastPoint = railsintPoints[RAILSINT_SIZE-1];
  if (theta < lastPoint.theta) {
    return -3 * (theta - lastPoint.theta) + lastPoint.dtheta_dt;
  }
  return findDesiredRailsSpeed(theta, 0, RAILSINT_SIZE);
}
