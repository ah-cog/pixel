// Michael Gubbels
// Date: 2013-07-21 (Started, in San Francisco at Pacific Tradewinds Hostel in Chinatown.)

// RGB pins (PWM pins)
const int redLED   = 9;
const int greenLED = 10;
const int blueLED  = 6;

// Button Grounds
const int buttonPins[4] = { 12, 7, 13, 11 };

// LED Grounds (LED "Top" is the side with the wires coming out, going to the Arduino.)
const int ledPins[4] = { 5, 8, 2, 3 };

// Color definitions
int red[]    = { 255, 0, 0 };
int green[]  = { 0, 255, 0 };
int blue[]   = { 0, 0, 255 };
int purple[] = { 255, 0, 150 };
int yellow[] = { 255, 255, 0 };
int dark[]   = { 0, 0, 0 };

// Program Counter State
int counter = 0; // Program Counter Index
unsigned long previousTime = 0;

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
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);
  pinMode(ledPins[3], OUTPUT);

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
//ledColorU(purple);

  // Dark is just off. Use dark to turn any LED off.
//  ledColor(dark, dark, dark, dark);
  ledColor(buttonColorState[0], buttonColorState[1], buttonColorState[2], buttonColorState[3]);
  
  //
  // Read button states
  //
  
  // Read Button 1
  readButtonState(0);
  readButtonState(1);
  readButtonState(2);
  readButtonState(3);
  
  //
  // Update counter column
  //
  unsigned long currentTime = millis();
  if (currentTime - previousTime > 500) {
    previousTime = currentTime;
    if (counter == 0) {
      counter = 1;
    } else {
      counter = 0;
    }
  }
  
  //
  // Update color based on button state
  //
  
  updateButtonColor(0, blue);
  updateButtonColor(1, blue);
  updateButtonColor(2, blue);
  updateButtonColor(3, blue);

  //
  // Check for button presses and output states
  // Enable if you want to test your buttons
  //
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

void updateButtonColor(int i, int color[]) {
  
  if (buttonSwitchState[i] == 0) { // Check if button state is "off"
  
    buttonColorState[i][0] = 0;
    buttonColorState[i][1] = 0;
    buttonColorState[i][2] = 0;
    
  } else { // Check if button state is "on"
  
    if (counter == 0 && ((i == 0 || i == 1))) {
      // Counter is in left column
      buttonColorState[i][0] = 255;
      buttonColorState[i][1] = 255;
      buttonColorState[i][2] = 255;
    } else if (counter == 1 && ((i == 2 || i == 3))) {
      buttonColorState[i][0] = 255;
      buttonColorState[i][1] = 255;
      buttonColorState[i][2] = 255;
    } else { // Counter is in right column
      // Counter is in left column
      buttonColorState[i][0] = color[0];
      buttonColorState[i][1] = color[1];
      buttonColorState[i][2] = color[2];
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
  digitalWrite(ledPins[0], LOW); // "Turn on" LED
  // Flicker control
  delayMicroseconds(1100); // Wait (for POV effect?)
  digitalWrite(ledPins[0], HIGH); // "Turn off" LED

  // LED 2
  analogWrite(redLED, led2[0]);
  analogWrite(greenLED, led2[1]);
  analogWrite(blueLED, led2[2]);
  // Flicker control
  delay(2);
  digitalWrite(ledPins[1], LOW);
  // Flicker control
  delayMicroseconds(1100);
  digitalWrite(ledPins[1], HIGH);

  // LED 3
  analogWrite(redLED, led3[0]);
  analogWrite(greenLED, led3[1]);
  analogWrite(blueLED, led3[2]);
  // Flicker control
  delay(2);
  digitalWrite(ledPins[2], LOW);
  // Flicker control
  delayMicroseconds(1100);
  digitalWrite(ledPins[2], HIGH);

  // LED 4
  analogWrite(redLED, led4[0]);
  analogWrite(greenLED, led4[1]);
  analogWrite(blueLED, led4[2]);
  // Flicker control
  delay(2);
  digitalWrite(ledPins[3], LOW);
  // Flicker control
  delayMicroseconds(1100);
  digitalWrite(ledPins[3], HIGH);
}

// Uniform color
// This doesn't appear to work well with mixed colors.
void ledColorU(int color[]) {
  analogWrite(redLED, color[0]);
  analogWrite(greenLED, color[1]);
  analogWrite(blueLED, color[2]);
  digitalWrite(ledPins[0], LOW);
  digitalWrite(ledPins[1], LOW);
  digitalWrite(ledPins[2], LOW);
  digitalWrite(ledPins[3], LOW); 
}
