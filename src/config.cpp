#include "common.hpp"

namespace {
class Config {
  static const lv_align_t ALIGNMENT = LV_ALIGN_IN_TOP_MID;
  static lv_obj_t * colorLabel;

  static lv_obj_t * createLabel(lv_obj_t * parent, lv_coord_t x, lv_coord_t y,
    lv_align_t alignment, const char * title
  ) {
    lv_obj_t * label = lv_label_create(parent, NULL);
    lv_label_set_text(label, title);
    lv_obj_align(label, NULL, alignment, x, y);
    return label;
  }

  static lv_obj_t * createLabel(lv_obj_t * parent, lv_coord_t x, lv_coord_t y,
    const char * title
  ) {
    return createLabel(parent, x, y, LV_ALIGN_CENTER, title);
  }

  static lv_obj_t * createLabel(lv_coord_t x, lv_coord_t y, const char * title) {
    return createLabel(lv_scr_act(), x, y, title);
  }

  static lv_obj_t * createButton(lv_obj_t * parent, lv_coord_t x, lv_coord_t y, lv_coord_t width, lv_coord_t height,
    int id, const char * title, lv_res_t (*clickAction)(lv_obj_t*)
  ) {
    lv_obj_t * button = lv_btn_create(parent, NULL);
    lv_obj_set_pos(button, x, y);
    lv_obj_set_size(button, width, height);
    lv_obj_set_free_num(button, id);
    lv_btn_set_action(button, LV_BTN_ACTION_CLICK, clickAction);
    createLabel(button, 0, 5, title);
    return button;
  }

  static lv_obj_t * createButton(lv_coord_t x, lv_coord_t y, lv_coord_t width, lv_coord_t height,
    int id, const char * title, lv_res_t (*clickAction)(lv_obj_t*)
  ) {
    return createButton(lv_scr_act(), x, y, width, height, id, title, clickAction);
  }

  static lv_style_t createStyle(lv_style_t start, lv_color_t main_color,
    lv_color_t grad_color, lv_color_t color, int radius
  ) {
    lv_style_t style;
    lv_style_copy(&style, &start);
    style.body.main_color = main_color;
    style.body.grad_color = grad_color;
    style.body.radius = radius;
    style.text.color = color;
    return style;
  }

  static lv_style_t createStyle(lv_color_t main_color, lv_color_t grad_color,
    lv_color_t color, int radius
  ) {
    return createStyle(lv_style_plain, main_color, grad_color, color, radius);
  }

  static void updateMessages() {
    char buffer[100];
    sprintf(buffer, "Color select: %s", isRed ? "red" : "blue");
    lv_label_set_text(colorLabel, buffer);
  }

  static lv_res_t switch_color(lv_obj_t * button) {
    isRed = !isRed;
    updateMessages();
    return LV_RES_OK;
  }

public:

  static void initialize() {
    static lv_style_t myButtonStyleREL = createStyle(
      LV_COLOR_MAKE(150, 0, 0),
      LV_COLOR_MAKE(0, 0, 150),
      LV_COLOR_MAKE(255, 255, 255),
      0
    );

    static lv_style_t myButtonStylePR = createStyle(
      LV_COLOR_MAKE(255, 0, 0),
      LV_COLOR_MAKE(0, 0, 255),
      LV_COLOR_MAKE(255, 255, 255),
      0
    );

    static lv_obj_t * colorButton = createButton(lv_scr_act(), 10, 10, 200, 50, 0,
      "Switch Color", switch_color);

    colorLabel = createLabel(lv_scr_act(), 220, 25, LV_ALIGN_IN_TOP_LEFT, "");

    updateMessages();
  }
};

// initialize shared variables to avoid undefined errors
lv_obj_t * Config::colorLabel;
}
