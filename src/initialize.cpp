#include "common.hpp"
/**
 * LCD Configuration
 */

bool is_red = true;
bool is_back = true;

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

ChassisControllerIntegrated drive = ChassisControllerFactory::create(
  -10, 1,
  AbstractMotor::gearset::green,
  {4_in, 11.5_in}
);

AsyncPosIntegratedController lift = AsyncControllerFactory::posIntegrated(9);

AsyncPosIntegratedController rails = AsyncControllerFactory::posIntegrated(2);

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

  lift.setMaxVelocity(30);
  liftTareHeight(1_in);
  moveLift(1_in);

  rails.setMaxVelocity(20);
  rails.tarePosition();

  /**
   * LCD Configuration
   */

 	pros::lcd::initialize();

  pros::lcd::set_text(0, "Configuration");
  set_location_text();
  set_color_text();

	pros::lcd::register_btn0_cb(on_left_button);
  pros::lcd::register_btn2_cb(on_right_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

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
