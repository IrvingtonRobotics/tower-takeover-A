/**
 * Initialization, run before all other code as soon as the proram is started
 */

#include "common.hpp"

#define POT_RED_POS 1612
#define POT_BLUE_POS 1123

/* ---- CONFIG ---- */
// default start settings
bool isRed = true;
bool isSmallSide = true;

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

  drive.setSide(isRed);

  lift.tare();

  rails.tare();

  pros::ADIPotentiometer pot(POTENTIOMETER_PORT);
  int potValue = pot.get_value();
  isRed = abs(potValue - POT_RED_POS) < abs(potValue - POT_BLUE_POS);
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
