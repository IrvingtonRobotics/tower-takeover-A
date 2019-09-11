/**
 * Initialization, run before all other code as soon as the proram is started
 */

#include "common.hpp"

/**
 * LCD Configuration
 */

const lv_align_t ALIGNMENT = LV_ALIGN_IN_TOP_MID;

bool is_red = true;
bool is_back = true;

lv_obj_t * createLabel(lv_obj_t * parent, lv_coord_t x, lv_coord_t y, const char * title) {
  lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, title);
  lv_obj_align(label, NULL, ALIGNMENT, x, y);
  return label;
}

lv_obj_t * createButton(lv_obj_t * parent, lv_coord_t x, lv_coord_t y, lv_coord_t width, lv_coord_t height,
    int id, const char * title, lv_res_t (*clickAction)(lv_obj_t*))
{

    lv_obj_t * button = lv_btn_create(parent, NULL);
    lv_obj_set_pos(button, x, y);
    lv_obj_set_size(button, width, height);
    lv_obj_set_free_num(button, id);
    lv_btn_set_action(button, LV_BTN_ACTION_CLICK, clickAction);

    createLabel(button, 0, 5, title);

    return button;
}

lv_obj_t * myButton;
lv_obj_t * myButtonLabel;
lv_obj_t * myLabel;

lv_style_t myButtonStyleREL; //relesed style
lv_style_t myButtonStylePR; //pressed style

static lv_res_t button_click_action(lv_obj_t * button)
{
  uint8_t id = lv_obj_get_free_num(button); //id usefull when there are multiple buttons

  if(id == 0)
  {
    char buffer[100];
    sprintf(buffer, "button was clicked %i milliseconds from start", pros::millis());
    lv_label_set_text(myLabel, buffer);
  }

  return LV_RES_OK;
}

void set_color_text() {
  std::string color = is_red ? "red" : "blue";
	pros::lcd::set_text(2, "Color: " + color + ".");
}

void set_location_text() {
  std::string location = is_back ? "back" : "side";
  pros::lcd::set_text(3, "Location: " + location + ".");
}

void on_left_button() {
  is_red = !is_red;
  set_color_text();
}

void on_right_button() {
  is_back = !is_back;
  set_location_text();
}

// Instantiate subsystem classes
Lift lift = Lift();
Drive drive = Drive();
Rails rails = Rails();
Intake intake = Intake();

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  /**
   * Parts Configuration
   */

  drive.setSide(is_red);

  lift.setMaxVelocity(70);
  lift.tare();

  rails.tare();

  /**
   * LCD Configuration
   */

   lv_style_copy(&myButtonStyleREL, &lv_style_plain);
   myButtonStyleREL.body.main_color = LV_COLOR_MAKE(150, 0, 0);
   myButtonStyleREL.body.grad_color = LV_COLOR_MAKE(0, 0, 150);
   myButtonStyleREL.body.radius = 0;
   myButtonStyleREL.text.color = LV_COLOR_MAKE(255, 255, 255);

   lv_style_copy(&myButtonStylePR, &lv_style_plain);
   myButtonStylePR.body.main_color = LV_COLOR_MAKE(255, 0, 0);
   myButtonStylePR.body.grad_color = LV_COLOR_MAKE(0, 0, 255);
   myButtonStylePR.body.radius = 0;
   myButtonStylePR.text.color = LV_COLOR_MAKE(255, 255, 255);

   myButton = createButton(lv_scr_act(), 10, 10, 200, 50, 0, "Click the Button yee", button_click_action);
   // myButton = lv_btn_create(lv_scr_act(), NULL); //create button, lv_scr_act() is deafult screen object
   // lv_obj_set_free_num(myButton, 0); //set button is to 0
   // lv_btn_set_action(myButton, LV_BTN_ACTION_CLICK, btn_click_action); //set function to be called on button click
   // lv_btn_set_style(myButton, LV_BTN_STYLE_REL, &myButtonStyleREL); //set the relesed style
   // lv_btn_set_style(myButton, LV_BTN_STYLE_PR, &myButtonStylePR); //set the pressed style
   // lv_obj_set_size(myButton, 200, 50); //set the button size
   // lv_obj_align(myButton, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10); //set the position to top mid

   // myButtonLabel = lv_label_create(myButton, NULL); //create label and puts it inside of the button
   // lv_label_set_text(myButtonLabel, "Click the Button"); //sets label text

   myLabel = lv_label_create(lv_scr_act(), NULL); //create label and puts it on the screen
   lv_label_set_text(myLabel, "Button has not been clicked yet"); //sets label text
   lv_obj_align(myLabel, NULL, LV_ALIGN_IN_LEFT_MID, 10, 0); //set the position to center
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  drive.straighten();
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}
