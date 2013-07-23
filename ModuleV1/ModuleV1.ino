// Michael Gubbels
// Date: 2013-07-21 (Started, in San Francisco at Pacific Tradewinds Hostel in Chinatown.)

// RGB pins (PWM pins)
const int redLED   = 9;
const int greenLED = 10;
const int blueLED  = 6;

// Button Grounds
//const int buttonMatrix1 = 12; // BL
//const int buttonMatrix2 = 7;  // BR
//const int buttonMatrix3 = 13; // TL
//const int buttonMatrix4 = 11; // TR

const int buttonPins[4] = { 12, 7, 13, 11 };

// LED Grounds (LED "Top" is the side with the wires coming out, going to the Arduino.)
const int ledGnd1 = 5; // BL
const int ledGnd2 = 8; // 3; // TL
const int ledGnd3 = 2; // 8; // BR
const int ledGnd4 = 3; // TR

// Color definitions
int red[]    = { 255, 0, 0 };
int green[]  = { 0, 255, 0 };
int blue[]   = { 0, 0, 255 };
int purple[] = { 255, 0, 150 };
int yellow[] = { 255, 255, 0 };
int dark[]   = { 0, 0, 0 };

// Program Counter State
int pci = 0; // Program Counter Index
unsigned long epoch = 0;

// Button State
int buttonReadyState[4] = { 1, 1, 1, 1 };
int buttonSwitchState[4] = { 0, 0, 0, 0 };
int buttonColorState[4][3] = { 
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 }
};

void setup() {
  
  // Switch Grounds
  pinMode(buttonPins[0], INPUT);
  pinMode(buttonPins[1], INPUT);
  pinMode(buttonPins[2], INPUT);
  pinMode(buttonPins[3], INPUT);

  // LED Grounds
  pinMode(ledGnd1, OUTPUT);
  pinMode(ledGnd2, OUTPUT);
  pinMode(ledGnd3, OUTPUT);
  pinMode(ledGnd4, OUTPUT);

  // RGB Pins
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  // Serial if you need it 
  Serial.begin(115200);
}

void loop() {

  // Pass in the color of your buttons 1, 2, 3, and 4.
//  ledColor(yellow, red, purple, green);
//  ledColor(dark, blue, blue, green);

  // Uniform color. Pass in the color array of your choice.
//  ledColorU(purple);

  // Dark is just off. Use dark to turn any LED off.
  //ledColor(dark, dark, dark, dark);
  ledColor(buttonColorState[0], buttonColorState[1], buttonColorState[2], buttonColorState[3]);
  
  //
  // Read button states
  //
  
  // Read Button 1
  readButtonState(0);
  readButtonState(1);
  readButtonState(2);
  readButtonState(3);
  
//  buttonSwitchState[1] = (digitalRead(buttonPins[1]) && buttonSwitchState[1] ? 0 : 1);
//  buttonSwitchState[2] = (digitalRead(buttonPins[2]) && buttonSwitchState[2] ? 0 : 1);
//  buttonSwitchState[3] = (digitalRead(buttonPins[3]) && buttonSwitchState[3] ? 0 : 1);
  
  if (buttonSwitchState[0] == 0) {
    buttonColorState[0][0] = 0;
    buttonColorState[0][1] = 0;
    buttonColorState[0][2] = 0;
  } else {
    if (pci == 0) {
      buttonColorState[0][0] = blue[0];
      buttonColorState[0][1] = blue[1];
      buttonColorState[0][2] = blue[2];
    } else {
      buttonColorState[0][0] = 255;
      buttonColorState[0][1] = 255;
      buttonColorState[0][2] = 255;
    }
  }
  
  if (buttonSwitchState[1] == 0) {
    buttonColorState[1][0] = 0;
    buttonColorState[1][1] = 0;
    buttonColorState[1][2] = 0;
  } else {
    if (pci == 0) {
      buttonColorState[1][0] = blue[0];
      buttonColorState[1][1] = blue[1];
      buttonColorState[1][2] = blue[2];
    } else {
      buttonColorState[1][0] = 255;
      buttonColorState[1][1] = 255;
      buttonColorState[1][2] = 255;
    }
  }
  
  if (buttonSwitchState[2] == 0) {
    buttonColorState[2][0] = 0;
    buttonColorState[2][1] = 0;
    buttonColorState[2][2] = 0;
  } else {
    if (pci == 1) {
      buttonColorState[2][0] = blue[0];
      buttonColorState[2][1] = blue[1];
      buttonColorState[2][2] = blue[2];
    } else {
      buttonColorState[2][0] = 255;
      buttonColorState[2][1] = 255;
      buttonColorState[2][2] = 255;
    }
  }
  
  if (buttonSwitchState[3] == 0) {
    buttonColorState[3][0] = 0;
    buttonColorState[3][1] = 0;
    buttonColorState[3][2] = 0;
  } else {
    if (pci == 1) {
      buttonColorState[3][0] = blue[0];
      buttonColorState[3][1] = blue[1];
      buttonColorState[3][2] = blue[2];
    } else {
      buttonColorState[3][0] = 255;
      buttonColorState[3][1] = 255;
      buttonColorState[3][2] = 255;
    }
  }
  
  // Update PC column
  unsigned long currentTime = millis();
  if (currentTime - epoch > 500) {
    epoch = currentTime;
    if (pci == 0) {
//      buttonColorState[0][0] = red[0];
//      buttonColorState[0][1] = red[1];
//      buttonColorState[0][2] = red[2];
//      
//      buttonColorState[1][0] = red[0];
//      buttonColorState[1][1] = red[1];
//      buttonColorState[1][2] = red[2];
      
      pci = 1;
      
    } else {
      
//      buttonColorState[2][0] = red[0];
//      buttonColorState[2][1] = red[1];
//      buttonColorState[2][2] = red[2];
//      
//      buttonColorState[3][0] = red[0];
//      buttonColorState[3][1] = red[1];
//      buttonColorState[3][2] = red[2];
      
      pci = 0;
    }
  }

  // Check for button presses and output states
  // Enable if you want to test your buttons
  Serial.print(buttonSwitchState[0]);
  Serial.print("\t");
  Serial.print(buttonSwitchState[1]);
  Serial.print("\t");
  Serial.print(buttonSwitchState[2]);
  Serial.print("\t");
  Serial.println(buttonSwitchState[3]);
}

void readButtonState(int i) {
  if (buttonReadyState[i]) {
    buttonReadyState[i] = 0;
    if (digitalRead(buttonPins[i]) && buttonSwitchState[i]) {
      buttonSwitchState[i] = 0;
    } else if (digitalRead(buttonPins[i]) && !buttonSwitchState[i]) {
      buttonSwitchState[i] = 1;
    }
  } else {
    if (!digitalRead(buttonPins[i])) {
      buttonReadyState[i] = 1;
    }
  }
}

// Control individual LEDs
// Pass in a RGB color array for each LED
void ledColor(int led1[], int led2[], int led3[], int led4[]) {
  
  // LED 1
  analogWrite(redLED, led1[0]); // Set color
  analogWrite(greenLED, led1[1]);
  analogWrite(blueLED, led1[2]);
  // Flicker control
  delay(2);
  digitalWrite(ledGnd1, LOW); // "Turn on" LED
  // Flicker control
  delayMicroseconds(1100); // Wait (for POV effect?)
  digitalWrite(ledGnd1, HIGH); // "Turn off" LED

  // LED 2
  analogWrite(redLED, led2[0]);
  analogWrite(greenLED, led2[1]);
  analogWrite(blueLED, led2[2]);
  // Flicker control
  delay(2);
  digitalWrite(ledGnd2, LOW);
  // Flicker control
  delayMicroseconds(1100);
  digitalWrite(ledGnd2, HIGH);

  // LED 3
  analogWrite(redLED, led3[0]);
  analogWrite(greenLED, led3[1]);
  analogWrite(blueLED, led3[2]);
  // Flicker control
  delay(2);
  digitalWrite(ledGnd3, LOW);
  // Flicker control
  delayMicroseconds(1100);
  digitalWrite(ledGnd3, HIGH);

  // LED 4
  analogWrite(redLED, led4[0]);
  analogWrite(greenLED, led4[1]);
  analogWrite(blueLED, led4[2]);
  // Flicker control
  delay(2);
  digitalWrite(ledGnd4, LOW);
  // Flicker control
  delayMicroseconds(1100);
  digitalWrite(ledGnd4, HIGH);
}

// Uniform color
// This doesn't appear to work well with mixed colors.
void ledColorU(int color[]) {
  analogWrite(redLED, color[0]);
  analogWrite(greenLED, color[1]);
  analogWrite(blueLED, color[2]);
  digitalWrite(ledGnd1, LOW);
  digitalWrite(ledGnd2, LOW);
  digitalWrite(ledGnd3, LOW);
  digitalWrite(ledGnd4, LOW); 
}
