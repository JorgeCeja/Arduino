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

// save old thr values
int oldArmMain = 0, oldArmWrist = 0;

const byte ENABLE_PIN = 11; // checked to be true on the ARM PUCK

// defines pins numbers
const int STEP1 = 10;
const int STEP2 = 9; 
const int STEP3 = 8;
const int STEP4 = 7;
const int STEP_DIR = 12; 

// EasyDriver MS pins
const int MS1 = 4; 
const int MS2 = 5; 
const int MS3 = 6;

int step_speed = 10;  // Speed of Stepper motor (higher = slower)

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

  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  
  // Sets the two pins as Outputs
  pinMode(STEP1,OUTPUT); 
  pinMode(STEP2,OUTPUT);
  pinMode(STEP3,OUTPUT); 
  pinMode(STEP4,OUTPUT);  
  pinMode(STEP_DIR,OUTPUT);

  /* Configure type of Steps on EasyDriver:
  // MS1 MS2
  //
  // LOW LOW = Full Step //
  // HIGH LOW = Half Step //
  // LOW HIGH = A quarter of Step //
  // HIGH HIGH = An eighth of Step //
  */

  digitalWrite(MS1, LOW);      // Configures to Full Steps
  digitalWrite(MS2, LOW);    // Configures to Full Steps
  
  enablePinLow(); // disable sending on enable pin
}

void loop(){

  // receive from master and checking it is our receive 
  byte received = recvMsg (fAvailable, fRead, RovCmdVals, sizeof (RovCmdVals));

if (received){
  
  if (RovCmdVals[0] != ROV_CMD_VAL) 
      return;  // not my device
      
    // Map the values received to THR accepted values
    int ARM_MAIN = mapValue(RovCmdVals[6]);
    int ARM_WRIST = mapValue(RovCmdVals[7]);
    int ARM_GRIP = mapValue(RovCmdVals[8]);
    int ARM_OTHER = mapValue(RovCmdVals[9]);


    // do actions if new thr values don't equal like the previous arm values 
//     if(ARM_MAIN != oldArmMain && ARM_WRIST != oldArmWrist){
       if(true){
         moveStepper(ARM_MAIN, STEP1);
         moveStepper(ARM_WRIST, STEP2);
         moveStepper(ARM_GRIP, STEP3);
         moveStepper(ARM_OTHER, STEP4);
  }
    

    oldArmMain = ARM_MAIN;
    oldArmWrist = ARM_WRIST;
    
    /*
     * Get and set status values to sending array 
     * statses to send: 
     * System Voltage, Heading, Pressure, Temperature, 
     * (calculated) Depth, Leak detector, thr current,  
     */

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

void moveStepper(int reading, int stepperNumber){
  if (reading > 150) {  //  If joystick is moved Left
           digitalWrite(STEP_DIR, LOW);  // (HIGH = anti-clockwise / LOW = clockwise)
           
           digitalWrite(stepperNumber, HIGH);
           delay(step_speed);
           digitalWrite(stepperNumber, LOW);
           delay(step_speed);
    }      
  
   if (reading < 100) {  // If joystick is moved right
          digitalWrite(STEP_DIR, HIGH);  // (HIGH = anti-clockwise / LOW = clockwise)
          
          digitalWrite(stepperNumber, HIGH);
          delay(step_speed);
          digitalWrite(stepperNumber, LOW);
          delay(step_speed); 
   }
  
}

