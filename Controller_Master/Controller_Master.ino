
int space = 7;
int esc = 8;
int e = 6;
int leftClick=4;
int encoderPin1 = 2;
int encoderPin2 = 3;
boolean alreadyOn = false;
boolean ealreadyOn = false;
boolean escapealreadyOn = false;
boolean WalreadyOn = false;
boolean AalreadyOn = false;
boolean SalreadyOn = false;
boolean DalreadyOn = false;
boolean leftClickalreadyOn = false;

const int switchPin   =  5;
const int ledPin = 13;
//const int mouseButton = 7;

const int xAxis  = A1;         // joystick X axis to A1
const int yAxis  = A0;

int MouseXaxis = A2;
int MouseYaxis = A3;



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
int threshold = range / 4;    // resting threshold
int center = range / 2;       // resting position value
const float powerValue = 1.9; // for exponential behavior, 1 < value < 2

boolean mouseIsActive = false;      // whether or not to control the mouse
int lastSwitchState = LOW;          // previous switch state
boolean mouseButtonPressed = false; // whether or not mouse button pressed
int  lastReading = 1;       // last joystick/mouse button reading
long debounceTime = 0;      // last time the mouse button was toggled
long debounce = 50;

//Debounce for spacebar

int spacestate = 0;




void setup() {
  Keyboard.begin();
  Mouse.begin();
  pinMode(switchPin,   INPUT);   // the switch pin
  //pinMode(mouseButton, INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);


  pinMode(space, INPUT_PULLUP);
  pinMode(esc, INPUT);
  pinMode(e, INPUT);
  pinMode(leftClick,INPUT_PULLUP);

  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor o

  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);


}

void loop() {
  mouse();
  //WASD();
  scroll();
jump();

}


//

void updateEncoder() {
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}

void scroll() {
  oldValue = newValue;
  newValue = encoderValue;
  dif = (oldValue - newValue) / 2;



  //Serial.println(dif);
  if (oldValue != newValue) {
    //Serial.println("The Values are different");
    Mouse.move(0, 0, dif);


  }
}

void mouse() {
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
  if (xReading > 0)
    xReading =  (int)pow(powerValue, xReading);
  else if (xReading < 0)
    xReading = -(int)pow(powerValue, -xReading);

  if (yReading > 0)
    yReading =  (int)pow(powerValue, yReading);
  else if (yReading < 0)
    yReading = -(int)pow(powerValue, -yReading); // end nonlinear acceleration code

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
      Mouse.move(xReading, yReading, 0); // move, no mouse button press
    }
  }
  //delay(responseDelay);  // wait between mouse readings
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

void jump() {
  if (digitalRead(space) == LOW           ) {
    if (!alreadyOn) {
      Keyboard.press(32);
      //delay(100);
      alreadyOn = true;

    }
  }

  else {
    Keyboard.release(32);
    alreadyOn = false;

  }

  if (digitalRead(e) == HIGH  ) {
    if (!ealreadyOn) {
      Keyboard.press('e');
      //delay(100);
      ealreadyOn = true;

    }
  }

  else {
    Keyboard.release('e');
    ealreadyOn = false;

  }

  if (digitalRead(esc) == HIGH  ) {
    if (!escapealreadyOn) {
      Keyboard.press(177);
      //delay(100);
      escapealreadyOn = true;

    }
  }

  else {
    Keyboard.release(177);
    escapealreadyOn = false;

  }

  if (digitalRead(leftClick) == LOW  ) {
    if (!leftClickalreadyOn) {
      Mouse.press();
      //delay(100);
      leftClickalreadyOn = true;

    }
  }

  else {
    Mouse.release();
    leftClickalreadyOn = false;

  }



}

void WASD() {
  int xValue = analogRead(MouseXaxis);
  int yValue = analogRead(MouseYaxis);

  //  if (xValue > 511 && xValue < 1025) {
  //    Keyboard.press('d');
  //    //delay(500);
  //
  //  }
  //  else {
  //    Keyboard.release('d');
  //  }
  //  if (xValue > -1 && xValue < 509) {
  //    Keyboard.press('a');
  //    //delay(500);
  //
  //  }
  //  else {
  //    Keyboard.release('a');
  //  }

  //  if (yValue > 523 && yValue < 1025) {
  //    Keyboard.press('s');
  //    //delay(500);
  //  }
  //  else {
  //    Keyboard.release('s');
  //  }
  //  if (yValue > -1 && yValue < 506) {
  //    Keyboard.press('w');
  //    //delay(500);
  //
  //  }
  //  else {
  //    Keyboard.release('w');
  //  }
  if (yValue > -1 && yValue < 50) {
    if (!WalreadyOn) {
      Keyboard.press('w');
      //delay(100);
      WalreadyOn = true;

    }
  }

  else {
    Keyboard.release('w');
    WalreadyOn = false;

  }
  if (yValue > 523 && yValue < 1025) {
    if (!SalreadyOn) {
      Keyboard.press('s');
      //delay(100);
      SalreadyOn = true;

    }
  }

  else {
    Keyboard.release('s');
    SalreadyOn = false;

  }//a
  if (xValue > -1 && xValue < 500) {
    if (!AalreadyOn) {
      Keyboard.press('a');
      //delay(100);
      AalreadyOn = true;

    }
  }

  else {
    Keyboard.release('a');
    AalreadyOn = false;

  }

  if (xValue > 511 && xValue < 1025) {
    if (!DalreadyOn) {
      Keyboard.press('d');
      //delay(100);
      DalreadyOn = true;

    }
  }

  else {
    Keyboard.release('d');
    DalreadyOn = false;

  }






}



