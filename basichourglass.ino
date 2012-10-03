/*
  basic hourglass
 
  Changes the color of two RGB LEDs depending on orientation of tilt

  right now this makes the light on the up side green and on the down
  side red. and it works!

 */

// constants won't change. They're used here to 
// set pin numbers:

const int buttonPin = 8;     // the number of the pushbutton pin

//RGB LED pins, these pins must be PWM
//the three pins of the first analog LED 3 = redPin, 5 = greenPin, 6 = bluePin
int ledAnalogOne[] = {3, 5, 6}; 
int ledAnalogTwo[] = {9, 10, 11}; 

//defined colors (different RGB (red, green, blue) values for colors
const byte RED[] = {10, 0, 0}; 
const byte YELLOW[] = {255, 255, 0}; 
const byte GREEN[] = {0, 10, 0}; 
const byte BLUE[] = {0, 0, 255}; 
const byte BLACK[] = {0, 0, 0}; 

// variables will change:
// variable for reading the pushbutton status
int buttonState = 0;

// debouncing code for the tilt sensor
// to record previous reading
int lastButtonState = LOW; 
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

// define the setup
void setup() {
  
  //Set the six LED pins as outputs
  // for ledAnalogOne
  for(int i = 0; i < 3; i++){
    pinMode(ledAnalogOne[i], OUTPUT);   
  }
  // for ledAnalogTwo  
  for(int i = 0; i < 3; i++){
    pinMode(ledAnalogTwo[i], OUTPUT);   
  }
  
  //Turn off led 1
  setColor(ledAnalogOne, BLACK);
  //Turn off led 2
  setColor(ledAnalogTwo, BLACK);
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);     

}

void loop(){
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if the tilt switch changed
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonState = reading;
  }

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {     
    // make LED 1 GREEN and LED 2 to RED
    setColor(ledAnalogOne, GREEN);
    setColor(ledAnalogTwo, RED);
  } 
  else {
    // flip LED 1 to RED and LED 2 to GREEN
    setColor(ledAnalogOne, RED);
    setColor(ledAnalogTwo, GREEN);
  }
  
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}

/* Sets the color of the LED to any RGB Value
   led - (int array of three values defining the LEDs pins (led[0] = redPin, led[1] = greenPin, led[2] = bluePin))
   color - (byte array of three values defing an RGB color to display (color[0] = new Red value, color[1] = new Green value, color[2] = new Red value
*/
void setColor(int* led, byte* color){
 for(int i = 0; i < 3; i++){             //iterate through each of the three pins (red green blue)
   analogWrite(led[i], color[i]);  //set the analog output value of each pin to the input value (ie led[0] (red pin) to 255- color[0] (red input color)
                                         //we use 255 - the value because our RGB LED is common anode, this means a color is full on when we output analogWrite(pin, 0)
                                         //and off when we output analogWrite(pin, 255). 
   Serial.print("LED number ");
   Serial.print(led[i]);
   Serial.println(" "); 
   Serial.print("Color value ");
   Serial.print(color[i]);
   Serial.println(" "); 
 }
}

/* A version of setColor that takes a predefined color (neccesary to allow const int pre-defined colors */
void setColor(int* led, const byte* color){
  byte tempByte[] = {color[0], color[1], color[2]};
  setColor(led, tempByte);
}
