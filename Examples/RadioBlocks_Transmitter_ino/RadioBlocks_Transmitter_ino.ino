#include <SoftwareSerial.h>
#include <RadioBlock.h>

//Pins connected to RadioBlock pins 1/2/3/4
//RadioBlockSerialInterface interface = RadioBlockSerialInterface(-1, -1, 7, 8);
RadioBlockSerialInterface interface = RadioBlockSerialInterface(-1, -1, 8, 7);
//RadioBlockSerialInterface interface = RadioBlockSerialInterface(5, 4, 3, 2);

uint8_t payload[] = { 2 };

#define CODE_TEMP   1
#define CODE_ALARM  2
#define CODE_LED    3
#define CODE_VALVE  4

#define OUR_ADDRESS   0x0002
#define THEIR_ADDRESS 0x0001

// These #define's are copied from the RadioBlock.cpp file
#define TYPE_UINT8 	1
#define TYPE_INT8	2
#define	TYPE_UINT16	3
#define TYPE_INT16	4
#define TYPE_UINT32	5
#define TYPE_INT32	6
#define TYPE_UINT64	7
#define TYPE_INT64	8
#define TYPE_FLOAT	9
#define TYPE_FIXED8_8	10
#define TYPE_FIXED16_8	11
#define TYPE_8BYTES	12
#define TYPE_16BYTES	13
#define TYPE_ASCII	14

unsigned long lastCount = 0;

void setup()  
{
  delay(1000);
  
  interface.begin();  

  //Give RadioBlock time to init
  delay(500);

  //Tell the world we are alive  
  interface.setLED(true);
  delay(1000);
  interface.setLED(false);
  
  interface.setChannel(15);
  interface.setPanID(0xBAAD);
  interface.setAddress(OUR_ADDRESS);
    
  Serial.begin(9600); 
  while (!Serial) { } // wait for serial port to connect. Needed for Leonardo only
  Serial.println("Starting...");
}

#define RADIOBLOCK_PACKET_READ_TIMEOUT 1000
#define PAYLOAD_START_INDEX 5 // Index of the first byte in the payload

void loop() // run over and over
{ 
  //New Message
//  if (interface.getResponse().isAvailable()) {
//    Serial.println("New Response...");
//  }

  if (interface.readPacket(RADIOBLOCK_PACKET_READ_TIMEOUT)) { // Waits a maximum of <i>timeout</i> milliseconds for a response packet before timing out; returns true if packet is read. Returns false if timeout or error occurs.
          // interface.readPacket(); // Read the packet (NOTE: Seemingly must do this for isAvailable() to work properly.)
          // if (interface.getResponse().isAvailable()) {
          // TODO: Change this to interface.isAvailable() so it doesn't block and wait for any time, so it just reads "when available" (in quotes because it's doing polling)
  
          // General command format (sizes are in bytes), Page 4:
          //
          // [ Start Byte (1) | Size (1) | Payload (Variable) | CRC (2) ]
          //
          // - Start Byte (1 byte)
          // - Size (1 byte): Size of the command id + options + payload field
          // - Payload (Variable): command payload
          // - CRC (2 bytes): 16 bit CRC calculated over payload with initial value 0x1234 (i.e., it is 
          //                  calculated over the command id, options, and payload if any.)
  
          // Data request command format (this is the Payload field in the command format):
          // 
          // [ Command ID (1) | Destination (2) | Options (1) | Handle (1) ]
          //
          // NOTE: I believe this constitutes a "frame".
  
          // Data response command format (this is the Payload field in the command format):
          // 
          // [ Command ID (1) | Source Address (2) | Options (1) | LQI (1) | RSSI (1) | Payload (Variable) ]
          //
          // NOTE: I believe this constitutes a "frame".f
  
          if (interface.getResponse().getErrorCode() == APP_STATUS_SUCESS) {
              Serial.println("\n\n\n\nReceived packet.");
  
              int commandId = interface.getResponse().getCommandId();
  
              if (commandId == APP_COMMAND_ACK) {
                  // "Every command is confirmed with an acknowledgment command even if it is impossible
                  //   to immediately execute the command. There is no particular order in which responses 
                  //   are sent, so for example Data Indication Command might be sent before 
                  //   Acknowledgment Command."
  
                  // Acknowledgment command format: Page 5 of SimpleMesh Serial Protocol document.
  
                  Serial.println("Received Ack");
  
                  Serial.print("  Status: ");
                  Serial.println(interface.getResponse().getFrameData()[0], HEX); // Source address
  
              } else if (commandId == APP_COMMAND_DATA_IND) { // (i.e., 0x22) [Page 15]
  
                Serial.print(interface.getResponse().getFrameData()[0], HEX); // Frame options
                Serial.print(" | ");
                Serial.print(interface.getResponse().getFrameData()[1], HEX); // Frame options
                Serial.print(" | ");
                Serial.print(interface.getResponse().getFrameData()[2], HEX); // Frame options
                Serial.print(" | ");
                Serial.print(interface.getResponse().getFrameData()[3], HEX); // Frame options
                Serial.print(" | ");
                Serial.print(interface.getResponse().getFrameData()[4], HEX); // Frame options
  
  
                  int frameDataLength = interface.getResponse().getFrameDataLength();
                  //        Serial.print("  Payload length: ");
                  //        Serial.println(frameDataLength, DEC); // "Payload" length (minus "Command Id" (1 byte))
  
                  //        Serial.print("  Frame Command Id: ");
                  //        Serial.println(interface.getResponse().getFrameData()[0], HEX); // Frame options
  
                  // Compute source address using bitwise operators (combine two bytes into a "short", a 16-bit integer data type)
                  short sourceAddress = interface.getResponse().getFrameData()[0] | ((short) interface.getResponse().getFrameData()[1] << 8);
                  Serial.print("  Source address: ");
                  Serial.println(sourceAddress, HEX); // Source address
  
                  // Frame Options [Page 15]
                  // i.e.,
                  // 0x00 None
                  // 0x01 Acknowledgment was requested
                  // 0x02 Security was used
                  //        Serial.print("  Frame options: ");
                  //        Serial.println(interface.getResponse().getFrameData()[2], HEX); // Frame options
                  //  
                  //        Serial.print("  Link Quality Indicator: ");
                  //        Serial.println(interface.getResponse().getFrameData()[3], HEX); // Link quality indicator
                  //  
                  //        Serial.print("  Received Signal Strength Indicator: ");
                  //        Serial.println(interface.getResponse().getFrameData()[4], HEX); // Received Signal Strength Indicator
  
                  // NOTE: Payload starts at index position 5
  
                  //
                  // Parse payload data
                  //
  
                  //        int PAYLOAD_START_INDEX = 5; // Index of first byte of "Payload"
  
                  //        Serial.print("Raw Payload Data: [ ");
                  //        for (int i = 0; i < (frameDataLength - PAYLOAD_START_INDEX - 1); i++) {
                  //          Serial.print(interface.getResponse().getFrameData()[5 + i] , HEX);
                  //          Serial.print(" : ");
                  //        }
                  //        Serial.print(interface.getResponse().getFrameData()[5 + (frameDataLength - PAYLOAD_START_INDEX - 1)], HEX);
                  //        Serial.println(" ]");
                  //        
                  //        Serial.println("Parsed Payload Data:");
  
                  // Get the method the sending unit used to construct the packet
                  int sendMethod = -1;
                  if (frameDataLength == 6) {
                      sendMethod = 0; // The sender used sendData(). More data is expected to come along...
                  } else if (frameDataLength > 6) {
                      sendMethod = 1; // The sender used setupMessage(), addData(), and sendMessage()
                  }
  
                  // Parse payload data based on whether sendData() or the three functions setupMessage(), 
                  // addData(), and sendMessage() were used.
  
                  if (sendMethod != -1) { // Check if sendMethod is valid... if < 6, no data was attached...
  
                      // "sendData()" was used, so only one byte of data was sent (since this function sends only one byte).
                      // Therefore, extract the one byte of data from the first byte of the "Payload". [Page 15]
                      if (sendMethod == 0) {
  
                        Serial.print("  Sent Data: ");
                        Serial.println(interface.getResponse().getFrameData()[5], HEX);
  
                      } 
                      else if (sendMethod == 1) {
  
                          // The "Payload" field is packed in pairs, each pair consisting of a 1 byte code followed by a 
                          // variable number of bytes of data, determinable by the 1 byte code.
  
                          unsigned int codeAndType = 0;
                          unsigned int payloadCode = 0;
                          unsigned int payloadDataType = 0;
  
                          int payloadOffset = 0;
  
                          int loopCount = 0;
                          int maxLoopCount = 20;
  
                          while(payloadOffset < (frameDataLength - PAYLOAD_START_INDEX)) {
  
                              // Protect against infinite loop with this conditional
//                              if (loopCount > maxLoopCount) {
//                                  Serial.println("WARNING: loopCount > maxLoopCount");
//                                  interface.getResponse().reset(); // TODO: Possibly remove this. This might be a bad idea.
//                                  clearCounter();
//                                  return false;
//                                  break;
//                              }
  
                              codeAndType = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset];
                              //              Serial.print(" Encoded send code and original data type: ");
                              //              Serial.println(codeAndType, HEX); // The actual data
  
                              payloadDataType = codeAndType & 0xF;
                              payloadCode = (codeAndType >> 4) & 0xF;
                              //              Serial.print("  The sent code was (in hex): ");
                              //              Serial.println(payloadCode, HEX);
                              //              Serial.print("  The original data type was: ");
                              //              Serial.println(payloadDataType);
  
                              //
                              // Extract and type cast the data based on data type
                              //
  
                              if (payloadDataType == TYPE_UINT8) {
  
                                  Serial.println("   Data type is TYPE_UINT8. High and low bytes:");
                                  Serial.print("    High part: ");
                                  Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]);
  
                                  // Append message to message queue
//                                  messageQueue = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1];
  
                                  payloadOffset = payloadOffset + sizeof(unsigned char) + 1;
  
                              } else if (payloadDataType == TYPE_INT8) {
  
                                  Serial.println("   Data type is TYPE_INT8. High and low bytes:");
                                  Serial.print("    High part: ");
                                  Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]);
  
                                  payloadOffset = payloadOffset + sizeof(char) + 1;
  
                              }
  
                              /* 
                               else if (payloadDataType == TYPE_UINT16) {
                               
                               Serial.println("   Data type is TYPE_UINT16. High and low bytes:");
                               Serial.print("    High part: ");
                               Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]); 
                               Serial.print("    Low part: ");
                               Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 2]);
                               
                               short unsigned int data = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1] << 8 | ((unsigned short int) interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 2]);
                               Serial.print("    Value: ");
                               Serial.println(data);
                               }
                               */
  
                              loopCount++;
  
                          } // while()
  
                          //
                          // Process received data, update state
                          //
  
                          // Turn on if received counter
                          //            setCounter();
  
                      }
                  } 
  
                  // Return true if a packet was read (i.e., received) successfully
                  // TODO: Add "return true" and "return false" cases to different conditions above, in this block.
                  // return true;
  
              } else if (commandId == APP_COMMAND_DATA_CONF) {
  
                  Serial.println("APP_COMMAND_DATA_CONF");
  
                  // return false; // Return true if a packet was read (i.e., received) successfully
  
              } else if (commandId == APP_COMMAND_GET_ADDR_RESP) { // (i.e., 0x25) [Page 15]
  
                  Serial.print("  Frame Command Id: ");
                  Serial.println(interface.getResponse().getFrameData()[0], HEX); // Frame options
  
//                  // Computer source address using bitwise operators (combine two bytes into a "short", a 16-bit integer data type)
//                  address = interface.getResponse().getFrameData()[0] | ((short) interface.getResponse().getFrameData()[1] << 8);
//                  Serial.print("  Device address: ");
//                  Serial.println(address, HEX); // Source address
//  
//                  hasValidAddress = true;
//  
//                  return false; // Return true if a packet was read (i.e., received) successfully
              }
  
          } else {
  
              Serial.println("Error: Failed to receive packet.");
              // TODO: Clear the buffer, check if this is causing messages to stop sending back and forth.
              // TODO: Reset!!
//              interface.getResponse().reset(); // TODO: Possibly remove this. This might be a bad idea.
//              clearCounter();
//              return false; // Return true if a packet was read (i.e., received) successfully
          }
  
          // return true; // Return true if a packet was read (i.e., received) successfully
  
      } else { // Timeout or error occurred
//          return false;
      }
  
  //Oops?
  if (interface.getResponse().isError()) {
    Serial.println("You had ONE job Arduino. ONE job.");
  }
  
  delay(1000);
  
  //We use the 'setupMessage()' call if we want to use a bunch of data,
  //otherwise can use sendData() calls to directly send a few bytes
  
  //This is the OTHER guys address
//  interface.setupMessage(THEIR_ADDRESS);
//  
//  // Package the data payload for transmission
//  interface.addData(1, (unsigned char) 0x13); // TYPE_UINT8
////  interface.addData(1, (char) 0x14); // TYPE_INT8
//  interface.addData(3, (unsigned short int) 0xFFFD); // TYPE_UINT16
//  interface.addData(1, (short) 0xABCD); // TYPE_INT16
//  interface.addData(14, (unsigned long) 0xDDDDCCAA); // TYPE_UINT32
//  interface.addData(9, (long) 0xFF03CCAA); // TYPE_INT32
//  
////  //Send state of pot (potentimeter, not drug manufacturing)
////  interface.addData(CODE_VALVE, analogRead(1));
////  
////  //Toggle other other guys LED on RadioBlock
////  interface.addData(CODE_LED, 1);
// 
//  //Send data OTA
//  interface.sendMessage();
  
  // sendCounter()
  if (OUR_ADDRESS == 0x0001) {
    unsigned long currentTime = millis();
    if (currentTime - lastCount > 1000) {
      
      Serial.println("Tick.");
      
      // Send to all linked devices
//      for (int i = 0; i < 1; i++) {
//          // Set the destination address
//          interface.setupMessage(next[i]);
//  
//          // Package the data payload for transmission
//          interface.addData(1, (byte) 0x1F); // TYPE_INT8
//          interface.sendMessage(); // Send data OTA
//  
//          // Wait for confirmation
//          // delayUntilConfirmation();
//      }

      interface.setupMessage(THEIR_ADDRESS);
        
      // Package the data payload for transmission
      interface.addData(1, (unsigned char) 0x13); // TYPE_UINT8
      //  interface.addData(1, (char) 0x14); // TYPE_INT8
      interface.addData(3, (unsigned short int) 0xFFFD); // TYPE_UINT16
      interface.addData(1, (short) 0xABCD); // TYPE_INT16
      interface.addData(14, (unsigned long) 0xDDDDCCAA); // TYPE_UINT32
      interface.addData(9, (long) 0xFF03CCAA); // TYPE_INT32
      
      //  //Send state of pot (potentimeter, not drug manufacturing)
      //  interface.addData(CODE_VALVE, analogRead(1));
      //  
      //  //Toggle other other guys LED on RadioBlock
      //  interface.addData(CODE_LED, 1);
       
      //Send data OTA
      interface.sendMessage();
      
      lastCount = millis();
    }
  }
}
