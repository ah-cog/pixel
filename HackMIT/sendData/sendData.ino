#include <SoftwareSerial.h>
#include <RadioBlock.h>

#include "pitches.h"

// Button Module
#define MODULE_BUTTON_0_PIN 7
#define MODULE_BUTTON_1_PIN 8
#define MODULE_BUTTON_2_PIN 12
#define MODULE_BUTTON_3_PIN 13

#define SPEAKER_PIN A4

unsigned char buttonState = 0x00;
bool colorLock = false;
// TL: 0b00000001
// TR: 0b00000010
// BL: 0b00000100
// BR: 0b00001000


// RGB Pins (PWM Pins)
const int redLED   = 9;
const int blueLED  = 10;
const int greenLED = 11;

// Button Grounds
const int ledPins[4] = { A0, A1, A2, A3 };

int buttonColorState[4][3] = { 
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 }
};

// Color definitions
int red[]    = { 255, 0, 0 };
int green[]  = { 0, 255, 0 };
int blue[]   = { 0, 0, 255 };
int purple[] = { 255, 0, 150 };
int yellow[] = { 255, 255, 0 };
int dark[]   = { 0, 0, 0 };

//Pins connected to RadioBlock pins 1/2/3/4
RadioBlockSerialInterface interface = RadioBlockSerialInterface(5, 4, 3, 2);

//uint8_t payload[] = { 2 };

#define CODE_TEMP   1
#define CODE_ALARM  2
#define CODE_LED    3
#define CODE_VALVE  4

#define OUR_ADDRESS   0x1000
#define THEIR_ADDRESS 0x1001

// Speakers:

// notes in the melody:
int melody[] = { NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4 };

// Startup Jingle (note durations: 4 = quarter note, 8 = eighth note, etc.)
int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };

//uint8_t packetData[] = ;

void setup() {
  // Set up button module
  pinMode(MODULE_BUTTON_0_PIN, INPUT);
  pinMode(MODULE_BUTTON_1_PIN, INPUT);
  pinMode(MODULE_BUTTON_2_PIN, INPUT);
  pinMode(MODULE_BUTTON_3_PIN, INPUT);
  
  // RGB Pins
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  // RGB Pin GNDs
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);
  pinMode(ledPins[3], OUTPUT);
  
  // Set up RadioBlock module
  interface.begin();  

  //Give RadioBlock time to init
  delay(500);
  
  // Blink lights to indicate that the device is initializing. It's alive!
  interface.setLED(true);
  delay(25);
  interface.setLED(false);
  delay(25);
  interface.setLED(true);
  delay(25);
  interface.setLED(false);
  delay(25);
  interface.setLED(true);
  delay(25);
  interface.setLED(false);
  
  // We need to set these values so other RadioBlocks can find us
  interface.setChannel(15);
  interface.setPanID(0xBAAD);
  interface.setAddress(OUR_ADDRESS);
    
  Serial.begin(9600); 
  Serial.println("Starting...");
  
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(SPEAKER_PIN, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(SPEAKER_PIN);
  }
}

void loop() { // run over and over

  //ledColor(yellow, red, purple, green);
  ledColor(buttonColorState[0], buttonColorState[1], buttonColorState[2], buttonColorState[3]);

  // Get button input state
  unsigned int buttonInput = LOW;
  // Button 0
  buttonInput = digitalRead(MODULE_BUTTON_0_PIN);
  if (buttonInput == HIGH) {
    buttonState = buttonState | 0x01;
    buttonColorState[0][0] = 255;
    
    // Play sound for button
    int noteDuration = 1000 / noteDurations[0];
    tone(SPEAKER_PIN, NOTE_C5, noteDuration);
  } else {
    buttonState = buttonState & ~(1 << 0);
    buttonColorState[0][0] = 0;
  }
  // Button 1
  buttonInput = digitalRead(MODULE_BUTTON_1_PIN);
  if (buttonInput == HIGH) {
    buttonState = buttonState | 0x02;
    buttonColorState[1][0] = 255;
    
    // Play sound for button
    int noteDuration = 1000 / noteDurations[0];
    tone(SPEAKER_PIN, NOTE_C5, noteDuration);
  } else {
    buttonState = buttonState & ~(1 << 1);
    buttonColorState[1][0] = 0;
  }
  // Button 2
  buttonInput = digitalRead(MODULE_BUTTON_2_PIN);
  if (buttonInput == HIGH) {
    buttonState = buttonState | 0x04;
    buttonColorState[2][1] = 255;
    
    // Play sound for button
    int noteDuration = 1000 / noteDurations[3];
    tone(SPEAKER_PIN, melody[3], noteDuration);
  } else {
    buttonState = buttonState & ~(1 << 2);
    buttonColorState[2][1] = 0;
  }
  // Button 3
  buttonInput = digitalRead(MODULE_BUTTON_3_PIN);
  if (buttonInput == HIGH) {
    buttonState = buttonState | 0x08;
    buttonColorState[3][1] = 255;
    
    // Play sound for button
    int noteDuration = 1000 / noteDurations[3];
    tone(SPEAKER_PIN, melody[3], noteDuration);
  } else {
    buttonState = buttonState & ~(1 << 3);
    buttonColorState[3][1] = 0;
  }
  
  Serial.print("Button State: ");
  Serial.println(buttonState, BIN);

  //New Message
//  if (interface.getResponse().isAvailable()) {
//    Serial.println("New Response...");
//  }
//  
//  //Oops?
//  if (interface.getResponse().isError()) {
//    Serial.println("You had ONE job Arduino. ONE job.");
//  }
//  
//  delay(2000);
  
  //We use the 'setupMessage()' call if we want to use a bunch of data,
  //otherwise can use sendData() calls to directly send a few bytes
  
  //This is the OTHER guys address
  interface.setupMessage(THEIR_ADDRESS);
  
// (REMOVE:)  interface.getResponse().setCommandId(APP_COMMAND_DATA_REQ);
  
  //Send temperature reading
//  interface.addData(CODE_TEMP, analogRead(A0));
  
  //Send state of pot (potentimeter, not drug manufacturing)
//  interface.addData(CODE_VALVE, analogRead(1));
  
  //Toggle other other guys LED on RadioBlock
//  interface.addData(CODE_LED, 1);

//  char a_char = 0x04; // BUG: This type doesn't work right.
  interface.addData(0x1, buttonState); 
//  interface.addData(0xf, a_char); 
//  interface.addData(0xf, a_char);
//  interface.addData(0xf, a_char);
//  interface.addData(0xf, a_char);
  
//  Serial.print("Command ID: ");
//  Serial.println(interface.getResponse().getCommandId(), HEX);
                    
//  setFrameLength
//  setFrameData

  // Add frame data
//  interface.addData(0x1, a_uchar);
 
  // Send data over the air (OTA)
  interface.sendMessage(); 
  
  Serial.println("Data sent.");
  delay(1000);
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
