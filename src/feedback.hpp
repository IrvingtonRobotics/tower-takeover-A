#ifndef _SRC_FEEDBACK_H
#define _SRC_FEEDBACK_H

#include "main.h"

class Feedback {
  const QTime ATTENTION_INTERVAL = 500_ms;
  Controller controller;
  Timer attnTimer;
  bool doAttention = false;
  string currentText = "waddup";

public:
  Feedback() {
    attnTimer = Timer();
  }

  void step() {
    if (attnTimer.repeat(ATTENTION_INTERVAL)) {
      printf("TRY RUMBLEEEEE\n");
      controller.rumble(".");
      doAttention = false;
      attnTimer.placeHardMark();
    }
    controller.setText(0, 0, "hi");
  }

  void attention() {
    doAttention = true;
  }

  void print(string text) {
    currentText = text;
  }
};

#endif
