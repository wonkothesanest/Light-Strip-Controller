#include <Ultrasonic.h>
/*
 * 
 * 
 * Coding steps
 * define pins
 * turn on transistors to 1/2 power and cycle through each color
 * test
 * add button click register and cycle through colors via click
 * test
 * add range sensor and decide on 0 - 80% (trying not to fry npn)
 * test
 * code up proper user interface
 * 
 */
double maxpower = 0.8;
 
 //LED colors
int pin_colorR = 0;
int pin_colorG = 1;
int pin_colorB = 8;

//Ultrasonic sensor
int pin_ult_trig = 5;
int pin_ult_echo = 7;

//button
int pin_button = 11;

//constants
double max_percent = 0.80;
int colorArr[3] = {pin_colorR, pin_colorG, pin_colorB};



 
void setup() {
  // put your setup code here, to run once:
  pinMode(pin_colorR, OUTPUT);
  pinMode(pin_colorG, OUTPUT);
  pinMode(pin_colorB, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  clearColors();
  analogWritePercent(pin_colorR, 0.5);
  analogWritePercent(pin_colorG, 0.5);
  analogWritePercent(pin_colorB, 0.5);
  delay(500);
  clearColors();
  analogWritePercent(pin_colorR, 0.5);
  delay(500);
  clearColors();
  analogWritePercent(pin_colorG, 0.5);
  delay(500);
  clearColors();
  analogWritePercent(pin_colorB, 0.5);
  delay(500);
  clearColors();
  delay(2000);
  
}


//percent is 0.0 - 1.0
void analogWritePercent(int pin, double percent){
  double dval = (double)percent*maxpower*255.0;
  
  int value = (int)dval;
  analogWrite(pin, value);
}

void clearColors(){
  analogWritePercent(pin_colorR, 0.0);
  analogWritePercent(pin_colorG, 0.0);
  analogWritePercent(pin_colorB, 0.0);
}

