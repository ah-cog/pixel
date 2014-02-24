#include <SoftwareSerial.h>
#include <RadioBlock.h>

// RadioBlock pins Tx, Rx, Gnd, Vcc are connected to Arduino pins 2, 3, 4, 5 (respectively).
RadioBlockSerialInterface interface = RadioBlockSerialInterface(5, 4, 3, 2);

//uint8_t payload[] = { 2 };

// Set our known network addresses. How do we deal with unexpected nodes...?
#define OUR_ADDRESS   0x1001
#define THEIR_ADDRESS 0x1000

void setup() {
  interface.begin();  
  
  // Give the RadioBlock some time to init
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
  // while (!Serial); // HACK: Wait untilSerial is ready - Leonardo
  Serial.println("Starting...");
}

void loop() { // run over and over

  // NOTE: (Bug?) Uncommenting the following lines of code causes an incorrect Command ID portion of the Data Request in the Frame.
  // interface.setLED(true);
  // delay(500);
  // interface.setLED(false);
  // delay(1500);
  // interface.toggleLED();
  
  // Read an incoming packet if available within the specified number of milliseconds (the timeout value).
  if (interface.readPacket(10)) { // NOTE: Every time this is called, the response returned by getResponse() is overwritten.
    Serial.println("Received a packet:");
    
    // Get error code for response
    if (interface.getResponse().getErrorCode() == APP_STATUS_SUCESS) {
      Serial.println("Success: Good packet.");
    } else {
      Serial.println("Failure: Bad packet.");
    }
     Serial.print("Len: ");
     Serial.print(interface.getResponse().getPacketLength(), DEC);
     Serial.print(", Command: ");
     Serial.print(interface.getResponse().getCommandId(), HEX);
     Serial.print(", CRC: ");
     Serial.print(interface.getResponse().getCrc(), HEX); // Cyclic redundancy check (CRC) [Source: http://en.wikipedia.org/wiki/Cyclic_redundancy_check]
     Serial.println("");

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
     Serial.print("Length of Frame Data: ");
     Serial.println(frameDataLength);
     
     // Get the method the sending unit used to construct the packet
     if (frameDataLength == 6) {
       sendMethod = 0; // The sender used sendData()
     } else if (frameDataLength > 6) {
       sendMethod = 1; // The sender used setupMessage(), addData(), and sendMessage()
     }
     
     // The following "meanings" for these bytes are from page 15 of the SimpleMesh_Serial_Protocol.pdf from Colorado Micro Devices.
     Serial.println("Frame Data: ");
     
     //Serial.println(interface.getResponse().getFrameData()[0], HEX);
     //Serial.println(interface.getResponse().getFrameData()[1]);
     //Serial.println(interface.getResponse().getFrameData()[2]);
     
     // Process message based on the its Command ID
     
     commandId = interface.getResponse().getCommandId();
     if (commandId == 0x22) { //APP_COMMAND_DATA_IND) { // 0x22
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
       
       // Parse Data or Payload:
       
       if (sendMethod == 0) {
         Serial.print("  Sent Data: ");
         Serial.println(interface.getResponse().getFrameData()[5], HEX);
       } else if (sendMethod == 1) {
         codeAndType = interface.getResponse().getFrameData()[5]; 
         
         
         Serial.print(" Encoded send code and original data type: ");
         Serial.println(codeAndType, HEX); // The actual data
         
         payloadDataType = codeAndType & 0xf;
         payloadCode = (codeAndType >> 4) & 0xf;
         
         Serial.print("  The sent code was (in hex): ");
         Serial.println(payloadCode, HEX);
         Serial.print("  The original data type was: ");
         Serial.println(payloadDataType);
         
         if (payloadDataType == 1) {
           Serial.println("   Data type is TYPE_UINT8. Data:");
           Serial.print("    The data: ");
           Serial.println(interface.getResponse().getFrameData()[6]);
         } else if (payloadDataType == 2) {
           Serial.println("   Data type is TYPE_INT8. High and low bytes:");
           Serial.print("    High part: ");
           Serial.println(interface.getResponse().getFrameData()[6]); 
           Serial.print("    Low part: ");
           Serial.println(interface.getResponse().getFrameData()[7]);
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
