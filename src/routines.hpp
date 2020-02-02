#ifndef SRC_ROUTINES_H_
#define SRC_ROUTINES_H_

void foldout();
void foldin();
void travelProfile(std::initializer_list<okapi::Point> iwaypoints,
  bool backwards, float speed
);

#endif
