int ledPin = 13;
 int joyPin1 = 14;                 // slider variable connecetd to analog pin 0
 int joyPin2 = 9;                 // slider variable connecetd to analog pin 1
 int value1 = 0;                  // variable to read the value from the analog pin 0
 int value2 = 0;                  // variable to read the value from the analog pin 1

 void setup() {
  pinMode(ledPin, OUTPUT);              // initializes digital pins 0 to 7 as outputs
  Serial.begin(9600);
 }

 int treatValue(int data) {
  // signal value should be between 1100 and 1900
  return map(data, 0, 1023, 1100, 1900);
 }

 void loop() {
  value1 = analogRead(joyPin1);   
  Serial.println(treatValue(value1));
  
  delay(500);             
  
//  value2 = analogRead(joyPin2);   
//  Serial.println(treatValue(value2));
 }
