// Need the time library
#include <Time.h>
#include <Math.h>

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 
// T1262347200  //noon Jan 1 2010


//RGB LED pins
int ledAnalogOne[] = {9, 10, 11}; //the three pins of the first analog LED 3 = redPin, 5 = greenPin, 6 = bluePin
                                //These pins must be PWM

//Defined Colors (different RGB (red, green, blue) values for colors
//(to add your own ie. fuscia experiment and then add to the list)
const byte RED[] = {255, 0, 0}; 
const byte YELLOW[] = {255, 255, 0}; 
const byte GREEN[] = {0, 255, 0}; 
const byte BLUE[] = {0, 0, 255}; 
const byte BLACK[] = {0, 0, 0}; 

// button pressing constants. They're used here to 
// set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// time constants
const int timeZone = -7;     // time zone away from UTC, PDT is 7 hours back
const long secHr = pow(60,2); // seconds in an hour
const long secHrDay = pow(60,2) * 24; // seconds in 60 hours
const long timeChange = timeZone * secHr;
// calculate our times
const long resetTime = 4 * secHr;
const long oneTime = 6 * secHr;
const long twoTime = 12 * secHr;
const long threeTime = 18 * secHr;
const long midnight = 24 * secHr;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int buttonToday = 0;         // variance for holding the status of the button push
int blink = 0;               // variable for determining to blink



// define the setup
void setup(){

  // open serial for testing
  Serial.begin(9600); // open serial at 9600 bps
  
  for(int i = 0; i < 3; i++){
    pinMode(ledAnalogOne[i], OUTPUT);   //Set the three LED pins as outputs
  }
  
  // initialize the LED pin 13 as an output:
  pinMode(ledPin, OUTPUT);   
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT); 
  
  setColor(ledAnalogOne, BLACK);       //Turn off led 1
  
}

void loop(){
  
  // check to see if the serial port is available, if so, start talking
  if(Serial.available() ) 
  {
    processSyncMessage();
  }
  if(timeStatus() == timeNotSet) 
  {
    Serial.println("waiting for sync message");
  }
  else
  {  
    digitalClockDisplay();  
  } 
  
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == 0) {     
    // turn LED on:    
    digitalWrite(ledPin, HIGH);  
    buttonToday = 1;
  } 
  else {
    // turn LED off:
    digitalWrite(ledPin, LOW); 
  }

/* color by time
  Set the color of the RGB LED based on the current time
*/
  // give the time a variable
  long t = now();
  
  // run the timeColor function
  timeColor(t, buttonToday);

}

/* sets the color of the RGB light to a value based on waht time it is now
   Goal is to show the following:
   1. Blue between midnight and 6am
   2. Fade to yellow between 6am and 12pm
   3. Fade to red between 12pm and 6pm
   4. Blink red after 6pm until midnight
   ** turn green for full day once button pressed, until 4am reset
*/
void timeColor(long t, int buttonToday){ 

  long timeInTimeZone = t + timeChange;
  long dayTime = fmod(timeInTimeZone, secHrDay);
  
  // Serial.print(timeInTimeZone);
  // Serial.print("    ");      
  // Serial.print(dayTime);
  // Serial.print("    ");
  
  
  // determine if the button was pressed today
  if(dayTime >= 0 && dayTime <= 600){
    // sets the color back to blue and resets the status of whether someone pressed button
    setColor(ledAnalogOne, BLUE);   
    buttonToday = 0;  
    // Serial.print("Reset Time!");   
  }
  else if(buttonToday == 1){
    setColor(ledAnalogOne, GREEN);
    // Serial.print("You Met Your Goal!");       
  }  
  // if button not pressed today, then 
  else{
    if(dayTime > 0 && dayTime <= oneTime){
      // color should be BLUE
      setColor(ledAnalogOne, BLUE);      
      // Serial.print("Shit");   
    }
    else if(dayTime > oneTime && dayTime <= twoTime) {
      // what is our progress on time
      float progress = calcProgress(dayTime, oneTime, twoTime);
      // color should fade between blue and yellow
      int rgbRed = progress * 255;
      int rgbGreen = progress * 255;
      int rgbBlue = 255 - (progress * 255);
      // put it in an array
      byte colorAssigned[] = {rgbRed, rgbGreen, rgbBlue}; 
      // set color
      setColor(ledAnalogOne, colorAssigned);  
      // Serial.print("Fuck   ");
      // Serial.print(progress);
      // Serial.print(" ");        
      // Serial.print(colorAssigned[0]);
      // Serial.print(" ");    
      // Serial.print(colorAssigned[1]);
      // Serial.print(" ");  
      // Serial.print(colorAssigned[2]);
      // Serial.print(" ");      
    }
    else if(dayTime > twoTime && dayTime <= threeTime) {
      // what is our progress on time
      float progress = calcProgress(dayTime, twoTime, threeTime);
      // color should fade between yellow and red
      int rgbRed = 255;
      int rgbGreen = 255 - (progress * 255);
      int rgbBlue = 0;
      // put it in an array
      byte colorAssigned[] = {rgbRed, rgbGreen, rgbBlue}; 
      // set color
      setColor(ledAnalogOne, colorAssigned); 
      // Serial.print("Double Fuck  ");  
      // Serial.print(progress);
      // Serial.print(" ");        
      // Serial.print(colorAssigned[0]);
      // Serial.print(" ");    
      // Serial.print(colorAssigned[1]);
      // Serial.print(" ");  
      // Serial.print(colorAssigned[2]);
      // Serial.print(" ");       
    }
    else if(dayTime > threeTime && dayTime <= midnight) {
      // color should be flashing red
      // this is testing
      // Serial.print(blink); 
      // Serial.print("  ");       
      
      if(blink < 15){
        // set to red
        setColor(ledAnalogOne, RED); 
        blink++;
      }  
      else if(blink < 29){
        // set to black
        setColor(ledAnalogOne, BLACK);
        blink++;
      }  
      else {
        setColor(ledAnalogOne, BLACK);
        blink = 0;
      }  
      // Serial.print("Triple Fuck");       
    }
    else {
      // fail
      // Serial.print("failed!");
    }    
  }  
}

float calcProgress(float dayTime, float first, float second){
  // all this does is do a calculation on how far we are along from one time to another. 
  // would be nice if this was not float!
  float top = dayTime - first;
  float bottom = second - first;
  float result = top / bottom;    
  return result;
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
   //Serial.print("LED number ");
   //Serial.print(led[i]);
   //Serial.println(" "); 
   //Serial.print("Color value ");
   //Serial.print(color[i]);
   //Serial.println(" "); 
 }
}

/* A version of setColor that takes a predefined color (neccesary to allow const int pre-defined colors */
void setColor(int* led, const byte* color){
  byte tempByte[] = {color[0], color[1], color[2]};
  setColor(led, tempByte);
}

// this prints the time to serial
void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of header & 10 ASCII digits
    char c = Serial.read() ; 
    Serial.print(c);  
    if( c == TIME_HEADER ) {       
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){   
        c = Serial.read();          
        if( c >= '0' && c <= '9'){   
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
      }   
      setTime(pctime);   // Sync Arduino clock to the time received on the serial port
    }  
  }
}
