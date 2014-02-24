#include <SoftwareSerial.h>
#include <RadioBlock.h>

// Get hacky. Get happy.
// It's hacky and it's happy. It's a proof of concept and a labor of love.

#include "pitches.h"

#define ENABLE_SERIAL 0

#define RELAY_ENABLE_PIN 12


// Button Module
const int buttonPins[4] = { 7, 8, 12, 13 };

#define SPEAKER_PIN A4

unsigned char buttonState = 0x00;
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

// Button State
bool updateState = false;
int buttonReadyState[4] = { 1, 1, 1, 1 };
//int buttonSwitchState[4] = { 0, 0, 0, 0 };
int buttonColorState[4][3] = { 
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 }
};
int buttonNote[4] = { NOTE_C5, NOTE_C5, NOTE_C3, NOTE_C3 };

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
bool counterDone = false;

//Pins connected to RadioBlock pins 1/2/3/4
RadioBlockSerialInterface interface = RadioBlockSerialInterface(5, 4, 3, 2);

//uint8_t payload[] = { 2 };

//#define CODE_TEMP   1
//#define CODE_ALARM  2
//#define CODE_LED    3
//#define CODE_VALVE  4

// Set our known network addresses. How do we deal with unexpected nodes...?
#define MODULE_ID 0
#if MODULE_ID == 0
  #define OUR_ADDRESS   0x1000
  #define THEIR_ADDRESS 0x1001
#elif MODULE_ID == 1
  #define OUR_ADDRESS   0x1001
  #define THEIR_ADDRESS 0x1000
#endif

// Speakers:

// notes in the melody:
int melody[] = { NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4 };

// Startup Jingle (note durations: 4 = quarter note, 8 = eighth note, etc.)
//int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };
int noteDurations[] = { 8, 8, 8 };

//uint8_t packetData[] = ;

void setup() {
  
  pinMode(RELAY_ENABLE_PIN, OUTPUT);
  
  
  
  
  // Set up button module
  pinMode(buttonPins[0], INPUT);
  pinMode(buttonPins[1], INPUT);
  pinMode(buttonPins[2], INPUT);
  pinMode(buttonPins[3], INPUT);
  
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
  interface.setAddress(OUR_ADDRESS); // TODO: Dynamically set address based on other address in the area (and extended address space from shared state, and add collision fixing.)

//  if (ENABLE_SERIAL) {
    //Serial.begin(9600); 
    Serial.begin(115200); 
    Serial.println("Starting...");
//  }
  
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 3; thisNote++) {

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
  getButtonState(0);
  getButtonState(1);
  getButtonState(2);
  getButtonState(3);
  
  if (!counterDone) {
    counterDone = true;
    generateSound(0);
    generateSound(1);
    generateSound(2);
    generateSound(3);
  }
  
  if (ENABLE_SERIAL) {
  Serial.print("Button State: ");
  Serial.println(buttonState, BIN);
  
  Serial.println(OUR_ADDRESS);  Serial.println(OUR_ADDRESS);  Serial.println(OUR_ADDRESS);  Serial.println(OUR_ADDRESS);
  }
  
  
  
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
    counterDone = false;
  }
  
//  Serial.print("CCOOUUNNTTEERR: ");
//  Serial.println(counter, DEC);
  
  
  updateButtonColor(0, blue);
  updateButtonColor(1, blue);
  updateButtonColor(2, blue);
  updateButtonColor(3, blue);
  
  
  
//  if (MODULE_ID == 0) {

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
  
      Serial.print("updateState = ");
    Serial.println(updateState);
  
  if (updateState) {

    updateState = false;
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
    
    if (ENABLE_SERIAL) {
    Serial.println("Data sent.");
    }
    delay(1200);
  }

//  }





//  if (MODULE_ID == 1) {

  // Read an incoming packet if available within the specified number of milliseconds (the timeout value).
  if (interface.readPacket(5)) { // NOTE: Every time this is called, the response returned by getResponse() is overwritten.
    if (ENABLE_SERIAL) {
    Serial.println("Received a packet:");
    }
    
    // Get error code for response
    if (interface.getResponse().getErrorCode() == APP_STATUS_SUCESS) {
      if (ENABLE_SERIAL) {
      Serial.println("Success: Good packet.");
      }
    } else {
      if (ENABLE_SERIAL) {
      Serial.println("Failure: Bad packet.");
      }
    }
    if (ENABLE_SERIAL) {
     Serial.print("Len: ");
     Serial.print(interface.getResponse().getPacketLength(), DEC);
     Serial.print(", Command: ");
     Serial.print(interface.getResponse().getCommandId(), HEX);
     Serial.print(", CRC: ");
     Serial.print(interface.getResponse().getCrc(), HEX); // Cyclic redundancy check (CRC) [Source: http://en.wikipedia.org/wiki/Cyclic_redundancy_check]
     Serial.println("");
     }

     //
     // Parse Frame Data
     //
     
     // General command format (sizes are in bytes), Page 4:
     // | Start Byte (1) | Size (1) | Payload (Variable) | CRC (2) |
     
     // COMMAND ID   MEANING
     // 0x20         This command is used to send data over the network, Page 13
     
     int frameDataLength = 0;
     int sendMethod = -1;
     // Send method will be:
     // 0 = unknown
     // 1 = sendData()
     // 2 = sendMessage()
     
     int commandId = -1;
     unsigned int codeAndType = 0;
     unsigned int payloadCode = 0;
     unsigned int payloadDataType = 0;
     
     // We can use this to determine which commands the sending unit used to construct the packet:
     // If the length == 6, the sender used sendData()
     // If the length > 6, the sender used setupMessage(), addData(), and sendMessage() 
     //
     // If the sender used the second method, we need to do more parsing of the payload to pull out
     // the sent data. See "Data or start of payload" below at array offset of 5.
     
     frameDataLength = interface.getResponse().getFrameDataLength();
     if (ENABLE_SERIAL) {
       Serial.print("Length of Frame Data: ");
       Serial.println(frameDataLength);
     }
     
     // Get the method the sending unit used to construct the packet
     if (frameDataLength == 6) {
       sendMethod = 0; // The sender used sendData()
     } else if (frameDataLength > 6) {
       sendMethod = 1; // The sender used setupMessage(), addData(), and sendMessage()
     }
     
     // The following "meanings" for these bytes are from page 15 of the SimpleMesh_Serial_Protocol.pdf from Colorado Micro Devices.
     if (ENABLE_SERIAL) {
     Serial.println("Frame Data: ");
     }
     
     //Serial.println(interface.getResponse().getFrameData()[0], HEX);
     //Serial.println(interface.getResponse().getFrameData()[1]);
     //Serial.println(interface.getResponse().getFrameData()[2]);
     
     // Process message based on the its Command ID
     
     commandId = interface.getResponse().getCommandId();
     if (commandId == 0x22) { //APP_COMMAND_DATA_IND) { // 0x22
       if (ENABLE_SERIAL) {
       Serial.print("  Source address: ");
       Serial.println(interface.getResponse().getFrameData()[1], HEX); // Source address
       
       Serial.print("  Frame options: ");
       // 0x00 None
       // 0x01 Acknowledgment was requested
       // 0x02 Security was used
       Serial.println(interface.getResponse().getFrameData()[2], HEX); // Frame options
       
       Serial.print("  Link Quality Indicator: ");
       Serial.println(interface.getResponse().getFrameData()[3], HEX); // Link quality indicator
       
       Serial.print("  Received Signal Strength Indicator: ");
       Serial.println(interface.getResponse().getFrameData()[4], HEX); // Received Signal Strength Indicator
       }
       
       // Parse Data or Payload:
       
       if (sendMethod == 0) {
         if (ENABLE_SERIAL) {
         Serial.print("  Sent Data: ");
         Serial.println(interface.getResponse().getFrameData()[5], HEX);
         }
       } else if (sendMethod == 1) {
         codeAndType = interface.getResponse().getFrameData()[5]; 
         
         
         if (ENABLE_SERIAL) {
         Serial.print(" Encoded send code and original data type: ");
         Serial.println(codeAndType, HEX); // The actual data
         }
         
         payloadDataType = codeAndType & 0xf;
         payloadCode = (codeAndType >> 4) & 0xf;
         
         if (ENABLE_SERIAL) {
         Serial.print("  The sent code was (in hex): ");
         Serial.println(payloadCode, HEX);
         Serial.print("  The original data type was: ");
         Serial.println(payloadDataType);
         }
         
         if (payloadDataType == 1) {
           if (ENABLE_SERIAL) {
           Serial.println("   Data type is TYPE_UINT8. Data:");
           Serial.print("    The data: ");
           Serial.println(interface.getResponse().getFrameData()[6]);
           }
           
           
           // TODO: CHANGE THIS!!! HACKY!!!!
           
           buttonState = interface.getResponse().getFrameData()[6];
           
           
         } else if (payloadDataType == 2) {
           if (ENABLE_SERIAL) {
           Serial.println("   Data type is TYPE_INT8. High and low bytes:");
           Serial.print("    High part: ");
           Serial.println(interface.getResponse().getFrameData()[6]); 
           Serial.print("    Low part: ");
           Serial.println(interface.getResponse().getFrameData()[7]);
           }
         } else if (payloadDataType == 3) {
           Serial.println("   Data type is TYPE_UINT16. High and low bytes:");
           Serial.print("    High part: ");
           Serial.println(interface.getResponse().getFrameData()[6]); 
           Serial.print("    Low part: ");
           Serial.println(interface.getResponse().getFrameData()[7]);
         } else if (payloadDataType == 4) {
           Serial.println("   Data type is TYPE_INT16. High and low bytes:");
           Serial.print("    High part: ");
           Serial.println(interface.getResponse().getFrameData()[6]); 
           Serial.print("    Low part: ");
           Serial.println(interface.getResponse().getFrameData()[7]);
         } else if (payloadDataType == 5) {
           Serial.println("   Data type is TYPE_UINT32. Four bytes:");
           Serial.print("    MSB: ");
           Serial.println(interface.getResponse().getFrameData()[6]); 
           Serial.print("    : ");
           Serial.println(interface.getResponse().getFrameData()[7]);
           Serial.print("    :");
           Serial.println(interface.getResponse().getFrameData()[8]);
           Serial.print("    LSB:");
           Serial.println(interface.getResponse().getFrameData()[9]);
         } else {
           Serial.println("   Data type is not coded for yet...");
           // Debugging: 
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[6]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[7]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[8]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[9]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[10]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[11]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[12]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[13]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[14]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[15]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[16]);
           // End debugging
         }
                 
       }
     }
  }
  
//  }
}




void getButtonState(int i) {
  
  unsigned char buttonBitFlag = 0x00;
  if (i == 0) {
    buttonBitFlag = 0x01;
  } else if (i == 1) {
    buttonBitFlag = 0x02;
  } else if (i == 2) {
    buttonBitFlag = 0x04;
  } else if (i == 3) {
    buttonBitFlag = 0x08;
  }
  
  // Get button input state
  unsigned int buttonInput = LOW;
  
  // Button i
  if (buttonReadyState[i]) {
    buttonReadyState[i] = 0;
    
    buttonInput = digitalRead(buttonPins[i]);
    
    if (buttonInput == HIGH) {
      //if (buttonState & 0x01) {
      if (buttonState & buttonBitFlag) {
        buttonState = buttonState & ~(1 << i);
        buttonColorState[i][0] = 0;
//        buttonSwitchState[i] = 0;
      } else {
        buttonState = buttonState | buttonBitFlag;
        buttonColorState[i][0] = 255;
//        buttonSwitchState[i] = 1;
      }
      updateState = true;
      
//      // Play sound for button
//      int noteDuration = 1000 / noteDurations[0];
//      tone(SPEAKER_PIN, buttonNote[i], noteDuration);
    } else {
      // NONE?
    }
  } else {
    // Button is not 
    if (!digitalRead(buttonPins[i])) {
      buttonReadyState[i] = 1;
    }
  }
}

void generateSound(int i) {
  
  unsigned char buttonBitFlag = 0x00;
  if (i == 0) {
    buttonBitFlag = 0x01;
  } else if (i == 1) {
    buttonBitFlag = 0x02;
  } else if (i == 2) {
    buttonBitFlag = 0x04;
  } else if (i == 3) {
    buttonBitFlag = 0x08;
  }
  
  //if (buttonSwitchState[i] == 0) { // Check if button state is "off"
  if ((buttonState & buttonBitFlag) == 0) {
  
    buttonColorState[i][0] = 0;
    buttonColorState[i][1] = 0;
    buttonColorState[i][2] = 0;
    
  } else { // Check if button state is "on"
  
    if (counter == 0 && ((i == 0 || i == 2))) {
      // Counter is in right column
      // Play sound for button
      int noteDuration = 1000 / noteDurations[0];
      tone(SPEAKER_PIN, buttonNote[i], noteDuration);
    } else if (counter == 1 && ((i == 1 || i == 3))) {
      // Counter is in left column
      // Play sound for button
      int noteDuration = 1000 / noteDurations[0];
      tone(SPEAKER_PIN, buttonNote[i], noteDuration);
    } else {
      // Counter is in non-existent (invalid) column
//      buttonColorState[i][0] = color[0];
//      buttonColorState[i][1] = color[1];
//      buttonColorState[i][2] = color[2];
    }
  }
  
}

void updateButtonColor(int i, int color[]) {
  
  unsigned char buttonBitFlag = 0x00;
  if (i == 0) {
    buttonBitFlag = 0x01;
  } else if (i == 1) {
    buttonBitFlag = 0x02;
  } else if (i == 2) {
    buttonBitFlag = 0x04;
  } else if (i == 3) {
    buttonBitFlag = 0x08;
  }
  
  //if (buttonSwitchState[i] == 0) { // Check if button state is "off"
  if ((buttonState & buttonBitFlag) == 0) {
  
    buttonColorState[i][0] = 0;
    buttonColorState[i][1] = 0;
    buttonColorState[i][2] = 0;
    
  } else { // Check if button state is "on"
  
    if (counter == 0 && ((i == 0 || i == 2))) {
      // Counter is in left column
      buttonColorState[i][0] = 255;
      buttonColorState[i][1] = 255;
      buttonColorState[i][2] = 255;
    } else if (counter == 1 && ((i == 1 || i == 3))) {
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
