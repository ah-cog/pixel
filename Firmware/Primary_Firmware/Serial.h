#ifndef I2C_H
#define I2C_H

#include "Behavior.h"
#include "Platform.h"

// from Language.h
long Process_Message (Message* message); // start the "interpretive dance": transformation/message => interpret => behavior

#define DEVICE_SERIAL Serial3

char behaviorDescriptionBuffer[128];
int behaviorDescriptionBufferIndex = 0;

boolean Setup_Bridge () {
  DEVICE_SERIAL.begin (115200);
}

#define BUFFER_SIZE_I2C 32

char i2cMessageBuffer[128] = { 0 };
int i2cMessageBufferSize = 0;

//! Propagates channel values to machine's transclusions.
//!
//! TODO: Move this to "Ports.h"
void Propagate_Channel_Value (Channel* channel) {
//void syncPinValue(int pin) {

  // Get the most recent pin value
  int pinValue = Get_Channel_Value (channel);
  
  // TODO: Conditionally propagate only on channels on which machines/simulation transclusions exist.
  
  // TODO: Update state on other modules in mesh network.
  
  //channels[pin].isUpdated = false;
  (*channel).isUpdated = false;
}

/**
 * The "behavior" data structure and interpretter
 */
// TODO: Updates behavior (i.e., the state of the program being interpretted)
void Get_Behavior_Transformations () { // consider renaming this something like acceptBehaviorTransformation
  
//  Serial.print ("bytes = "); Serial.println(bytes);
  int transformationDataSize = 0; // The actual transformation data received (less than or equal to 32 bytes)
  
  i2cMessageBufferSize = 0;

  // Receive messages from slave (if any)
//  while (Wire.available () > 0) { // slave may send less than requested
  while (DEVICE_SERIAL.available () > 0) {
  
    //char c = Wire.read (); // receive a byte as character
//    char c = Wire.receive (); // receive a byte as character
    int incomingByte = DEVICE_SERIAL.read ();
    char c = (char) incomingByte;
//    Serial.print (c); // print the character
//    Serial.print(" ");
    
    // Count the bytes of actual data received (other than NULL bytes)
    if (c != '\0') {
      transformationDataSize++;
    }
    
    if (c == ')') {
//      Serial.println ("\t))))))))))");
    }
    
    // Copy byte into message buffer
    i2cMessageBuffer[i2cMessageBufferSize] = c;
    i2cMessageBufferSize++;
  }
  
  i2cMessageBuffer[transformationDataSize] = '\0'; // Terminate I2C message buffer
  
  i2cMessageBufferSize = 0; // Reset I2C message buffer size
  
  // Process received data (i.e., parse the received messages)
  // if (strlen(i2cMessageBuffer) > 1) { // if (i2cMessageBufferSize > 0) {
  if (transformationDataSize > 0) { // if (i2cMessageBufferSize > 0) {
//    Serial.println (i2cMessageBuffer);

    // Search for start of message
    char* firstCharacterIndex = NULL;
    firstCharacterIndex = strchr (i2cMessageBuffer, '(');
    
    // Buffer the received message
    if (firstCharacterIndex != NULL) {
//      Serial.println ("Found '('");
      firstCharacterIndex = firstCharacterIndex + 1;
      behaviorDescriptionBufferIndex = 0;
      
      // Find first of '\0', ')', or the (maximum) index of 32 (which means there should be more coming!)
      
//      strncpy (behaviorDescriptionBuffer + behaviorDescriptionBufferIndex, firstCharacterIndex + 1, );
//      Serial.print ("\tfirstCharacterIndex: "); Serial.print (firstCharacterIndex); Serial.print ("\n");
      
    } else {
      
      // The '(' character was not found, so set the beginning to the first character in the buffer.
      firstCharacterIndex = i2cMessageBuffer;
//      Serial.print ("\tfirst character not found! setting firstCharacterIndex to "); Serial.print (firstCharacterIndex); Serial.print ("\n");
      
    }
    
    boolean done = false;
    
    // Search for end of message
    char* lastCharacterIndex = NULL;
    lastCharacterIndex = strchr (i2cMessageBuffer, ')');
    
    // Update the last character index
    if (lastCharacterIndex != NULL) {
//      Serial.println ("Found(1) ')'");
      lastCharacterIndex = lastCharacterIndex - 1;
      done = true;
    }
    
    // Search for the '\0' character if ')' not found
    if (lastCharacterIndex == NULL) {
      //lastCharacterIndex = strchr (i2cMessageBuffer, '\0');
      lastCharacterIndex = strchr (i2cMessageBuffer, '\0');
      if (lastCharacterIndex != NULL) {
//        Serial.println ("Found(2) '\0'");
        lastCharacterIndex = lastCharacterIndex - 1;
      }
    }
    
    // Search for the last character in the message
    if (lastCharacterIndex == NULL) {
      lastCharacterIndex = i2cMessageBuffer + BUFFER_SIZE_I2C;
      if (lastCharacterIndex != NULL) {
//        Serial.print ("Found(3) end of string at position "); Serial.print (BUFFER_SIZE_I2C); Serial.print ("\n");
      }
    }
//    Serial.print ("\tlastCharacterIndex: "); Serial.print (lastCharacterIndex); Serial.print ("\n");
    
    // Copy the received data into the local buffer
    int behaviorDescriptionSize = (lastCharacterIndex + 1) - firstCharacterIndex;
//    int behaviorDescriptionSize = (lastCharacterIndex) - firstCharacterIndex;
//    if (i2cMessageBuffer[lastCharacterIndex] == ')') {
//      behaviorDescriptionSize = behaviorDescriptionSize + 1;
//    }
    strncpy (behaviorDescriptionBuffer + behaviorDescriptionBufferIndex, firstCharacterIndex, behaviorDescriptionSize);
    behaviorDescriptionBufferIndex = behaviorDescriptionBufferIndex + behaviorDescriptionSize;
    
    behaviorDescriptionBuffer[behaviorDescriptionBufferIndex] = '\0';
//    Serial.print ("\tbehaviorDescriptionBuffer: "); Serial.print (behaviorDescriptionBuffer); Serial.print ("\n");
    
    // TODO: Check for an infinite loop and prevent it (e.g., with a timer).
    
    if (done) {
      
      // Terminate the string stored in the buffer.
      behaviorDescriptionBuffer[behaviorDescriptionBufferIndex] = '\0';
      
      Serial.println (behaviorDescriptionBuffer);
      
      String split = String (behaviorDescriptionBuffer);
      int spaceCount = getValueCount (split, ' ');

      String first = getValue (split, ' ', 0);
      
      Serial.println (first);
      
      long responseUuid = 0L;
        
      String remoteMessage = String (behaviorDescriptionBuffer);
      Message* message = Create_Message (platformUuid, platformUuid, remoteMessage);
      responseUuid = Process_Message (message);
      
      // TODO: Get return value from Process_Message. This should be a UUID corresponding to the queued message item. The UUID generated is used for the created behavior if a behavior is being created. If no response will be given (over serial to the slave board), then return 0, which means "no UUID".
      
//      responseUuid = Generate_Platform_Uuid ();
      
      // DIRECTLY RESPOND (BLOCKING/SYNCRONOUS) relay response to the secondary board (to return to the HTTP client)
      String responseMessage = String ("(") + String (responseUuid) + String (")");
      const int serialResponseBufferSize = 64;
      char messageChar[serialResponseBufferSize];
      responseMessage.toCharArray (messageChar, serialResponseBufferSize);
      DEVICE_SERIAL.write (messageChar);
      
    }
  }
}

#endif
