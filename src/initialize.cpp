/**
 * Initialization, run before all other code as soon as the proram is started
 */

// include everything with config attached
#include "config.cpp"

// defaults
bool isRed = true;
bool isBack = true;

// Instantiate subsystem classes
Lift lift = Lift();
Drive drive = Drive();
Rails rails = Rails();
Intake intake = Intake();
Config config = Config();

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

  lift.setMaxVelocity(120);
  lift.tare();

  rails.tare();

  /**
   * LCD Configuration
   */

   config.initialize();
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
