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
  blue,
  off,
  white,
  rainbow,
  random_spaz,
  fade
} light_modes;

//a constant array to declare states of light display
//  this can be all of the LightModes, or if you want a subset (ie later)
//  you can just define the ones you want
int light_mode_array[] = {off, red, green, blue, white, fade, rainbow, random_spaz};
//initialize the index of the array so we can increment
int light_mode_array_index = 0;
//this is a bit more advanced coding but this is how you get the size of
//  the array programatically
int light_mode_array_length = (sizeof(light_mode_array)/sizeof(light_modes));

//Button
// Instantiate a Bounce object
Bounce debouncer = Bounce(); 

//Ultrasonic device initialization:
Ultrasonic ultrasonic(pin_ult_trig,pin_ult_echo); //Ultrasonic ultrasonic(Trig,Echo);



 
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
  /*
   * For clarity I've commented out the button code so we can focus on the ultrasonic
  //gives a call to the bouncer object to determine state of button
  debouncer.update();

  //fell() indicates that bouncer went from high to low
  if(debouncer.fell()){
    triggerLightStateChange();
  }
  */
  
  int cm = ultrasonic.Ranging(CM);

  double p = calculateRangePercent(cm);
  
  analogWritePercent(pin_colorR, p);
  analogWritePercent(pin_colorG, p);
  analogWritePercent(pin_colorB, p);
  
  
}

double calculateRangePercent(int cm){
  //try to write from range between 10 cm and 25 cm
  // range is 15 cm so thats whats in the percent calculation
  //  subtract the base and devide by total
  double minimumRange = 10.0;
  double maximumRange = 45.0;
  double p = 0.0;
  if(cm >= (int)minimumRange && cm <= (int)maximumRange){
    p = ((double)cm-minimumRange)/(maximumRange-minimumRange);
  }
  return p;

}

void triggerLightStateChange(){
  //advance the sequence
  light_mode_array_index += 1;
  //if we got to the end of the sequence then loop back to 0
  if(light_mode_array_index >= light_mode_array_length){
    light_mode_array_index = 0;
  }

  
  clearColors();

  double p = 1.0;
  switch (light_mode_array[light_mode_array_index]){
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

void fadeAnim(){
  for(int i = 0; i < 256; i ++){
    analogWritePercent(pin_colorR, (double)i/256.0);
    analogWritePercent(pin_colorG, (double)i/256.0);
    analogWritePercent(pin_colorB, (double)i/256.0);
    delay(20);
  }
}

void makerainbow(){
  for(int i = 0; i < 256; i++){
    int r = i;
    int g = i + 70;
    int b = g + 70;
    if(g >= 256){
      g = g - 256;
    }
    if(b >= 256){
      b = b - 256;
    }
    
    analogWritePercent(pin_colorR, (double)r/256.0);
    analogWritePercent(pin_colorG, (double)b/256.0);
    analogWritePercent(pin_colorB, (double)g/256.0);
    delay(20);
    
  }
}


void random_spaz_anim(){
  for(int i = 0; i < 256; i++){
    int r = random(255);
    int b = random(255);
    int g = random(255);
    
    analogWritePercent(pin_colorR, (double)r/256.0);
    analogWritePercent(pin_colorG, (double)b/256.0);
    analogWritePercent(pin_colorB, (double)g/256.0);
    delay(20);
  }
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


