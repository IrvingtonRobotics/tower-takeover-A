/**
 * Opcontrol, run during driver control period (1:45)
 */

#include "common.hpp"
#include "controls.hpp"
#include "utils.hpp"
#include "config.hpp"

const int STANDARD_DELAY = 10;
const RQuantity DOUBLE_PRESS_INTERVAL = 200_ms;
const bool disableDoublePress = true;
const RQuantity OUTLIFT_OUT_TIME = 200_ms;

bool killed = false;
Timer intakePressedTimer = Timer();
Timer outtakePressedTimer = Timer();
bool isContinuousIntake = false;
Timer outliftTimer = Timer();
bool isOutlifting = false;
bool autonActive = false;
/**
 * Run the Drive subsystem based on the joysticks
 */
void runDrive() {
  if (killed) {
    drive.stop();
    return;
  }
  bool armsUp = lift.getCurrentHeight() > 10_in;
  drive.move(DRIVE_X_CONTROL, DRIVE_Y_CONTROL, 1, false, armsUp);
}

/**
 * Task to be initiated in the main task
 * Not DRY at all: all the run*Fn look the same, but how do I generalize this
 * while loop, delay, and signature?
 * TODO (make more DRY): Look at pros::Task and pros::task_fn_t to avoid
 *   code duplication
 */
void runDriveFn(void* param) {
  while (true) {
    runDrive();
    pros::delay(STANDARD_DELAY);
  }
}

/**
 * Run the Lift subsystem
 * Controls intentional movement (small and non-small)
 * Controls lowering to button during opcontrol
 * Controls querying for lift location
 */
void runLift() {
  if (killed) {
    lift.stop();
    return;
  }
  lift.step();
  bool isUp = buttonLiftUp.changedToPressed();
  bool isDown = buttonLiftDown.changedToPressed();
  if (isUp || isDown) {
    lift.move(isUp, false);
  }
  bool isFastUp = buttonGlideUp.changedToPressed();
  bool isFastDown = buttonGlideDown.changedToPressed();
  if (isFastUp || isFastDown) {
    lift.glide(isFastUp);
  }
  if (buttonRetareLift.changedToPressed()) {
    lift.lowerToButton();
  }
  if (outliftTriggered()) {
    // move up ASAP -- hopefully outtake keeps up
    isOutlifting = true;
    outliftTimer = Timer();
    lift.glide(true);
  }
}

void runLiftFn(void* param) {
  while (true) {
    runLift();
    pros::delay(STANDARD_DELAY);
  }
}

/**
 * Run the Rails subsystem
 * Simply toggle between the two positions based on buttons
 */
void runRails() {
  if (killed) {
    rails.stop();
    return;
  }
  rails.step();
  if (buttonRailsToggle.changedToPressed()) {
    rails.togglePosition();
  }
  if (buttonRailsToggleMid.changedToPressed()) {
    rails.togglePositionMid();
  }
  if (buttonTareRails.changedToPressed()) {
    printf("taring rails\n");
    rails.backToButton();
  }
}

void runRailsFn(void* param) {
  while (true) {
    runRails();
    pros::delay(STANDARD_DELAY);
  }
}

/**
 * Run the Intake subsytem.
 * Intake, outtake, or stop based on buttons
 */
void runIntake() {
  if (killed) {
    intake.stop();
    return;
  }
  float speed = 1;
  bool released = buttonRunOuttake.changedToReleased() || buttonRunIntake.changedToReleased();
  if (!disableDoublePress && buttonRunIntake.changedToPressed()) {
    isContinuousIntake = intakePressedTimer.getDtFromMark() < DOUBLE_PRESS_INTERVAL;
  }
  if (!disableDoublePress && buttonRunOuttake.changedToPressed()) {
    isContinuousIntake = outtakePressedTimer.getDtFromMark() < DOUBLE_PRESS_INTERVAL;
  }
  if (buttonRunIntake.isPressed()) {
    intakePressedTimer.placeMark();
    intake.intake(speed);
  } else if (buttonRunOuttake.isPressed()) {
    outtakePressedTimer.placeMark();
    intake.outtake(speed);
  } else if (released && (disableDoublePress || !isContinuousIntake)) {
    intake.stop();
  }
  if (isOutlifting) {
    intake.outtake(0.25);
  }
  if (buttonKill.changedToPressed()) {
    intake.stop();
  }
}

void runIntakeFn(void* param) {
  while (true) {
    runIntake();
    pros::delay(STANDARD_DELAY);
  }
}

/**
 * Initiate all opcontrol subsystem tasks.
 * Each is in its own task, so subsystems cannot stop each other.
 * For thread safety, ensure that no task affects any other subsystem.
 */
void opcontrol() {
  if (MODE == DRIVER) {
    foldout();
  }
  drive.straighten();
  intakePressedTimer.placeMark();
  outtakePressedTimer.placeMark();
  // start all tasks
  pros::Task runDriveTask(runDriveFn);
  pros::Task runLiftTask(runLiftFn);
  pros::Task runRailsTask(runRailsFn);
  pros::Task runIntakeTask(runIntakeFn);
  while (true) {
    // kill if necessary
    if (buttonKill.changedToPressed()) {
      killed = !killed;
    }
    if (isOutlifting && outliftTimer.getDtFromStart() > OUTLIFT_OUT_TIME) {
      isOutlifting = false;
      intake.stop();
    }
    intake.logState();
    pros::delay(10);
  }
}
