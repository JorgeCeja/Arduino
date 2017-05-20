// variables | index numbers
// why define instead of asigning int, etc. 
// ROV Command buffer contents

//#define ROV_CMD_LOC     0
//#define ROV_STATUS_ID    1
//#define THR_PORT_H       2
//#define THR_PORT_V       3
//#define THR_STBD_H       4
//#define THR_STBD_V       5
//#define ARM_MAIN         6
//#define ARM_WRIST        7
//#define ARM_GRIP         8
// #define LED_R            9 //OTHER
// #define LED_G            10
// #define LED_B            11
// #define OUTPUT_CTL_BITS  12

// Thruster Status buffer contents

//#define STAT_THR_LOC     0
//#define STAT_THR_ID      1
//#define STAT_THR_PWM1    2
//#define STAT_THR_PWM2    3
//#define STAT_THR_AMPS1   4
//#define STAT_THR_AMPS2   5
//#define STAT_DEPTH_CM    6
//#define STAT_DEPTH_M     7
//#define STAT_TEMP_01     8
//#define STAT_TEMP_DEG    9
//#define STAT_PRESS_01    10
//#define STAT_PRESS       11
//#define STAT_HEAD_01     12
//#define STAT_HEAD_LOW    13
//#define STAT_HEAD_HIGH   14
//#define STAT_SYS_VOLTS   15
//#define STAT_SYS_VOLT_01 16
//#define STAT_ARM_TILT    17
//#define STAT_THR_LEAK    18
//#define STAT_THR_ERROR   19

// Stepper Status buffer contents

// #define STAT_STP_LOC     0
// #define STAT_STP_ARM     1
// #define STAT_STP_WRIST   2
// #define STAT_STP_GRIP    3
// #define STAT_STP_LEAK    4
// #define STAT_STP_VOLTS   5
// #define STAT_STP_VOLT01  6
// #define STAT_STP_ERROR   7

// Video Status buffer contents

// #define STAT_VID_LOC     0
// #define STAT_VID_FRAME   1
// #define STAT_VID_LEAK    2
// #define STAT_VID_VOLTS   3
// #define STAT_VID_VOLT_01 4
// #define STAT_VID_ERROR   5

// headers for each device
#define ROV_CMD_VAL      99
#define THR_PORT_VAL     22 // left thr puck
#define THR_STBD_VAL     33 // right thr puck
#define STEP_STAT_VAL    44 
#define VID_STAT_VAL     55

// define size for arrays
#define MAX_BFR_SIZE  25 
#define CMD_BFR_SIZE    14 
#define THR_BFR_SIZE     20 
#define STEP_BFR_SIZE    8
#define VID_BFR_SIZE     6

// define array with their respective size
byte RovCmdVals[CMD_BFR_SIZE];
byte StatusThrP [THR_BFR_SIZE];
byte StatusThrS [THR_BFR_SIZE];
byte StatusStep [STEP_BFR_SIZE];
byte StatusVid [VID_BFR_SIZE];
byte StatusBuf [MAX_BFR_SIZE]; 

#include "RS485_protocol.h"
#include <Stepper.h>

const byte ENABLE_PIN = 11; // checked to be true on the ARM PUCK

// defines pins numbers
const byte STEP1 = 10;
const byte STEP2 = 9; 
const byte STEP3 = 8;
const byte STEP4 = 7;
const byte STEP_DIR = 12; 

// EasyDriver MS pins
const int MS1 = 4; 
const int MS2 = 5; 
const int MS3 = 6;

/* Configure type of Steps on EasyDriver:
// MS1 MS2
//
// LOW = 0 -- HIGH = 1 
//
// LOW LOW = Full Step //
// HIGH LOW = Half Step //
// LOW HIGH = A quarter of Step //
// HIGH HIGH = An eighth of Step //
*/
const int ms1_setting = 0; 
const int ms2_setting = 0;
const int ms3_setting = 0;

const int stepSpeed = 585;  // Speed of Stepper motor (higher = slower)

byte ARM_MAIN;
byte ARM_WRIST;
byte ARM_GRIP;
byte ARM_OTHER;

void fWrite (const byte what) {
  Serial.write (what);  
}
    
int fAvailable () {
  return Serial.available ();  
}
  
int fRead () {
  return Serial.read ();  
} 
  
void setup(){
  // communications related setup
  Serial.begin (28800);
  pinMode (ENABLE_PIN, OUTPUT);  // driver output enable
  Serial.setTimeout(100);

  // ms config
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  // ms setting
  digitalWrite(MS1, ms1_setting);    
  digitalWrite(MS2, ms2_setting);
  digitalWrite(MS3, ms3_setting);
  
  // Sets the two pins as Outputs
  pinMode(STEP1,OUTPUT); 
  pinMode(STEP2,OUTPUT);
  pinMode(STEP3,OUTPUT); 
  pinMode(STEP4,OUTPUT);  
  pinMode(STEP_DIR,OUTPUT);

  // Step and direction settings
  // Ensures all stepers start up correctly
  digitalWrite(STEP1, LOW);
  digitalWrite(STEP2, LOW);
  digitalWrite(STEP3, LOW);
  digitalWrite(STEP4, LOW);
  digitalWrite(STEP_DIR, LOW);
  
  enablePinLow(); // disable sending on enable pin
}

void loop(){

  // receive from master and checking it is our receive 
  byte received = recvMsg (fAvailable, fRead, RovCmdVals, sizeof (RovCmdVals));

if (received){
  
  if (RovCmdVals[0] != ROV_CMD_VAL) 
      return;  // not my device
      
    // Map the values received to THR accepted values
    ARM_MAIN = RovCmdVals[6];
    ARM_WRIST = RovCmdVals[7];
    ARM_GRIP = RovCmdVals[8];
    ARM_OTHER = RovCmdVals[9];

        /*
         * Logic to drive one motor at a time
         */
        // Main
        if(ARM_MAIN > ARM_WRIST && ARM_MAIN > ARM_GRIP && ARM_MAIN > ARM_OTHER){
         moveStepper(STEP1, 1);
        }
        if(ARM_MAIN < ARM_WRIST && ARM_MAIN < ARM_GRIP && ARM_MAIN < ARM_OTHER){
         moveStepper(STEP1, 0);
        } 

        // Wrist
        if(ARM_WRIST > ARM_MAIN && ARM_WRIST > ARM_GRIP && ARM_WRIST > ARM_OTHER){
         moveStepper(STEP2, 1);
        }
        if(ARM_WRIST < ARM_MAIN && ARM_WRIST < ARM_GRIP && ARM_WRIST < ARM_OTHER){
         moveStepper(STEP2, 0);
        } 

        // Grip
        if(ARM_GRIP > ARM_MAIN && ARM_GRIP > ARM_WRIST && ARM_GRIP > ARM_OTHER){
         moveStepper(STEP3, 1);
        }
        if(ARM_GRIP < ARM_MAIN && ARM_GRIP < ARM_WRIST && ARM_GRIP < ARM_OTHER){
         moveStepper(STEP3, 0);
        }

        // Other
        if(ARM_OTHER > ARM_MAIN && ARM_OTHER > ARM_WRIST && ARM_OTHER > ARM_GRIP){
         moveStepper(STEP4, 1);
        }
        if(ARM_OTHER < ARM_MAIN && ARM_OTHER < ARM_WRIST && ARM_OTHER < ARM_GRIP){
         moveStepper(STEP4, 0);
        }

    /*
     * Send thr status to Master
     */
    byte StatusThrP [] = {
        44,     
        RovCmdVals[6],      
        RovCmdVals[7],    
        RovCmdVals[8],    
        RovCmdVals[9],     
    };
    
    enablePinHigh(); // enable sending
    
    sendMsg (fWrite, StatusThrP, sizeof StatusThrP);

    waitForBuffer();

   }  // end if something received

    enablePinLow(); // disable sending on enable pin

}  // end of loop

void enablePinLow() {
  // check the pin mapping for details
  //PORTB &= ~_BV(PD4);
  digitalWrite(ENABLE_PIN, LOW);
}

void enablePinHigh() {
  // check the pin mapping for details
  //PORTB |= _BV(PD4);
  digitalWrite(ENABLE_PIN, HIGH);
}

void waitForBuffer() {
  while (!(UCSR0A & (1 << UDRE0))) // Wait for empty transmit buffer
    UCSR0A |= 1 << TXC0; // mark transmission not complete
  while (!(UCSR0A & (1 << TXC0))); // Wait for the transmission to complete
}

int mapValue(int data) {
  // Serial communications come as 0 - 255 (BYTE)
  // Signal value should be between 0 and 100
  return map(data, 0, 255, 0, 255);
}

void moveStepper(int stepPin, int dir){
  for(int i = 0; i < 10; i++){
     digitalWrite(STEP_DIR, dir); // HIGH-1 = anti-clockwise / LOW-0 = clockwise
     digitalWrite(stepPin, HIGH);
     delayMicroseconds(stepSpeed);
     digitalWrite(stepPin, LOW);
     delayMicroseconds(stepSpeed); 
  }
}

