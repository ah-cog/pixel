/*
ADXL3xx Accelerometer Notes:
 
Reads an Analog Devices ADXL3xx accelerometer and communicates the
acceleration to the computer.  The pins used are designed to be easily
compatible with the breakout boards from Sparkfun, available from:

 http://www.sparkfun.com/commerce/categories.php?c=80
 http://www.arduino.cc/en/Tutorial/ADXL3xx
 http://learn.adafruit.com/adafruit-analog-accelerometer-breakouts/programming

The circuit:
 analog 0: accelerometer self test
 analog 1: z-axis
 analog 2: y-axis
 analog 3: x-axis
 analog 4: ground
 analog 5: vcc

Thanks to Michael Smith-Welch, the folks in The Tinkering Studio at the 
Exploratorium in California. Thanks to the IDC community for their inspiration.

Thanks to attendees and sponsors of HackMIT 2013 and HackRU 2013 for 
valuable inspirational conversations. Likewise, thanks to fellow 
members of Terrapin Hackers and Startup Shell.

This example is based on code in the public domain by 
David A. Mellis and Tom Igoe, Adafruit, SparkFun.
*/

#include <MovingAvarageFilter.h>
#include <RadioBlock.h>
#include <SoftwareSerial.h>

#define ENABLE_DEBUG 1

#define MAKEY_INPUT_PIN A0
#define ACCELEROMETER_X_PIN A3
#define ACCELEROMETER_Y_PIN A2
#define ACCELEROMETER_Z_PIN A1
#define RELAY_ENABLE_PIN 12

MovingAvarageFilter movingAvarageFilter(20);

boolean check = false; // This is for the MAKEY_INPUT_PIN

//
// Set up RadioBlocks
//

// Set our known network addresses. How do we deal with 
// unexpected nodes...? This should be dynamic, and nodes should
// self-assign their addresses and broadcast the the mesh network.
#define MODULE_ID 2
#if MODULE_ID == 2
  #define OUR_ADDRESS   0x1002
  #define THEIR_ADDRESS 0x1003
#elif MODULE_ID == 3
  #define OUR_ADDRESS   0x1003
  #define THEIR_ADDRESS 0x1002
#endif

// The module's pins 1, 2, 3, and 4 are connected to Arduino's pins 5, 4, 3, and 2.
RadioBlockSerialInterface interface = RadioBlockSerialInterface(5, 4, 3, 2);

void setup() {
  // Set pin modes
  pinMode(RELAY_ENABLE_PIN, OUTPUT);
  
  // We need to set these values so other RadioBlocks can find us
  interface.setChannel(15);
  interface.setPanID(0xBAAD);
  interface.setAddress(OUR_ADDRESS); // TODO: Dynamically set address based on other address in the area (and extended address space from shared state, and add collision fixing.)
  
  Serial.begin(115200);
}

void loop() {
  
  //
  // Check for node input
  //
  
  // Declare input and output variables
  float input =  analogRead(MAKEY_INPUT_PIN); // without a real input, looking at the step respons (input at unity, 1)
  float averageInputValue = 0;

  averageInputValue = movingAvarageFilter.process(input);

  // Call the fir routine with the input. The value 'fir' spits out is stored in the output variable.
  
  if (averageInputValue < 400) { // Change this parameter to fine tune the sensitivity
    if (!check){         
      // Keyboard.print("d");
      digitalWrite(RELAY_ENABLE_PIN, HIGH);
      Serial.println(averageInputValue);           
      check = !check;   
    }         
  }
  
  if (averageInputValue > 600) {     
    if (check) {
      check = !check;
      digitalWrite(RELAY_ENABLE_PIN, LOW);  
    }     
  }
  
  
  //
  // Print accelerometer data over serial
  //
  
  // Print the accelerometer sensor values:
  Serial.print(analogRead(ACCELEROMETER_X_PIN));
  Serial.print("\t");
  Serial.print(analogRead(ACCELEROMETER_Y_PIN));
  Serial.print("\t");
  Serial.print(analogRead(ACCELEROMETER_Z_PIN));
  Serial.println();
  
  
  //
  // Send state to other module
  //
  
  // This is the other node's address
  
  interface.setupMessage(THEIR_ADDRESS);
  
  unsigned char stateMessage = 0x00;
  // TL: 0b00000001
  // TR: 0b00000010
  // BL: 0b00000100
  // BR: 0b00001000
  interface.addData(0x1, stateMessage);
  
  // Send data over the air (OTA)
  interface.sendMessage();
  
  delay(1200);
  
  
  
  
  
  
  
  // Read an incoming packet if available within the specified number of milliseconds (the timeout value).
  if (interface.readPacket(5)) { // NOTE: Every time this is called, the response returned by getResponse() is overwritten.
    if (ENABLE_DEBUG) {
    Serial.println("Received a packet:");
    }
    
    // Get error code for response
    if (interface.getResponse().getErrorCode() == APP_STATUS_SUCESS) {
      if (ENABLE_DEBUG) {
      Serial.println("Success: Good packet.");
      }
    } else {
      if (ENABLE_DEBUG) {
      Serial.println("Failure: Bad packet.");
      }
    }
    if (ENABLE_DEBUG) {
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
     if (ENABLE_DEBUG) {
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
     if (ENABLE_DEBUG) {
       Serial.println("Frame Data: ");
     }
     
     //Serial.println(interface.getResponse().getFrameData()[0], HEX);
     //Serial.println(interface.getResponse().getFrameData()[1]);
     //Serial.println(interface.getResponse().getFrameData()[2]);
     
     // Process message based on the its Command ID
     
     commandId = interface.getResponse().getCommandId();
     if (commandId == 0x22) { //APP_COMMAND_DATA_IND) { // 0x22
       if (ENABLE_DEBUG) {
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
         if (ENABLE_DEBUG) {
         Serial.print("  Sent Data: ");
         Serial.println(interface.getResponse().getFrameData()[5], HEX);
         }
       } else if (sendMethod == 1) {
         codeAndType = interface.getResponse().getFrameData()[5]; 
         
         
         if (ENABLE_DEBUG) {
           Serial.print(" Encoded send code and original data type: ");
           Serial.println(codeAndType, HEX); // The actual data
         }
         
         payloadDataType = codeAndType & 0xf;
         payloadCode = (codeAndType >> 4) & 0xf;
         
         if (ENABLE_DEBUG) {
           Serial.print("  The sent code was (in hex): ");
           Serial.println(payloadCode, HEX);
           Serial.print("  The original data type was: ");
           Serial.println(payloadDataType);
         }
         
         if (payloadDataType == 1) {
           if (ENABLE_DEBUG) {
           Serial.println("   Data type is TYPE_UINT8. Data:");
           Serial.print("    The data: ");
           Serial.println(interface.getResponse().getFrameData()[6]);
           }
           
           
           // TODO: CHANGE THIS!!! HACKY!!!!
           
           stateMessage = interface.getResponse().getFrameData()[6];
           
           
         } else if (payloadDataType == 2) {
           if (ENABLE_DEBUG) {
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
}
