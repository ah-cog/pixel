// Michael Gubbels
// Date: 2013-07-21 (Started, in San Francisco at Pacific Tradewinds Hostel in Chinatown.)

// RGB pins (PWM pins)
const int redLED   = 9;
const int greenLED = 10;
const int blueLED  = 6;

// Button Grounds
const int buttonMatrix1 = 12; // BL
const int buttonMatrix2 = 7;  // BR
const int buttonMatrix3 = 13; // TL
const int buttonMatrix4 = 11; // TR

// LED Grounds (LED "Top" is the side with the wires coming out, going to the Arduino.)
const int ledGnd1 = 2; // BL
const int ledGnd2 = 3; // BR
const int ledGnd3 = 8; // TL
const int ledGnd4 = 5; // TR

// Color definitions
int red[]    = { 255, 0, 0 };
int green[]  = { 0, 255, 0 };
int blue[]   = { 0, 0, 255 };
int purple[] = { 255, 0, 150 };
int yellow[] = { 255, 255, 0 };
int dark[]   = { 0, 0, 0 };

void setup() {
  
  // Switch Grounds
  pinMode(buttonMatrix1, INPUT);
  pinMode(buttonMatrix2, INPUT);
  pinMode(buttonMatrix3, INPUT);
  pinMode(buttonMatrix4, INPUT);

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
  ledColor(yellow, red, purple, green);
//  ledColor(dark, blue, blue, green);

  // Uniform color. Pass in the color array of your choice.
//  ledColorU(purple);

  // Dark is just off. Use dark to turn any LED off.
  //ledColor(dark, dark, dark, dark);

  // Check for button presses and output states
  // Enable if you want to test your buttons
  Serial.print(digitalRead(buttonMatrix1));
  Serial.print("\t");
  Serial.print(digitalRead(buttonMatrix2));
  Serial.print("\t");
  Serial.print(digitalRead(buttonMatrix3));
  Serial.print("\t");
  Serial.println(digitalRead(buttonMatrix4));
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
