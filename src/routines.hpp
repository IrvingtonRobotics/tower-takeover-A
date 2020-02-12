#ifndef SRC_ROUTINES_H_
#define SRC_ROUTINES_H_

void foldout(bool doBackward);
void foldout();
void travelProfile(std::initializer_list<okapi::Point> iwaypoints,
  bool backwards, float speed
);
void stack();

#endif
