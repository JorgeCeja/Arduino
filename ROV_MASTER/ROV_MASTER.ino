// variables | index numbers
// why define instead of asigning int, etc. 
/*
 * ROV Command buffer contents
 */
byte ROV_CMD_LOC;     
byte ROV_STATUS_ID;    
byte THR_PORT_H;       
byte THR_PORT_V;      
byte THR_STBD_H;       
byte THR_STBD_V;       
byte ARM_MAIN;         
byte ARM_WRIST;        
byte ARM_GRIP;
byte ARM_OTHER;         
byte LED_R;            
byte LED_G;            
byte LED_B;            
byte OUTPUT_CTL_BITS; 
/*
 * Thruster Status buffer contents
 */
// #define STAT_THR_LOC     0
// #define STAT_THR_ID      1
// #define STAT_THR_PWM1    2
// #define STAT_THR_PWM2    3
// #define STAT_THR_AMPS1   4
// #define STAT_THR_AMPS2   5
// #define STAT_DEPTH_CM    6
// #define STAT_DEPTH_M     7
// #define STAT_TEMP_01     8
// #define STAT_TEMP_DEG    9
// #define STAT_PRESS_01    10
// #define STAT_PRESS       11
// #define STAT_HEAD_01     12
// #define STAT_HEAD_LOW    13
// #define STAT_HEAD_HIGH   14
// #define STAT_SYS_VOLTS   15
// #define STAT_SYS_VOLT_01 16
// #define STAT_ARM_TILT    17
// #define STAT_THR_LEAK    18
// #define STAT_THR_ERROR   19
/*
 * Stepper Status buffer contents
 */
// #define STAT_STP_LOC     0
// #define STAT_STP_ARM     1
// #define STAT_STP_WRIST   2
// #define STAT_STP_GRIP    3
// #define STAT_STP_LEAK    4
// #define STAT_STP_VOLTS   5
// #define STAT_STP_VOLT01  6
// #define STAT_STP_ERROR   7
/*
 * Video Status buffer contents
 */
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
byte StatusThrs [THR_BFR_SIZE];
byte StatusStep [STEP_BFR_SIZE];
byte StatusVid [VID_BFR_SIZE];
byte StatusBuf [MAX_BFR_SIZE];
// SainSmart LCD Display Settings
const byte LCD_RS = 8; 
const byte LCD_ENA = 9;
const byte LCD_D4  = 4;
const byte LCD_D5 = 5;
const byte LCD_D6 = 6;
const byte LCD_D7 = 7;

int THR_Y;
int THR_X;
int THR_ROTATE;
int THR_SLIDER;

 //LCD R/W pin to ground
// include the library code:
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(LCD_RS, LCD_ENA, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

#include "RS485_protocol.h"

const byte ENABLE_PIN = 2; // checked to be true on the ROV shield

/*  
 *   Thruster controls (PIN 9-11)
 *   RIGHT CONTROLLER
 */
const byte UP_DOWN_T = 8; // Joystick
const byte LEFT_RIGHT_T = 9; // Joystick
const byte ROTATE_T = 10; // Joystick
const byte SLIDER_T = 11; // Joystick

/*  
 *   ARM controls (PIN 12-15)
 *   LEFT CONTROLLER
 */
const byte UP_DOWN_A = 13; // Joystick
const byte LEFT_RIGHT_A = 12; // Joystick
const byte ROTATE_A = 14; // Joystick
const byte SLIDER_A = 15; // Joystick

void fWrite (const byte what) {
  Serial.write (what);  
}
    
int fAvailable () {
  return Serial.available ();  
}
  
int fRead () {
  return Serial.read ();  
} 
  
void setup()
{  
  // Communications Related Setup
  Serial.begin (28800);
  pinMode (ENABLE_PIN, OUTPUT);  // driver output enable
  Serial.setTimeout(100);
   
  // Start the LCD Display for readings
  lcd.begin(16,2); 
}  // end of setup
  
void loop(){
  // Analog Stick Read

  // THR VALUES FROM RIGHT CONTROLLER
  THR_Y = mapValue(analogRead(UP_DOWN_T));
  THR_X = mapValue(analogRead(LEFT_RIGHT_T));
  THR_ROTATE = mapValue(analogRead(ROTATE_T));
  THR_SLIDER = mapValue(analogRead(SLIDER_T));
  
  // ARM VALUES FROM LEFT CONTROLLER
  ARM_MAIN = mapValue(analogRead(UP_DOWN_A));    
  ARM_WRIST = mapValue(analogRead(LEFT_RIGHT_A));        
  ARM_GRIP = mapValue(analogRead(ROTATE_A));
  ARM_OTHER = mapValue(analogRead(SLIDER_A));

  ARM_MAIN = byte (ARM_MAIN);    // output value for the Port Horizontal
  ARM_WRIST = byte (ARM_WRIST);   // output value for the Port Vertical
  ARM_GRIP = byte (ARM_GRIP);
  ARM_OTHER = byte(ARM_OTHER);

  /*
   * LOGIC TO DRIVE THE THRUSTERS
   */
  // move forward and back 
  THR_PORT_H = THR_Y;
  THR_STBD_H = THR_Y;

  // move up and down
  THR_PORT_V = THR_SLIDER;
  THR_STBD_V = THR_SLIDER;

  // rotate
  if(THR_ROTATE > 127.5){ // right
    THR_STBD_H = THR_ROTATE;
    THR_PORT_H = (255-THR_ROTATE);  // opposite
  } 
  
  else { //left
    THR_STBD_H = (255-THR_ROTATE);  // opposite
    THR_PORT_H = THR_ROTATE;
  }

  // slide
  if(THR_X > 127.5){ // right
    //left goes positive
    THR_PORT_V = THR_X;
    THR_STBD_V = (255-THR_X);  // opposite
  } 

  else{ // left
    THR_PORT_V = (255-THR_X);  // opposite
    THR_STBD_V = THR_X;
  }

  THR_PORT_H  = byte (THR_PORT_H);    // output value for the Port Horizontal
  THR_PORT_V   = byte (THR_PORT_V);   // output value for the Port Vertical
  THR_STBD_H  = byte (THR_STBD_H);    // output value for the Stbd Horizontal
  THR_STBD_V   = byte (THR_STBD_V);   // output value for the Stbd Vertical
  
  // assemble message
  byte RovCmdVals [] = { 
  ROV_CMD_VAL,     
  44,    // this is the header. Change it
  THR_PORT_H,
  THR_PORT_V,     
  THR_STBD_H,       
  THR_STBD_V,      
  ARM_MAIN,         
  ARM_WRIST,        
  ARM_GRIP,
  ARM_OTHER,        
  LED_R,           
  LED_G,            
  LED_B,            
  OUTPUT_CTL_BITS  
  };
  
  // send to slave  
  enablePinHigh();  // enable sending
  
  sendMsg (fWrite, RovCmdVals, sizeof RovCmdVals);

  waitForBuffer();

  enablePinLow();  // disable sending
  
  // receive response  
  byte received = recvMsg (fAvailable, fRead, StatusBuf, MAX_BFR_SIZE);

  displayStatus(received);
    
}  // end of loop

void enablePinLow() {
//  PORTE &= ~_BV(PE4);
digitalWrite(ENABLE_PIN, LOW);
}

void enablePinHigh() {
//  PORTE |= _BV(PE4);
digitalWrite(ENABLE_PIN, HIGH);
}

void waitForBuffer() {
  while (!(UCSR0A & (1 << UDRE0))) // Wait for empty transmit buffer
    UCSR0A |= 1 << TXC0; // mark transmission not complete
  while (!(UCSR0A & (1 << TXC0))); // Wait for the transmission to complete
}

int mapValue(int data) {
 // signal value should be between 1100 and 1900
// return map(data, 0, 1023, 1100, 1900);
// Serial communications should be between 0 and 255 (BYTE)
return map(data, 0, 1023, 0, 255);
}

void displayStatus(byte received){
  if (received == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error!");
    
    // DEBUGING PURPOSES
    lcd.setCursor(0,1);
    lcd.print("PLACE HOLDER");
  }  // end of if
    
  else {
    lcd.clear();
    
    switch(StatusBuf[0]){
      // ARM STATUS
      case 44:
        lcd.setCursor(0, 0);
        lcd.print(ARM_MAIN);
        lcd.print(":");
        lcd.print(ARM_WRIST);
        lcd.setCursor(0,1);
        lcd.print(StatusBuf[1]);
        lcd.print(":");
        lcd.print(StatusBuf[2]);
        break;
      // THR PORT STATUS
      case 22:
        lcd.setCursor(0, 0);
        lcd.print(THR_PORT_H);
        lcd.print(":");
        lcd.print(THR_PORT_V);
        lcd.print("  ");
        lcd.print(THR_STBD_H);
        lcd.print(":");
        lcd.print(THR_STBD_V);
        lcd.setCursor(0,1);
        lcd.print(StatusBuf[1]);
        lcd.print(":");
        lcd.print(StatusBuf[2]);
        lcd.print("  ");
        lcd.print(StatusBuf[3]);
        lcd.print(":");
        lcd.print(StatusBuf[4]);
        break;
      // THR STBD STATUS
      case 33:
        lcd.setCursor(0, 0);
        lcd.print(THR_STBD_H);
        lcd.print(":");
        lcd.print(THR_STBD_V);
        lcd.setCursor(0,1);
        lcd.print((StatusBuf[14]<<8)|StatusBuf[13]);
        lcd.print(":");
        lcd.print(StatusBuf[12]);
        break;
      default :
        lcd.print("NOTHING");
      }  // end of switch
    }  // end of else
    
}  // end of function

