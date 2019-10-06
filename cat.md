## Code Printout
`src/initialize.cpp`
```cpp
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
```
- - - - -
`src/common.hpp`
```cpp
/**
 * Common definitions shared between initialize, autonomous, and opcontrol
 * Includes and makes instances of each subsystem
 */


#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#include "main.h"
#include "lift.cpp"
#include "drive.cpp"
#include "rails.cpp"
#include "intake.cpp"

extern Lift lift;
extern Drive drive;
extern Rails rails;
extern Intake intake;

extern bool isRed;
extern bool isBack;

#endif
```
- - - - -
`src/autonomous.cpp`
```cpp
/**
 * Autonomous, run during autonomous period (0:15).
 * Each subsystem is not split into tasks because autonomous does one at a time.
 * If you want to multitask autonomous, put the tasks inside the subsystem
 * classes so they initialize when the subsystem classes
 *
 * All of this code assumes blue side and gets automatically flipped
 */

#include "common.hpp"

// set this to true (for setup) to fold in instead of fold out
// use pros upload --slot 2
const bool isFoldin = false;

/**
 * Run foldout at beginning of match
 */
void foldout() {
  printf("fold out\n");
  // home rails while lifting
  rails.backToButton();
  // foldout
  lift.move(27_in);
  intake.outtake();
  lift.waitUntilSettled();
  // return
  lift.move(0);
  intake.stop();
  drive.moveDistance(3.5_in);
  drive.moveDistance(-3.5_in);
}

void foldin() {
  printf("fold in\n");
  // lift.lowerToButton(120);
  lift.move(12_in);
  lift.waitUntilSettled();
  rails.backToButton();
  intake.intake();
  pros::delay(3000);
  intake.stop();
  lift.lowerToButton(160);
}


void travelProfile(std::initializer_list<okapi::Point> iwaypoints,
  bool backwards, float speed
) {
  // hopefully little overhead here
  auto profileController = drive.getProfileController(speed);
  string name = "current";
  profileController.generatePath(iwaypoints, name);
  profileController.setTarget(name, backwards);
  profileController.waitUntilSettled();
  profileController.removePath(name);
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  if (isFoldin) {
    foldin();
    return;
  }
  foldout();
  pros::delay(100);
  // drive forward and suck
  intake.move(600);
  travelProfile({
    Point{11.0_in, -116.0_in, 0_deg},
    Point{49.0_in, -116.0_in, 0_deg}
  }, false, 0.3);
  intake.stop();
  // drive backward and turn
  travelProfile({
    Point{49.0_in, 116.0_in, 0_deg},
    Point{31.5_in, 91.5_in, 90.0_deg}
  }, true, 0.45);
  // drive forward to endzone
  travelProfile({
    Point{31.5_in, -91.5_in, -90.0_deg},
    Point{19.14215_in, -121.728_in, -135.0_deg}
  }, false, 0.35);
  // release stack
  intake.move(-40);
  rails.moveForward(150);
  rails.waitUntilSettled();
  intake.stop();
  // backup
  travelProfile({
    Point{19.14215_in, 121.728_in, 135.0_deg},
    Point{23.3137_in, 117.8135_in, 135.0_deg}
  }, true, 1.0);
  // push cube into endzone
  // drive.moveDistance(20_in);
  // drive.turnAngle(-10_deg);
  // drive.moveDistance(5_in);
  // drive.moveDistance(-10_in);
}
```
- - - - -
`src/opcontrol.cpp`
```cpp
/**
 * Opcontrol, run during driver control period (1:45)
 */

#include "common.hpp"
#include "controls.hpp"
#include "utils.hpp"

const int STANDARD_DELAY = 10;
const RQuantity DOUBLE_PRESS_INTERVAL = 200_ms;
const bool disableDoublePress = true;

bool killed = false;

bool slowToggleActive = false;
bool slow = false;
/**
 * Run the Drive subsystem based on the joysticks
 */
void runDrive() {
  if (killed) {
    drive.stop();
    return;
  }
  drive.move(DRIVE_X_CONTROL, DRIVE_Y_CONTROL, slow ? 0.25 : 1, slow);
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
  lift.checkTare();
  bool smallUp = buttonLiftSmallUp.isPressed();
  bool up = buttonLiftUp.changedToPressed();
  bool smallDown = buttonLiftSmallDown.isPressed();
  bool down = buttonLiftDown.changedToPressed();
  bool isIncrease = smallUp || up;
  bool isSmall = smallUp || smallDown;
  if (smallUp || up || smallDown || down) {
    // printf("moving %d %d\n", isIncrease, isSmall);
    lift.move(isIncrease, isSmall);
  }
  if (buttonRetareLift.changedToPressed()) {
    lift.lowerToButton();
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

Timer intakePressedTimer = Timer();
Timer outtakePressedTimer = Timer();
bool isContinuousIntake = false;

/**
 * Run the Intake subsytem.
 * Intake, outtake, or stop based on buttons
 */
void runIntake() {
  if (killed) {
    intake.stop();
    return;
  }
  float speed = slow ? 0.25 : 1;
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
  } else if (disableDoublePress || !isContinuousIntake) {
    intake.stop();
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
    // slow if necessary
    if (slowDistance() > 0.3) {
      if (!slowToggleActive) {
        slow = !slow;
      }
      slowToggleActive = true;
    } else {
      slowToggleActive = false;
    }
    pros::delay(10);
  }
}
```
- - - - -
`src/controls.hpp`
```cpp
/**
 * Definitions of joystick controls and controller button mappings
 */

#ifndef SRC_CONTROLS_H_
#define SRC_CONTROLS_H_

ControllerButton buttonLiftUp(ControllerDigital::up);
ControllerButton buttonLiftDown(ControllerDigital::down);
ControllerButton buttonLiftSmallUp(ControllerDigital::right);
ControllerButton buttonLiftSmallDown(ControllerDigital::left);
ControllerButton buttonRailsToggle(ControllerDigital::R1);
ControllerButton buttonRailsToggleMid(ControllerDigital::R2);
ControllerButton buttonRetareLift(ControllerDigital::B);
ControllerButton buttonKill(ControllerDigital::X);
ControllerButton buttonTareRails(ControllerDigital::Y);
ControllerButton buttonRunIntake(ControllerDigital::L1);
ControllerButton buttonRunOuttake(ControllerDigital::L2);
Controller masterController;

// Macros expanded to get X and Y joystick values
#define DRIVE_X_CONTROL masterController.getAnalog(ControllerAnalog::rightX)
#define DRIVE_Y_CONTROL masterController.getAnalog(ControllerAnalog::rightY)

float slowDistance() {
  float x = masterController.getAnalog(ControllerAnalog::leftX);
  float y = masterController.getAnalog(ControllerAnalog::leftY);
  return x*x + y*y;
}

#endif
```
- - - - -
`src/config.cpp`
```cpp
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
```
- - - - -
`src/ports.hpp`
```cpp
/**
 * All port definitions
 *
 * Use a negative port number to manually reverse the drive. However, this
 * would often be more appropriate to place inside the controller class code
 */

#ifndef SRC_PORTS_H_
#define SRC_PORTS_H_

// motor ports
#define DRIVE_LEFT_FRONT_PORT 10
#define DRIVE_LEFT_BACK_PORT 9
#define DRIVE_RIGHT_FRONT_PORT 1
#define DRIVE_RIGHT_BACK_PORT 2
#define ANGLE_RAILS_PORT 8
#define INTAKE_LEFT_PORT 7
#define INTAKE_RIGHT_PORT 20
#define LIFT_PORT 5

// ADI (3-wire)
// What does ADI Stand for? Analog Device Input?
#define LIFT_LIMIT_PORT 'E'
#define RAILS_LIMIT_PORT 'B'

#endif
```
- - - - -
`src/drive.cpp`
```cpp
#include "main.h"
#include "ports.hpp"

#define controller (*controllerPtr)

/**
 * Main class for the wheeled driving subsystem
 * Wraps around a ChassisControllerIntegrated with limited acceleration
 */
class Drive {
  float leftDriveSpeed = 0;
  float rightDriveSpeed = 0;
  const float MAX_DRIVE_ACCEL = 0.01;
  const float MAX_DRIVE_DECEL = 0.05;
  const float TURN_LIMIT_THRESHOLD = 1.20;
  const float TURN_LIMIT_SCALE = 0.85;
  ChassisControllerIntegrated controllerStraight = getController(false);
  ChassisControllerIntegrated controllerReversed = getController(true);
  ChassisControllerIntegrated* controllerPtr = &controllerStraight;

  ChassisControllerIntegrated controllerBack = ChassisControllerFactory::create(
    DRIVE_LEFT_BACK_PORT,
    -DRIVE_RIGHT_BACK_PORT,
    AbstractMotor::gearset::green,
    {4_in, 14.5_in}
  );

  ChassisControllerIntegrated getController(bool isReversed) {
    if (isReversed) {
      return ChassisControllerFactory::create(
        {-DRIVE_RIGHT_FRONT_PORT, -DRIVE_RIGHT_BACK_PORT},
        {DRIVE_LEFT_FRONT_PORT, DRIVE_LEFT_BACK_PORT},
        AbstractMotor::gearset::green,
        {4_in, 14.5_in}
      );
    } else {
      return ChassisControllerFactory::create(
        {DRIVE_LEFT_FRONT_PORT, DRIVE_LEFT_BACK_PORT},
        {-DRIVE_RIGHT_FRONT_PORT, -DRIVE_RIGHT_BACK_PORT},
        AbstractMotor::gearset::green,
        {4_in, 14.5_in}
      );
    }
  }

  /**
   * Get new drive speed after one tick for one side of the robot,
   * taking into account acceleration limits
   */
  float getNewDriveSpeed(float current, float input) {
    // tried using clamp, but difficulties
    float lo;
    float hi;
    if (current >= 0) {
      lo = current - MAX_DRIVE_DECEL;
      hi = current + MAX_DRIVE_ACCEL;
    } else {
      lo = current - MAX_DRIVE_ACCEL;
      hi = current + MAX_DRIVE_DECEL;
    }
    if (lo > input) return lo;
    if (hi < input) return hi;
    return input;
  }

  void moveTank(float left, float right) {
    moveTank(left, right, false);
  }

  void moveTank(float left, float right, bool stopFront) {
    // controller.flipDisable(false);
    // controllerBack.flipDisable(true);
    if (!stopFront) {
      controllerBack.tank(0, 0);
      controller.tank(leftDriveSpeed, rightDriveSpeed);
    } else {
      controller.tank(0, 0);
      controllerBack.tank(leftDriveSpeed, rightDriveSpeed);
    }
  }

public:
  // Drive() {
  //   controllerBack.flipDisable(true);
  // }

  /**
   * Arcade drive based on controllerX and controllerY
   * Pass through controller.tank to use existing acceleration limit code
   */
  float move(float controllerX, float controllerY, float scl, bool stopFront) {
    // compute tank left and right based on arcade x and y
    float left = controllerY + controllerX;
    float right = controllerY - controllerX;
    left *= scl;
    right *= scl;
    leftDriveSpeed = getNewDriveSpeed(leftDriveSpeed, left);
    rightDriveSpeed = getNewDriveSpeed(rightDriveSpeed, right);
    float turnRate = abs(leftDriveSpeed - rightDriveSpeed);
    if (turnRate > TURN_LIMIT_THRESHOLD) {
      leftDriveSpeed *= TURN_LIMIT_SCALE;
      rightDriveSpeed *= TURN_LIMIT_SCALE;
    }
    moveTank(leftDriveSpeed, rightDriveSpeed, stopFront);
  }

  /**
   * Synchronous
   *
   * Move a specific distance.
   * No acceleration limits are applied, so this might cause chain to slip
   */
  void moveDistance(QLength length) {
    // might click
    controller.moveDistance(length);
  }

  void stop() {
    moveTank(0, 0);
  }

  /**
   * Turn clockwise some angle, use negative to turn counterclockwise
   */
  void turnAngle(QAngle angle) {
    controller.turnAngle(angle);
  }

  /**
   * Set side to reverse angles
   */
  void setSide(bool isRed) {
    if (isRed) {
      controllerPtr = &controllerStraight;
    } else {
      controllerPtr = &controllerReversed;
    }
  }

  /**
   * Reset to blue
   */
   void straighten() {
     setSide(false);
   }

   auto getProfileController(float speed) {
     return AsyncControllerFactory::motionProfile(
       1.5 * speed,
       4.0 * speed,
       10.0 * speed,
       controller
     );
   }
};

#undef controller
```
- - - - -
`src/lift.cpp`
```cpp
#include "main.h"
#include "ports.hpp"

/**
 * Main class for the Lift subsystem.
 * Most complicated subsytem controller of them all.
 * Wraps around both AsyncPosIntegratedController and AsyncVelIntegratedController
 * to provide movement to precise locations and smooth movement when lowering
 * to the button/limit switch
 *
 * TODO: Use raw pros::MOTOR to utilize both velocity and position controls
 * Maybe try Gyro (do we have one?)
 *   https://www.robotc.net/wikiarchive/VEX2_Sensors_Overview
 * This'll dencessitate thorough calibration
 */
class Lift {
  const QLength ARM_LENGTH = 22_in;
  // height of arm pivot above ground
  const QLength ARM_ELEVATION = 16_in;
  // 1800 ticks/rev with 36:1 gears -- high torque
  // 900 ticks/rev with 18:1 gears
  // 300 ticks/rev with 6:1 gears -- high speed
  const double ARBITRARY_TICKS_FACTOR = 1.8;
  const int ticksPerRev = 1800 * ARBITRARY_TICKS_FACTOR;
  // height the arm caps out at
  const QLength MAX_ARM_HEIGHT = 31_in;
  // height the arm starts at
  const QLength MIN_ARM_HEIGHT = 2.5_in;
  // tolerance of position when calculating new targets
  const QLength POS_TOLERANCE = 0.5_in;
  double tareTicks = 0;
  // size of opcontrol small movements
  const QLength SMALL_MOVE_SIZE = 0.5_in;
  // just leave this very small
  const QLength MIN_THRESHOLD = -1000_in;
  // threshold until the target is reached for opcontrol small movements
  QLength smallMoveThreshold;
  // tolerance when doing small movements: just reach within this of the threshold
  const QLength SMALL_MOVE_TOLERANCE = 0.2_in;
  // direction currently (or last) moved in for small movements
  int smallMoveDir = 1;
  static const int NUM_HEIGHTS = 4;
  // give up on lowering to button after this time if not hit limit switch
  const QTime LOWER_TO_BUTTON_TIMEOUT = 5_s;
  const int DEFEAULT_LOWER_SPEED = 30;
  // WARNING: targetHeights MUST be sorted
  const QLength targetHeights[NUM_HEIGHTS] = {MIN_ARM_HEIGHT, 16_in, 24.5_in, MAX_ARM_HEIGHT};
  const int PORT = -LIFT_PORT;
  // is this currently doing a hard stop?
  bool stopping = false;
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated(PORT);
  AsyncVelIntegratedController velController =
    AsyncControllerFactory::velIntegrated(PORT);
  ADIButton buttonLimit = ADIButton(LIFT_LIMIT_PORT);

  /**
   * @param QLength height from ground
   * @returns ticks from center
   */
  double getTicks(QLength height) {
    QLength dy = height - ARM_ELEVATION;
    double ratio = (dy / ARM_LENGTH).getValue();
    // printf("Height sin: %f\n", ratio);
    // radians
    double angle = asin(ratio);
    return getTicks(angle);
  }

  /**
   * @param radians angle delta
   * @return ticks delta
   */
  double getTicks(double angle) {
    double revolutions = angle / PI / 2;
    return revolutions * ticksPerRev;
  }

  /**
   * @param ticks from center
   * @returns radian angle from center
   */
  double getAngle(double taredTicks) {
    double ticks = taredTicks;
    double revolutions = ticks / ticksPerRev;
    // radians
    double angle = revolutions * PI * 2;
    return angle;
  }

  /**
   * @param ticks from center
   * @returns QLength height from ground
   */
  QLength getHeight(double taredTicks) {
    double angle = getAngle(taredTicks);
    double ratio = sin(angle);
    QLength dy = ratio * ARM_LENGTH;
    QLength height = dy + ARM_ELEVATION;
    return height;
  }

  /**
   * To help opcontrol
   * If isIncreaseSmall, adds a small bit in the direction isIncrease
   * Else returns next preset in the direction isIncrease at least
   *  POS_TOLERANCE away
   */
  QLength getChangedHeight(QLength lastHeight, bool isIncrease, bool isIncreaseSmall) {
    printf("---------\n");
    printf("  getChangedHeight(%f, %d, %d)  \n", lastHeight / 0.0254,
      isIncrease?1:0, isIncreaseSmall?1:0);
    printf("---------\n");
    // printf("Last height %f\n", lastHeight.getValue());
    int m = boolToSign(isIncrease);
    if (isIncreaseSmall) {
      // just move a bit in one direction
      smallMoveDir = m;
      QLength smallMoveTarget = lastHeight + m * SMALL_MOVE_SIZE;
      smallMoveThreshold = smallMoveTarget * m - SMALL_MOVE_TOLERANCE;
      return smallMoveTarget;
    } else {
      /*
        increasing: find first one greater than current position
        decreasing: find last one less than current position
        switching isIncrease just switches direction and comparison type
      */
      int i = isIncrease ? 0 : NUM_HEIGHTS - 1;
      while (lastHeight * m > targetHeights[i] * m - POS_TOLERANCE) {
        // printf("i %d\n", i);
        i += m;
      }
      i = std::clamp(i, 0, NUM_HEIGHTS - 1);
      printf("clamped i %d\n", i);
      printf("returning ... %f\n", targetHeights[i] / 0.0254);
      return targetHeights[i];
    }
  }

  /**
   * Switch control between position controller and velocity controller
   */
  void flipDisable() {
    controller.flipDisable();
    velController.flipDisable();
  }

public:
  Lift() {
    // turn off velController so it doesn't conflict with posController
    velController.flipDisable();
  }

  void checkTare() {
    if(buttonLimit.isPressed()) {
      tare();
    }
  }

  void tare() {
    tareHeight(MIN_ARM_HEIGHT);
  }

  /**
   * Retare by assuming the current position is at height height
   */
  void tareHeight(QLength height) {
    // calculates tareTicks as ticks from center
    // tare to 0
    controller.tarePosition();
    // printf("Think ticks: %f\n", controller.getTarget());
    // assume lift is height off ground
    // printf("Taring arm to height %f\n", height.getValue());
    // printf("Think ticks: %f\n", controller.getTarget());
    tareTicks = getTicks(height);
    // printf("Height ticks: %f\n", getTicks(height));
    // printf("Think height:%f\n", getHeight(getCurrentTicks()));
    // printf("Tare ticks: %f\n", tareTicks);
  }

  void move(int heightIndex) {
    // printf("Moving lift to height %d\n", heightIndex);
    QLength targetHeight = targetHeights[heightIndex];
    move(targetHeight);
  }

  /**
   * @param ticks from center
   */
  void move(double ticks) {
    double taredTicks = ticks - tareTicks;
    controller.setTarget(taredTicks);
  }

  /**
   * Moves, but height is limited between MIN_ARM_HEIGHT and MAX_ARM_HEIGHT
   */
  void move(QLength height) {
    // printf("tare ticks %f\n", tareTicks);
    QLength clampedHeight = std::clamp(height, MIN_ARM_HEIGHT, MAX_ARM_HEIGHT);
    // printf("Clamped height from %f to %f\n", height.getValue(), clampedHeight.getValue());
    double targetTicks = getTicks(clampedHeight);
    // printf("New target ticks %f\n", taredTicks);
    move(targetTicks);
  }

  /**
   * Opcontrol calls this based on button inputs
   *
   * @param isIncrease whether or not the height increases
   * @param isSmall whether to move in small increment or to next preset
   */

  void move(bool isIncrease, bool isSmall) {
    // printf("\n");
    // printf("Button limit status: %d\n", buttonLimit.isPressed() ? 1 : 0);
    // printf("Moving lift +%d\n", boolToSign(isIncrease) * (isSmall ? 1 : 20));
    // printf("Last height %f\n", lastTargetHeight.getValue());
    if (!isSmall) {
      smallMoveThreshold = MIN_THRESHOLD;
    }
    QLength height = getCurrentHeight();
    if (height * smallMoveDir >= smallMoveThreshold) {
      QLength newHeight = getChangedHeight(getCurrentHeight (), isIncrease, isSmall);
      move(newHeight);
    }
  }

  /**
   * @returns ticks from center
   */
  float getCurrentTicks() {
    // return controller.getTarget() + tareTicks;
    return getTargetTicks() - controller.getError();
  }

  /**
   * @returns height from ground
   */
  QLength getCurrentHeight() {
    // height off ground
    return getHeight(getCurrentTicks());
  }

  /**
   * @returns ticks from center
   */
  float getTargetTicks() {
    return controller.getTarget() + tareTicks;
  }

  /**
   * @returns height from ground
   */
  QLength getTargetHeight() {
    return getHeight(getTargetTicks());
  }

  void setMaxVelocity(double tps) {
    controller.setMaxVelocity(tps);
  }

  /**
   * BLOCKING: Completely locks out rest of program while lowering
   * Should only be run at the beginning -- autonomous should never forget
   * position, and opcontrol can just non-small movement down
   * Can be transferred to state variable (isCurrentlyLowering) but I'm leaving
   * as-is because completely stopping ensures precision and is a feature, not a
   * bug
   */
  void lowerToButton(int speed) {
    // move control to vel for smooth movement
    flipDisable();
    velController.setTarget(-abs(speed));
    Timer timeoutTimer = Timer();
    // delay whole code
    while(!buttonLimit.isPressed() && timeoutTimer.getDtFromStart() < LOWER_TO_BUTTON_TIMEOUT) {
      if (stopping) {
        stopping = !stopping;
        return;
      }
      pros::delay(10);
    }
    velController.setTarget(0);
    // hand control back to pos
    flipDisable();
    tare();
    // avoid the controller resuming to its previous location
    controller.setTarget(0);
  }

  void lowerToButton() {
    lowerToButton(DEFEAULT_LOWER_SPEED);
  }

  /**
   * Get a quick data readout of where the lift thinks it is
   * Useful for calibration and debugging
   */
  void query() {
    printf("---------\n");
    printf("  query  \n");
    printf("---------\n");
    double ticks = getCurrentTicks();
    printf("ticks %f\n", ticks);
    QLength height = getHeight(ticks);
    printf("Height: %f inches\n", height.getValue() * 1 / 0.0254);
    double angle = getAngle(ticks);
    printf("Height angle: %f degrees\n", angle * 180 / 3.14);
    printf("---------\n");
  }

  /**
   * [Deprecated]
   * Move the lift 90 degrees up from its current position without regard
   * for apparent height limits
   * Used in debugging (Please don't)
   */
  void lift90() {
    printf("---------\n");
    printf("  lift90  \n");
    printf("---------\n");
    double currentTicks = getCurrentTicks();
    printf("current %f\n", currentTicks);
    double nextTicks = currentTicks + getTicks(PI / 2);
    printf("next %f\n", nextTicks);
    controller.setTarget(nextTicks);
    printf("---------\n");
  }

  void waitUntilSettled() {
    controller.waitUntilSettled();
  }

  void stop() {
    move(getCurrentTicks());
    stopping = true;
  }
};
```
- - - - -
`src/intake.cpp`
```cpp
#include "main.h"
#include "ports.hpp"

/**
 * Main class for the Intake subsystem
 * Thinly wraps around AsyncVelIntegratedController
 **/
class Intake {
  double SPEED = 100;
  AsyncVelIntegratedController controller =
    AsyncControllerFactory::velIntegrated({INTAKE_LEFT_PORT, -INTAKE_RIGHT_PORT});

public:
  void move(double speed) {
    controller.setTarget(speed);
  }

  void intake() {
    intake(1);
  }

  void intake(float scl) {
    move(SPEED * scl);
  }

  void outtake() {
    outtake(1);
  }

  void outtake(float scl) {
    move(-SPEED * scl);
  }

  void stop() {
    move(0);
  }
};
```
- - - - -
`src/rails.cpp`
```cpp
#include "main.h"
#include "ports.hpp"

/**
 * Main class for the Rails angling subsystem
 * Wraps around AsyncPosIntegratedController and AsyncVelIntegratedController
 * in the manner of Lift (some of the code is transferred)
 * Need a limit because the rails can now start in the middle for space concerns
 **/
class Rails {
  const double RAILS_BACK_TICKS = 0;
  const double RAILS_FORWARD_TICKS = 1400;
  const double RAILS_MIDPOINT_TICKS = (RAILS_BACK_TICKS + RAILS_FORWARD_TICKS) / 2;
  const double RAILS_MID_TICKS = 650;
  const double MOVE_BACK_SPEED = 45;
  const double RAILS_MID_MIDPOINT_TICKS = (RAILS_BACK_TICKS + RAILS_MID_TICKS) / 2;
  const double MOVE_FORWARD_SPEED = 30;
  const double MOVE_MID_SPEED = 60;
  const QTime BACK_TO_BUTTON_TIMEOUT = 5_s;
  const int DEFAULT_BACK_SPEED = 40;
  const int PORT = ANGLE_RAILS_PORT;
  bool stopping = false;
  AsyncPosIntegratedController controller =
    AsyncControllerFactory::posIntegrated(PORT);
  AsyncVelIntegratedController velController =
    AsyncControllerFactory::velIntegrated(PORT);
  ADIButton buttonLimit = ADIButton(RAILS_LIMIT_PORT);


  void move(double ticks) {
    printf("Moving rails to %f ticks\n", ticks);
    controller.setTarget(ticks);
  }

  void move(double ticks, double maxSpeed) {
    setMaxVelocity(maxSpeed);
    move(ticks);
  }

  void setMaxVelocity(double ticks) {
    controller.setMaxVelocity(ticks);
  }

  /**
   * Switch control between position controller and velocity controller
   */
  void flipDisable() {
    controller.flipDisable();
    velController.flipDisable();
  }

  bool isBack() {
    return getTargetTicks() < RAILS_MIDPOINT_TICKS;
  }

  bool isBackMid() {
    return getTargetTicks() < RAILS_MID_MIDPOINT_TICKS;
  }

public:
  Rails() {
    velController.flipDisable();
    setMaxVelocity(30);
  }

  float getTargetTicks() {
    return controller.getTarget();
  }

  void moveBack() {
    moveBack(MOVE_BACK_SPEED);
  }


  void moveBack(double maxSpeed) {
    move(RAILS_BACK_TICKS, maxSpeed);
  }

  void moveMid() {
    moveMid(MOVE_MID_SPEED);
  }

  void moveMid(double maxSpeed) {
    move(RAILS_MID_TICKS, maxSpeed);
  }

  void moveForward() {
    moveForward(MOVE_FORWARD_SPEED);
  }

  void moveForward(double maxSpeed) {
    moveForward(1, maxSpeed);
  }

  void moveForward(double weight, double maxSpeed) {
    double target = weight * RAILS_FORWARD_TICKS + (1 - weight) * RAILS_BACK_TICKS;
    move(target, maxSpeed);
  }

  void move(bool _isBack) {
    if (_isBack) {
      moveBack();
    } else {
      moveForward();
    }
  }

  void moveMid(bool _isBack) {
    if (_isBack) {
      moveBack();
    } else {
      moveMid();
    }
  }

  /**
   * BLOCKING
   */
  void backToButton(int speed) {
    // move control to vel for smooth movement
    flipDisable();
    velController.setTarget(-abs(speed));
    Timer timeoutTimer = Timer();
    // delay whole code
    while(!buttonLimit.isPressed() && timeoutTimer.getDtFromStart() < BACK_TO_BUTTON_TIMEOUT) {
      if (stopping) {
        stopping = false;
        return;
      }
      pros::delay(10);
    }
    velController.setTarget(0);
    // hand control back to pos
    flipDisable();
    tare();
    // avoid the controller resuming to its previous location
    controller.setTarget(0);
  }

  void backToButton() {
    backToButton(DEFAULT_BACK_SPEED);
  }

  /**
   * Switch between forward and backward
   * Assume rails have not moved and remain being isBackdrive
   */
  void togglePosition() {
    move(!isBack());
  }

  void togglePositionMid() {
    moveMid(!isBackMid());
  }

  /**
   * Retare by assuming current position is at ticks 0
   */
  void tare() {
    controller.tarePosition();
  }

  void waitUntilSettled() {
    controller.waitUntilSettled();
  }

  float getCurrentTicks() {
    // return controller.getTarget() + tareTicks;
    return getTargetTicks() - controller.getError();
  }

  void stop() {
    move(getCurrentTicks());
    stopping = true;
  }
};
```
- - - - -
