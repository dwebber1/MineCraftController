int w = 12;
int a = 11;
int d = 10;
int s = 9;
int space = 7;
int esc = 8;
int e = 6;
int encoderPin1 = 2;
int encoderPin2 = 3;

boolean SpacealreadyOn = false;
boolean WalreadyOn = false;
boolean AalreadyOn = false;
boolean SalreadyOn = false;
boolean DalreadyOn = false;


const int switchPin   =  5;
const int ledPin = 13; 
//const int mouseButton = 7;  

const int xAxis  = A1;         // joystick X axis to A1 
const int yAxis  = A0; 

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;
int oldValue = 0;
int newValue = 0;
int dif = 0;

int range = 12;               // output range of X or Y movement (zero to range)
int responseDelay = 5;        // response delay of the mouse, in ms
int threshold = range/4;      // resting threshold
int center = range/2;         // resting position value
const float powerValue = 1.9; // for exponential behavior, 1 < value < 2

boolean mouseIsActive = false;      // whether or not to control the mouse
int lastSwitchState = LOW;          // previous switch state
boolean mouseButtonPressed = false; // whether or not mouse button pressed
int  lastReading = 1;       // last joystick/mouse button reading
long debounceTime = 0;      // last time the mouse button was toggled
long debounce = 50;

//Debounce for spacebar

int spacestate=0;




void setup() {
  Keyboard.begin();
  Mouse.begin();  
  pinMode(switchPin,   INPUT);   // the switch pin
  //pinMode(mouseButton, INPUT_PULLUP);
  
  pinMode(ledPin, OUTPUT);    
  
  pinMode(w,INPUT);
  pinMode(a,INPUT);
  pinMode(s,INPUT);
  pinMode(d,INPUT);
  pinMode(space,INPUT);
  pinMode(esc,INPUT);
  pinMode(e,INPUT);

  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor o

  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
  
  
}

void loop() {
   mouse();
   wasd();
   scroll();
   jump();
   
}

void wasd(){

  if (digitalRead(w) == HIGH) {
    Keyboard.press('w'); 
    delay(200);
  }
  if (digitalRead(a) == HIGH) {
    Keyboard.press('a'); 
    delay(200);
  }
  if (digitalRead(s) == HIGH) {
    Keyboard.press('s'); 
    delay(200);
  }
  if (digitalRead(d) == HIGH) {
    Keyboard.press('d'); 
    delay(200);
  }
  
  
  if (digitalRead(esc) == HIGH) {
    Keyboard.press(177); 
    delay(300);
  }
   if (digitalRead(e) == HIGH) {
    Keyboard.press('e'); 
    delay(300);
  }
//  //else{
//  Keyboard.releaseAll();
//
//  
//  //e
//  
//
//
//   
//                                                         
//                                                                  
//   }
//  
}
void updateEncoder() {
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}

void scroll(){
  oldValue = newValue;
  newValue = encoderValue;
  dif = (oldValue - newValue)/2;
  


  Serial.println(dif);
  if (oldValue != newValue) {
    //Serial.println("The Values are different");
    Mouse.move(0, 0, dif);
    

  }
}

void mouse(){
  int switchState;  // State of the mouse enable/disable button
   //int buttonState;  // State of the mouse left button switch on joystick
  int xReading, yReading; // readings of the joystick movements
  int buttonReading;   

  switchState = digitalRead(switchPin);  // read the mouse disable switch
  // if it's changed and it's high, toggle the mouse state
  if (switchState != lastSwitchState) {
    if (switchState == HIGH) {
      mouseIsActive = !mouseIsActive;
     digitalWrite(13, mouseIsActive);   // toggle LED to indicate mouse state
    } 
  }
  lastSwitchState = switchState;    // save switch state for next comparison

  // read and scale the two joystick readings, one for each axis
  xReading = readAxis(xAxis);
  yReading = readAxis(yAxis);

  // This code gives the mouse a nonlinear acceleratione
  //   These 8 lines may be commented out to have linear acceleration
  if(xReading > 0)
     xReading =  (int)pow(powerValue,xReading);
  else if(xReading < 0)
     xReading = -(int)pow(powerValue,-xReading);
     
  if(yReading > 0)
     yReading =  (int)pow(powerValue,yReading);
  else if(yReading < 0)
     yReading = -(int)pow(powerValue,-yReading);  // end nonlinear acceleration code

  // Read the joystick button as the left mouse button.  Debounce per 
  //   Ladyada code at https://learn.adafruit.com/tilt-sensor/using-a-tilt-sensor
//  //buttonReading = digitalRead(mouseButton);  // read the mouse left button (push joystick)
//  if(buttonReading != lastReading) {         // switch changed
//     debounceTime = millis();                // reset debounce timer
//  }
//  if((millis() - debounceTime) > debounce) {
//     buttonState = buttonReading;
//     if(buttonState == LOW) {
//        mouseButtonPressed = true;
//     }
//     else {
//        mouseButtonPressed = false;
//     }
//  } 
//  lastReading = buttonReading;
//  digitalWrite(ledPin, mouseButtonPressed);  // toggle LED to indicate button state
//  
  // if the mouse control state is active, move the mouse:
  if (mouseIsActive) {
      if (mouseButtonPressed) {  // if joystick pressed down, indicate that too
         Mouse.move(xReading, yReading, 0);
         //digitalWrite(13,HIGH);
      }
      else {
         Mouse.move(xReading, yReading,0);  // move, no mouse button press
      }
  }  
  delay(responseDelay);  // wait between mouse readings
}

// Reads a joystick axis (0 or 1 for x or y) and scales the 
//  analog input range to a range from 0 to <range>
int readAxis(int thisAxis) { 
  int reading = analogRead(thisAxis);  // read the analog input

  // map the reading from the analog input range to the output range
  reading = map(reading, 0, 1023, 0, range);

  // if the output reading is outside from the rest position threshold, use it
  int distance = center - reading;

  if (abs(distance) < threshold) { // if distance not to threshold, no move
    distance = 0;                  // prevents tiny jitters due to readings
  } 
  return distance;   // return the distance for this axis


}

void jump(){
   if (digitalRead(space)==HIGH  ){
    if(!SpacealreadyOn){
    Keyboard.press(32);
    //delay(100);
    SpacealreadyOn = true;
    
    }
  }    
    
    else{
      Keyboard.release(32);
      SpacealreadyOn= false;
      
    }
}

  




