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
// #define LED_R            9
// #define LED_G            10
// #define LED_B            11
// #define OUTPUT_CTL_BITS  12

// Thruster Status buffer contents

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

// using servo library to controll thrs 
#include <Servo.h>

// Servos
Servo thrusterH;
Servo thrusterV;

byte thr_V_Pin = 6;
byte thr_H_Pin = 5;


// save old thr values
int oldThrVcmd = 0, oldThrHcmd = 0;

const byte ENABLE_PIN = 4; // checked to be true on the THR PUCK


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

  // Thr setup with their respective pins from thr circuit diagram
  thrusterH.attach(thr_V_Pin); //thr1 #5 on the puck
  thrusterH.writeMicroseconds(1500); // send "stop" signal to ESC.
  delay(1000); // delay to allow the ESC to recognize the stopped signal

  
  thrusterV.attach(thr_H_Pin); //thr2 #6 on the puck
  thrusterV.writeMicroseconds(1500); // send "stop" signal to ESC.
  delay(1000); // delay to allow the ESC to recognize the stopped signal

  enablePinLow(); // disable sending on enable pin
}

void loop(){

  // receive from master and checking it is our receive 
  byte received = recvMsg (fAvailable, fRead, RovCmdVals, sizeof (RovCmdVals));

if (received){
  
  if (RovCmdVals[0] != ROV_CMD_VAL) 
      return;  // not my device
      
    // Map the values received to THR accepted values
    int THR_H = mapValue(RovCmdVals[4]);
    int THR_V = mapValue(RovCmdVals[5]);

    /* 
     *  Apply thrP cmds from master THR_PORT_H and THR_PORT_V (index 2 & 3 respectively)
     *  Blue robotics documantation uses writeMicroseconds instead of write 
     *  But we will use write for testing purposes.
     */
    // dont do anything if new thr values equal previous thr values 
     if(THR_V == oldThrVcmd && THR_H == oldThrHcmd){
        thrusterV.writeMicroseconds(THR_V);
        thrusterH.writeMicroseconds(THR_H);
     }

    oldThrVcmd = THR_V;
    oldThrHcmd = THR_H;
    
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
        22,     
        RovCmdVals[2],      
        RovCmdVals[3],    
        RovCmdVals[4],    
        RovCmdVals[5],   
        STAT_THR_AMPS2,   
        STAT_DEPTH_CM,    
        STAT_DEPTH_M,     
        STAT_TEMP_01,     
        STAT_TEMP_DEG,    
        STAT_PRESS_01,    
        STAT_PRESS,       
        STAT_HEAD_01,     
        STAT_HEAD_LOW,    
        STAT_HEAD_HIGH,  
        STAT_SYS_VOLTS,   
        STAT_SYS_VOLT_01, 
        STAT_ARM_TILT,    
        STAT_THR_LEAK,    
        STAT_THR_ERROR   
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
  // Signal value should be between 1100 and 1900
  return map(data, 0, 255, 1100, 1900);
}
