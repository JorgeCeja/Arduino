# Variables | Index Numbers
## ROV Command buffer contents

#define ROV_CMD_LOC     0
#define ROV_STATUS_ID    1
#define THR_PORT_H       2
#define THR_PORT_V       3
#define THR_STBD_H       4
#define THR_STBD_V       5
#define ARM_MAIN         6
#define ARM_WRIST        7
#define ARM_GRIP         8
#define ARM_OTHER        9
#define LED_R            10
#define LED_G            11
#define LED_B            12
#define OUTPUT_CTL_BITS  13

## Thruster Status buffer contents

#define STAT_THR_LOC     0
#define STAT_THR_ID      1
#define STAT_THR_PWM1    2
#define STAT_THR_PWM2    3
#define STAT_THR_AMPS1   4
#define STAT_THR_AMPS2   5
#define STAT_DEPTH_CM    6
#define STAT_DEPTH_M     7
#define STAT_TEMP_01     8
#define STAT_TEMP_DEG    9
#define STAT_PRESS_01    10
#define STAT_PRESS       11
#define STAT_HEAD_01     12
#define STAT_HEAD_LOW    13
#define STAT_HEAD_HIGH   14
#define STAT_SYS_VOLTS   15
#define STAT_SYS_VOLT_01 16
#define STAT_ARM_TILT    17
#define STAT_THR_LEAK    18
#define STAT_THR_ERROR   19

## Stepper Status buffer contents

#define STAT_STP_LOC     0
#define STAT_STP_ARM     1
#define STAT_STP_WRIST   2
#define STAT_STP_GRIP    3
#define STAT_STP_LEAK    4
#define STAT_STP_VOLTS   5
#define STAT_STP_VOLT01  6
#define STAT_STP_ERROR   7

## Video Status buffer contents

#define STAT_VID_LOC     0
#define STAT_VID_FRAME   1
#define STAT_VID_LEAK    2
#define STAT_VID_VOLTS   3
#define STAT_VID_VOLT_01 4
#define STAT_VID_ERROR   5

## headers for each device
#define ROV_CMD_VAL      99
#define THR_PORT_VAL     22 // left thr puck
#define THR_STBD_VAL     33 // right thr puck
#define STEP_STAT_VAL    44
#define VID_STAT_VAL     55

## define size for arrays
#define MAX_BFR_SIZE  25
#define CMD_BFR_SIZE    14
#define THR_BFR_SIZE     20
#define STEP_BFR_SIZE    8
#define VID_BFR_SIZE     6
