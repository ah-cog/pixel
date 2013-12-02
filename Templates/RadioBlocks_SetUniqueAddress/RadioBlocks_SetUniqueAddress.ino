#include <SoftwareSerial.h>
#include <RadioBlock.h>

// These #define's are copied from the RadioBlock.cpp file.
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

#define RADIOBLOCKS_ADDRESS_MIN 0
#define RADIOBLOCKS_ADDRESS_MAX 65533 // 0xFFFD

//Pins connected to RadioBlock pins 1/2/3/4
RadioBlockSerialInterface interface = RadioBlockSerialInterface(-1,-1,8,4);

int neighbors[1] = { 2 };

void setup()  
{
  //
  // Set up RadioBlock module
  //
  
  interface.begin(); // Start the RadioBlocks serial interface
  delay(500); // Give RadioBlock time to initialize
  
  //
  // Perform "network commissioning". [Page 5, SimpleMesh User's Guide]
  // 
  // Network commissioning is a fancy way to refer to setting four basic parameters of the 
  // RadioBlock module: (1) setting the channel (11 to 25), 
  // (2) address (0x0000 to 0xFFFD, 0xFFFF is the broadcast address), (3) set the PAN ID 
  // (0x0000 to 0xFFFE), (4) set the network key.
  //
  
  // We need to set these values so other RadioBlocks can find us
  interface.setChannel(15); // (1) Set the channel
  interface.setPanID(0xBAAD); // (3) Set the PAN ID
  
  // Set up address of RadioBlocks interface
//  unsigned int address = random(RADIOBLOCKS_ADDRESS_MIN, RADIOBLOCKS_ADDRESS_MAX); // Initial, random address
//  for (int i = 0; i < RADIOBLOCKS_ADDRESS_MAX; i++) {
//    // Send message
//    // Wait for duration, and if no response is received, then assign the address
//  }
  // TODO: Iterate until an address is set. Iterate through addresses to check for availability.
  // Broadcast message to "Get Acknowledgment State Request" and wait for response until timeout...
  // TODO: Dynamically set address based on other address in the area (and extended address space from shared state, and add collision fixing.)
  interface.setAddress(0x0003); // (2) Set the module's address
  
  // Begin serial communication
  Serial.begin(115200);
  Serial.println("Done with setup.");
}

boolean sent = false;
void loop() // run over and over
{
  
  if (!sent) {
    delay(1000);
    interface.getAddress();
    sent = true;
  }
  
  //  interface.setLED(true);
  //  delay(500);
  //  interface.setLED(false);
  //  delay(1500);
  //  interface.toggleLED();

  if (interface.readPacket(1000)) { // Waits a maximum of <i>timeout</i> milliseconds for a response packet before timing out; returns true if packet is read. Returns false if timeout or error occurs.
  //interface.readPacket(); // Read the packet (NOTE: Seemingly must do this for isAvailable() to work properly.)
  //if (interface.getResponse().isAvailable()) {
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
    // NOTE: I believe this constitutes a "frame".
    
    if (interface.getResponse().getErrorCode() == APP_STATUS_SUCESS) {
      Serial.println("\n\n\n\nReceived packet.");
    } else {
      Serial.println("Error: Failed to receive packet.");
    }
    
    Serial.print("Packet length: ");
    Serial.print(interface.getResponse().getPacketLength(), DEC);
    Serial.print(", Command: ");
    Serial.print(interface.getResponse().getCommandId(), HEX);
    Serial.print(", Payload: <below>");
    Serial.print(", CRC: ");
    Serial.print(interface.getResponse().getCrc(), HEX);
    Serial.println("");

    int commandId = interface.getResponse().getCommandId();
    
    if (commandId == APP_COMMAND_ACK) {
      // "Every command is confirmed with an acknowledgment command even if it is impossible
     //   to immediately execute the command. There is no particular order in which responses 
     //   are sent, so for example Data Indication Command might be sent before 
     //   Acknowledgment Command."
     
     // Acknowledgment command format: Page 5 of SimpleMesh Serial Protocol document.
      
      Serial.println("Received ack");
      
      
      Serial.print("  Source address: ");
      Serial.println(interface.getResponse().getFrameData()[1], HEX); // Source address
      
    } else if (commandId == APP_COMMAND_DATA_IND) { // (i.e., 0x22) [Page 15]
    
//        Serial.print(interface.getResponse().getFrameData()[0], HEX); // Frame options
//        Serial.print(" | ");
//        Serial.print(interface.getResponse().getFrameData()[1], HEX); // Frame options
//        Serial.print(" | ");
//        Serial.print(interface.getResponse().getFrameData()[2], HEX); // Frame options
//        Serial.print(" | ");
//        Serial.print(interface.getResponse().getFrameData()[3], HEX); // Frame options
//        Serial.print(" | ");
//        Serial.print(interface.getResponse().getFrameData()[4], HEX); // Frame options
    

      int frameDataLength = interface.getResponse().getFrameDataLength();
      Serial.print("  Payload length: ");
      Serial.println(frameDataLength, DEC); // "Payload" length (minus "Command Id" (1 byte))
      
//        Serial.print("  Frame Command Id: ");
//        Serial.println(interface.getResponse().getFrameData()[0], HEX); // Frame options
      
      // Computer source address using bitwise operators (combine two bytes into a "short", a 16-bit integer data type)
      short sourceAddress = interface.getResponse().getFrameData()[0] | ((short) interface.getResponse().getFrameData()[1] << 8);
      Serial.print("  Source address: ");
      Serial.println(sourceAddress, HEX); // Source address

      // Frame Options [Page 15]
      // i.e.,
      // 0x00 None
      // 0x01 Acknowledgment was requested
      // 0x02 Security was used
      Serial.print("  Frame options: ");
      Serial.println(interface.getResponse().getFrameData()[2], HEX); // Frame options

      Serial.print("  Link Quality Indicator: ");
      Serial.println(interface.getResponse().getFrameData()[3], HEX); // Link quality indicator

      Serial.print("  Received Signal Strength Indicator: ");
      Serial.println(interface.getResponse().getFrameData()[4], HEX); // Received Signal Strength Indicator
      
      // NOTE: Payload starts at index position 5
      
      //
      // Parse payload data
      //
      
      int payloadStartIndex = 5; // Index of first byte of "Payload"
      
      Serial.print("Raw Payload Data: [ ");
      for (int i = 0; i < (frameDataLength - payloadStartIndex - 1); i++) {
        Serial.print(interface.getResponse().getFrameData()[5 + i] , HEX);
        Serial.print(" : ");
      }
      Serial.print(interface.getResponse().getFrameData()[5 + (frameDataLength - payloadStartIndex - 1)], HEX);
      Serial.println(" ]");
      
      Serial.println("Parsed Payload Data:");
      
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
          
        } else if (sendMethod == 1) {
          
          // The "Payload" field is packed in pairs, each pair consisting of a 1 byte code followed by a 
          // variable number of bytes of data, determinable by the 1 byte code.
          
          unsigned int codeAndType = 0;
          unsigned int payloadCode = 0;
          unsigned int payloadDataType = 0;
          
          int payloadOffset = 0;
          
          int loopCount = 0;
          int maxLoopCount = 20;
          
          while(payloadOffset < (frameDataLength - payloadStartIndex)) {
            
            // Protect against infinite loop with this conditional
            if (loopCount > maxLoopCount) {
              break;
            }
            
            codeAndType = interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset];
            Serial.print(" Encoded send code and original data type: ");
            Serial.println(codeAndType, HEX); // The actual data
            
            payloadDataType = codeAndType & 0xF;
            payloadCode = (codeAndType >> 4) & 0xF;
            Serial.print("  The sent code was (in hex): ");
            Serial.println(payloadCode, HEX);
            Serial.print("  The original data type was: ");
            Serial.println(payloadDataType);
          
            //
            // Extract and type cast the data based on data type
            //
            
            if (payloadDataType == TYPE_UINT8) {
              
              Serial.println("   Data type is TYPE_UINT8. High and low bytes:");
              Serial.print("    High part: ");
              Serial.println(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1]);
              
              payloadOffset = payloadOffset + sizeof(unsigned char) + 1;
              
            } else if (payloadDataType == TYPE_INT8) {
              
              Serial.println("   Data type is TYPE_INT8. High and low bytes:");
              Serial.print("    High part: ");
              Serial.println(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1]);
              
              payloadOffset = payloadOffset + sizeof(char) + 1;
              
            } else if (payloadDataType == TYPE_UINT16) {
              
              Serial.println("   Data type is TYPE_UINT16. High and low bytes:");
              Serial.print("    High part: ");
              Serial.println(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1]); 
              Serial.print("    Low part: ");
              Serial.println(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 2]);
              
              short unsigned int data = interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1] << 8 | ((unsigned short int) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 2]);
              Serial.print("    Value: ");
              Serial.println(data);
              
              payloadOffset = payloadOffset + sizeof(unsigned short int) + 1;
              
            } else if (payloadDataType == TYPE_INT16) {
              
              Serial.println("   Data type is TYPE_INT16. High and low bytes:");
              Serial.print("    High part: ");
              Serial.println(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1]); 
              Serial.print("    Low part: ");
              Serial.println(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 2]);
              
              short data = interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1] << 8 | ((short) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 2]);
              Serial.print("    Value: ");
              Serial.println(data);
              
              payloadOffset = payloadOffset + sizeof(short) + 1;
              
            } else if (payloadDataType == TYPE_UINT32) {
              
              Serial.println("   Data type is TYPE_UINT32. Four bytes:");
              Serial.print("    Raw data: (MSB) [ ");
              Serial.print(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1], HEX); 
              Serial.print(" : ");
              Serial.print(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 2], HEX);
              Serial.print(" : ");
              Serial.print(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 3], HEX);
              Serial.print(" : ");
              Serial.print(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 4], HEX);
              Serial.println(" ] (LSB)");
              
              unsigned long data = ((unsigned long) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1] << 24) | ((unsigned long) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 2] << 16) | ((unsigned long) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 3] << 8) | ((unsigned long) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 4]);
              Serial.print("    Value: ");
              Serial.print(data, DEC);
              Serial.print(" (HEX: ");
              Serial.print(data, DEC);
              Serial.println(" )");
              
              payloadOffset = payloadOffset + sizeof(unsigned long) + 1;
              
            } else if (payloadDataType == TYPE_INT32) {
              
              Serial.println("   Data type is TYPE_INT32. Four bytes:");
              Serial.print("    Raw data: (MSB) [ ");
              Serial.print(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1], HEX); 
              Serial.print(" : ");
              Serial.print(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 2], HEX);
              Serial.print(" : ");
              Serial.print(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 3], HEX);
              Serial.print(" : ");
              Serial.print(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 4], HEX);
              Serial.println(" ] (LSB)");
              
              //long data = ((long) interface.getResponse().getFrameData()[6] << 24) | ((long) interface.getResponse().getFrameData()[7] << 16) | ((long) interface.getResponse().getFrameData()[8] << 8) | ((long) interface.getResponse().getFrameData()[9]);
              long data = ((long) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1] << 24) | ((long) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 2] << 16) | ((long) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 3] << 8) | ((long) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 4]);
              Serial.print("    Value: ");
              Serial.print(data, DEC);
              Serial.print(" (HEX: ");
              Serial.print(data, DEC);
              Serial.println(" )");
              
              payloadOffset = payloadOffset + sizeof(long) + 1;
              
            } else if (payloadDataType == TYPE_UINT64) {
              // TODO:
            } else if (payloadDataType == TYPE_INT64) {
              // TODO:
            } else if (payloadDataType == TYPE_FLOAT) {
              // TODO:
            } else if (payloadDataType == TYPE_FIXED8_8) {
              // TODO:
            } else if (payloadDataType == TYPE_FIXED16_8) {
              // TODO:
            } else if (payloadDataType == TYPE_8BYTES) {
              // TODO:
            } else if (payloadDataType == TYPE_16BYTES) {
              // TODO:
            } else if (payloadDataType == TYPE_ASCII) {
              // TODO:
            }
            
            loopCount++;
            
          } // while()
        }
      }

      /// TODO...
    } else if (commandId == APP_STATUS_TIMEOUT) {
      
      Serial.println("Error: Timeout");
      // TODO: (?) Remove timed out node from neighbors list? Update "last status" to timeout?
      
    } else if (commandId == APP_COMMAND_GET_ADDR_RESP) { // (i.e., 0x25) [Page 15]
      
//        Serial.print("  Frame Command Id: ");
//        Serial.println(interface.getResponse().getFrameData()[0], HEX); // Frame options
      
      // Computer source address using bitwise operators (combine two bytes into a "short", a 16-bit integer data type)
      short address = interface.getResponse().getFrameData()[0] | ((short) interface.getResponse().getFrameData()[1] << 8);
      Serial.print("  Device address: ");
      Serial.println(address, HEX); // Source address
    
    }
  }



//  for(int i = 0; i < 4; i++) {

//    interface.setupMessage(neighbors[0]);
//
//    //Send temperature reading
//    //  interface.addData(CODE_TEMP, analogRead(A0));
//
//    //Send state of pot (potentimeter, not drug manufacturing)
//    //  interface.addData(CODE_VALVE, analogRead(1));
//
//    //Toggle other other guys LED on RadioBlock
////    interface.addData(1, );
//
//    //Send data OTA
//    interface.sendMessage(); 
//
//    delay(2000);
//  }


}

