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
  white, blue, red, green,
  nextcolor9, nextcolor9, nextcolor9,
  nextcolor9, nextcolor9, nextcolor9,
  nextcolor9, nextcolor9, nextcolor9,
  off
};

Color color_green = Color(0x00, 0xFF, 0x00);
Color color_red = Color(0xFF, 0x00, 0x00);
Color color_blue = Color(0x00, 0x00, 0xFF);
Color color_white = Color(0xFF, 0xFF, 0xFF);


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


//some holder to keep track of how long the button was pressed
unsigned long button_down_pressed = 0;
bool button_down = false;
int BUTTON_HOLD_TIME = 500; //milliseconds
bool did_catch_fade_adjust = false;


//array of rgb color values for the next 9 color function
//Guess what we get to learn?! where HTML hex colors come from!
// the common rgb format like 0xFF00FF for magenta means
//  0xFF is 256 in integer format in hexidecimal so it means 
//  r and b are both 256 and g is 0 (0xFF,0x00,0xFF) ----> (0xFF00FF)
//  256 is 2^8 which is where 8 bits to a byte comes from
// Have fun and play with the colors
Color next9ColorArray[9] = {
                              Color(0xFF, 0x80, 0x80),Color(0xFF, 0xFF, 0x00),Color(0xFF, 0x80, 0xFF),
                              Color(0x80, 0xFF, 0x80),Color(0x00, 0xFF, 0xFF),Color(0x80, 0xFF, 0xFF),
                              Color(0x80, 0x80, 0xFF),Color(0xFF, 0x00, 0xFF),Color(0xFF, 0xFF, 0x80)
                            };
int nex9ColorArray_index = 0;

//Smoothing parameters play with filterSamples size
#define filterSamples   10

// this determines smoothness  - .0001 is max  1 is off (no smoothing)
float filterVal = 0.55;

int ultraSmoothArray [filterSamples];
int ultraPercentSmoothArray [filterSamples];
float smoothed_percent_dim = 100.0;
//Global dimness to be used by all static color functions
double percent_dim = 1.0;

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
  triggerLightStateChange(false, true);
}

void loop() {
  //gives a call to the bouncer object to determine state of button
  debouncer.update();

  //fell() indicates that bouncer went from high to low
  if (debouncer.fell()) {
    //handle button action
    button_down = true;
    button_down_pressed = millis();
    
  }

  
  if(debouncer.rose()){
    button_down = false;
    if(!did_catch_fade_adjust){
      triggerLightStateChange(true, true);
    }
    did_catch_fade_adjust = false;
  }

  if(button_down && (millis() - button_down_pressed) >= BUTTON_HOLD_TIME){
    did_catch_fade_adjust = true;
    int raw_cm = ultrasonic.Ranging(CM);

    /*
     * First we filter on the raw data with a digital signal which
     * discards about 30% of high and low values
     * 
     * Then we perform a basic smooth on the percent calculated
     * 
     * this gives the best response and allows us to reach near full 100%
     * Eureka!
     */
    if(raw_cm != 0){
      int smoothed_cm = digitalSmooth(raw_cm, ultraSmoothArray);
      int raw_percent_dim = calculateRangePercent(smoothed_cm);
      if(raw_percent_dim != 0){
        smoothed_percent_dim = smooth(raw_percent_dim, filterVal, smoothed_percent_dim);
        percent_dim = (double)smoothed_percent_dim/100.0;
      }
    }
    //Found from the datasheet on the Ultrasonic device
    // that the minimum time between pings is 29ms (anything above that
    //  should be fine, higher values takes longer to filter)
    delay(29);

    /* digital on both (too much data is being discarded because both
     *  digitals get rid of 30% each.
    if(raw_cm != 0){
      int smoothed_cm = digitalSmooth(raw_cm, ultraSmoothArray);
      int raw_percent_dim = calculateRangePercent(smoothed_cm);
      if(raw_percent_dim != 0){
        int smoothed_percent_dim = digitalSmooth(raw_percent_dim, ultraPercentSmoothArray);
        percent_dim = (double)smoothed_percent_dim/100.0;
      }
    }
    */
    /* digital on percent (second best)
    int raw_percent_dim = calculateRangePercent(raw_cm);
    if(raw_percent_dim != 0){
      int smoothed_percent_dim = digitalSmooth(raw_percent_dim, ultraSmoothArray);
      percent_dim = (double)smoothed_percent_dim/100.0;
    }
    */
    /* Just digital smooth on raw reading (not very good)
    int smoothed_cm = digitalSmooth(raw_cm, ultraSmoothArray);
    percent_dim = calculateRangePercent(smoothed_cm);
    */
    triggerLightStateChange(false, false);
    
  }
  
}

int calculateRangePercent(int cm) {
  //try to write from range between 10 cm and 25 cm
  // range is 15 cm so thats whats in the percent calculation
  //  subtract the base and devide by total
  int minimumRange = 10;
  int maximumRange = 45;
  double p = 0;
  if (cm >= (int)minimumRange && cm <= (int)maximumRange) {
    p = ((double)cm - minimumRange)*100 / (maximumRange - minimumRange);
  }
  return p;

}

void triggerLightStateChange(bool advance, bool clear_colors) {
  if(advance){
    //advance the sequence
    light_mode_array_index += 1;
    //if we got to the end of the sequence then loop back to 0
    if (light_mode_array_index >= light_mode_array_length) {
      light_mode_array_index = 0;
    }
  }

  if(clear_colors){
    clearColors();
  }
  
  double p = percent_dim;
  
  switch (light_mode_array[light_mode_array_index]) {
    case red:
      display_color(color_red);
      break;
    case green:
      display_color(color_green);
      break;
    case blue:
      display_color(color_blue);
      break;
    case white:
      display_color(color_white);
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

int smooth(int data, float filterVal, float smoothedVal){


  if (filterVal > 1){      // check to make sure param's are within range
    filterVal = .99;
  }
  else if (filterVal <= 0){
    filterVal = 0;
  }

  smoothedVal = (data * (1 - filterVal)) + (smoothedVal  *  filterVal);

  return (int)smoothedVal;
}



int digitalSmooth(int rawIn, int *sensSmoothArray){     // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
 // static int raw[filterSamples];
  static int sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  // Serial.print("raw = ");

  for (j=0; j<filterSamples; j++){     // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting              
  while(done != 1){        // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++){
      if (sorted[j] > sorted[j + 1]){     // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1); 
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j< top; j++){
    total += sorted[j];  // total remaining indices
    k++; 
  }

  return total / k;    // divide by number of samples
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


