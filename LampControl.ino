//http://playground.arduino.cc/Code/Bounce
#include <Bounce2.h>

//https://github.com/hemalchevli/ultrasonic-library
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

//Introducing Enums! using a mode enumeration so that later we can put in 
// more logic hear for pretty light shows

enum LightModes {
  red,
  green,
  blue
} light_modes;

//a constant array to declare states of light display
//  this can be all of the LightModes, or if you want a subset (ie later)
//  you can just define the ones you want
int light_mode_array[3] = {red, green, blue};
//initialize the index of the array so we can increment
int light_mode_array_index = 0;
//this is a bit more advanced coding but this is how you get the size of
//  the array programatically
int light_mode_array_length = (sizeof(light_mode_array)/sizeof(light_modes));

//Button
// Instantiate a Bounce object
Bounce debouncer = Bounce(); 




 
void setup() {
  // Set up the transistors to be output for the lights
  pinMode(pin_colorR, OUTPUT);
  pinMode(pin_colorG, OUTPUT);
  pinMode(pin_colorB, OUTPUT);

  //Setup Bouncer Library
  // Setup the button with an internal pull-up :
  pinMode(pin_button,INPUT_PULLUP);

  // After setting up the button, setup the Bounce instance :
  debouncer.attach(pin_button);
  debouncer.interval(5); // interval in ms

}

void loop() {
  //gives a call to the bouncer object to determine state of button
  debouncer.update();

  //fell() indicates that bouncer went from high to low
  if(debouncer.fell()){
    triggerLightStateChange();
  }
  
}

void triggerLightStateChange(){
  //advance the sequence
  light_mode_array_index += 1;
  //if we got to the end of the sequence then loop back to 0
  if(light_mode_array_index >= light_mode_array_length){
    light_mode_array_index = 0;
  }

  
  clearColors();

  switch (light_mode_array[light_mode_array_index]){
    case red:
      analogWritePercent(pin_colorR, 1.0);
      break;
    case green:
      analogWritePercent(pin_colorG, 1.0);
      break;
    case blue:
      analogWritePercent(pin_colorB, 1.0);
      break;
  }

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





/*
 * Old test code functions
 */
 void _v1_function(){
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


