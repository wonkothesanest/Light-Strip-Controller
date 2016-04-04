//http://playground.arduino.cc/Code/Bounce
#include <Bounce2.h>

//https://github.com/hemalchevli/ultrasonic-library
#include <Ultrasonic.h>

//ooh we get to make a class
#include "LampControlColor.h"
/*


   Coding steps
   define pins
   turn on transistors to 1/2 power and cycle through each color
   test
   add button click register and cycle through colors via click
   test
   add range sensor and decide on 0 - 80% (trying not to fry npn)
   test
   code up proper user interface

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
  blue,
  off,
  white,
  nextcolor9,
  rainbow,
  random_spaz,
  fade
} light_modes;

//a constant array to declare states of light display
//  this can be all of the LightModes, or if you want a subset (ie later)
//  you can just define the ones you want
// This time we are only calling the solid colors because we don't have interrupts
//    for the animations
int light_mode_array[] = {
  red, green, blue, white, off,
  nextcolor9, nextcolor9, nextcolor9,
  nextcolor9, nextcolor9, nextcolor9,
  nextcolor9, nextcolor9, nextcolor9
};

//initialize the index of the array so we can increment
int light_mode_array_index = 0;
//this is a bit more advanced coding but this is how you get the size of
//  the array programatically
int light_mode_array_length = (sizeof(light_mode_array) / sizeof(light_modes));

//Button
// Instantiate a Bounce object
Bounce debouncer = Bounce();

//Ultrasonic device initialization:
Ultrasonic ultrasonic(pin_ult_trig, pin_ult_echo); //Ultrasonic ultrasonic(Trig,Echo);

//Global dimness to be used by all static color functions
double percent_dim = 1.0;

//some holder to keep track of how long the button was pressed
unsigned long button_down_pressed = 0;
bool button_down = false;
int BUTTON_HOLD_TIME = 500; //milliseconds


//array of rgb color values for the next 9 color function
//Guess what we get to learn?! where HTML hex colors come from!
// the common rgb format like 0xFF00FF for magenta means
//  0xFF is 256 in integer format in hexidecimal so it means 
//  r and b are both 256 and g is 0 (0xFF,0x00,0xFF) ----> (0xFF00FF)
//  256 is 2^8 which is where 8 bits to a byte comes from
// Have fun and play with the colors
Color next9ColorArray[9] = {
                              Color(0xFF, 0x00, 0x00),Color(0xFF, 0xFF, 0x00),Color(0xFF, 0x00, 0x00),
                              Color(0x00, 0xFF, 0x00),Color(0x00, 0xFF, 0xFF),Color(0x00, 0x00, 0x00),
                              Color(0x00, 0x00, 0xFF),Color(0xFF, 0x00, 0xFF),Color(0x00, 0x00, 0x00)
                            };
int nex9ColorArray_index = 0;


void setup() {
  // Set up the transistors to be output for the lights
  pinMode(pin_colorR, OUTPUT);
  pinMode(pin_colorG, OUTPUT);
  pinMode(pin_colorB, OUTPUT);

  //Setup Bouncer Library
  // Setup the button with an internal pull-up :
  pinMode(pin_button, INPUT_PULLUP);

  // After setting up the button, setup the Bounce instance :
  debouncer.attach(pin_button);
  debouncer.interval(5); // interval in ms
}

void loop() {
  //gives a call to the bouncer object to determine state of button
  debouncer.update();

  //fell() indicates that bouncer went from high to low
  if (debouncer.fell()) {
    //handle button action
    button_down = true;
    button_down_pressed = millis();
    
    triggerLightStateChange(true);
  }

  
  if(debouncer.rose()){
    button_down = false;
  }

  if(button_down && (millis() - button_down_pressed) >= BUTTON_HOLD_TIME){
    int cm = ultrasonic.Ranging(CM);
    percent_dim = calculateRangePercent(cm);
    
    triggerLightStateChange(false);
    
  }
  
}

double calculateRangePercent(int cm) {
  //try to write from range between 10 cm and 25 cm
  // range is 15 cm so thats whats in the percent calculation
  //  subtract the base and devide by total
  double minimumRange = 10.0;
  double maximumRange = 45.0;
  double p = 0.0;
  if (cm >= (int)minimumRange && cm <= (int)maximumRange) {
    p = ((double)cm - minimumRange) / (maximumRange - minimumRange);
  }
  return p;

}

void triggerLightStateChange(bool advance) {
  if(advance){
    //advance the sequence
    light_mode_array_index += 1;
    //if we got to the end of the sequence then loop back to 0
    if (light_mode_array_index >= light_mode_array_length) {
      light_mode_array_index = 0;
    }
  }
  
  clearColors();
  
  double p = percent_dim;
  
  switch (light_mode_array[light_mode_array_index]) {
    case red:
      analogWritePercent(pin_colorR, p);
      break;
    case green:
      analogWritePercent(pin_colorG, p);
      break;
    case blue:
      analogWritePercent(pin_colorB, p);
      break;
    case white:
      analogWritePercent(pin_colorR, p);
      analogWritePercent(pin_colorG, p);
      analogWritePercent(pin_colorB, p);
      break;
    case rainbow:
      makerainbow();
      break;
    case random_spaz:
      random_spaz_anim();
      break;
    case fade:
      fadeAnim();
      break;
    case nextcolor9:
      setNextColor9(advance);
      break;

  }
}


//percent is 0.0 - 1.0
void analogWritePercent(int pin, double percent) {
  double dval = (double)percent * maxpower * 255.0;

  int value = (int)dval;
  analogWrite(pin, value);
}

void clearColors() {
  analogWritePercent(pin_colorR, 0.0);
  analogWritePercent(pin_colorG, 0.0);
  analogWritePercent(pin_colorB, 0.0);

}

void setNextColor9(bool advance) {
  if(advance){
    //advance the sequence
    nex9ColorArray_index += 1;
    //if we got to the end of the sequence then loop back to 0
    if (nex9ColorArray_index >= 9) {
      nex9ColorArray_index = 0;
    }
  }
  display_color(next9ColorArray[nex9ColorArray_index]);
}

void display_color(Color c){
  write_pin_with_byte(pin_colorR, c.red);
  write_pin_with_byte(pin_colorG, c.green);
  write_pin_with_byte(pin_colorB, c.blue);
}

void write_pin_with_byte(int set_pin, int byt){
    
    analogWritePercent(set_pin, ((double)byt / 256.0)*percent_dim);
}

void fadeAnim() {
  for (int i = 0; i < 256; i ++) {
    analogWritePercent(pin_colorR, (double)i / 256.0);
    analogWritePercent(pin_colorG, (double)i / 256.0);
    analogWritePercent(pin_colorB, (double)i / 256.0);
    delay(20);
  }
}

void makerainbow() {
  for (int i = 0; i < 256; i++) {
    int r = i;
    int g = i + 70;
    int b = g + 70;
    if (g >= 256) {
      g = g - 256;
    }
    if (b >= 256) {
      b = b - 256;
    }

    analogWritePercent(pin_colorR, (double)r / 256.0);
    analogWritePercent(pin_colorG, (double)b / 256.0);
    analogWritePercent(pin_colorB, (double)g / 256.0);
    delay(20);

  }
}


void random_spaz_anim() {
  for (int i = 0; i < 256; i++) {
    int r = random(255);
    int b = random(255);
    int g = random(255);

    analogWritePercent(pin_colorR, (double)r / 256.0);
    analogWritePercent(pin_colorG, (double)b / 256.0);
    analogWritePercent(pin_colorB, (double)g / 256.0);
    delay(20);
  }
}



/*
   Old test code functions
*/
void _v1_function() {
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


