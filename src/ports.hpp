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
#define DRIVE_LEFT_BACK_PORT 2
#define DRIVE_RIGHT_FRONT_PORT 9
#define DRIVE_RIGHT_BACK_PORT 1
#define LIFT_PORT 15
#define ANGLE_RAILS_PORT 18
#define INTAKE_LEFT_PORT 12
#define INTAKE_RIGHT_PORT 13

// ADI (3-wire)
// What does ADI Stand for? Analog Device Input?
#define RAILS_LIMIT_PORT 'E'
#define POTENTIOMETER_PORT 'F'
// (OUTPUT wire) must be odd (ACEG/1357)
#define SIDE_ULTRA_PING_PORT 'A'
// (INPUT wire) must be 1 more than the ping?
#define SIDE_ULTRA_ECHO_PORT 'B'
// (OUTPUT wire) must be odd (ACEG/1357)
#define LEFT_ULTRA_PING_PORT 'C'
// (INPUT wire) must be 1 more than the ping?
#define LEFT_ULTRA_ECHO_PORT 'D'
// (OUTPUT wire) must be odd (ACEG/1357)
#define RIGHT_ULTRA_PING_PORT 'G'
// (INPUT wire) must be 1 more than the ping?
#define RIGHT_ULTRA_ECHO_PORT 'H'

#endif
