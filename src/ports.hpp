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
#define INTAKE_RIGHT_PORT 11
#define LIFT_PORT 5

// ADI (3-wire)
// What does ADI Stand for? Analog Device Input?
#define BUTTON_LIMIT_PORT 'E'

#endif
