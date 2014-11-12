#ifndef I2C_H
#define I2C_H

#include "Behavior.h"
#include "Platform.h"

#define I2C_MESSAGE_BYTE_SIZE 30
#define BUFFER_SIZE_I2C 32

char i2cMessageBuffer[128] = { 0 };
int i2cMessageBufferSize = 0;

// Message format:
// 00000000 00000000 00000000 00000000 00000000 00000000
// 
// Messages:
// - Pop next message
// - Clear all messages
// - Request top message (but don't remove it)
// - Remove top message (but don't send it, send removal status)

#define SLAVE_DEVICE_ADDRESS 2 // Slave device address

//String getValue (String data, char separator, int index);
//int getValueCount (String data, char separator);

// TODO: Move this to "Ports.h"
//void sendToSlave(char* text) {
//  // Send character string to slave device
//  Wire.beginTransmission(SLAVE_DEVICE_ADDRESS); // transmit to device #4
//  Wire.write(text);
//  Wire.endTransmission();    // stop transmitting
//}

// TODO: Move this to "Ports.h"
void Propagate_Channel_Value (Channel* channel) {
//void syncPinValue(int pin) {

  // Get the most recent pin value
  int pinValue = Get_Current_Channel_Value (channel);
  
  // Update the state (on slave board for Looper)
  char buf[8];
  Wire.beginTransmission(SLAVE_DEVICE_ADDRESS); // transmit to device #4
  Wire.write("pin ");
  Wire.write(itoa((*channel).address, buf, 10)); // The pin number
  Wire.write(" ");
  Wire.write(pinValue == HIGH ? "1" : "0"); // The pin's value
  //  Wire.write(";");        // sends five bytes
  Wire.endTransmission();    // stop transmitting
  
  // TODO: Update state on other pixels in mesh network
  
  //channels[pin].isUpdated = false;
  (*channel).isUpdated = false;
}





char behaviorDescriptionBuffer[128];
int behaviorDescriptionBufferIndex = 0;

#define I2C_CONFIRM_UPDATE_STATE 1

/**
 * The "behavior" data structure and interpretter
 */
// TODO: Updates behavior (i.e., the state of the program being interpretted)
void Get_Behavior_Transformations () { // consider renaming this something like acceptBehaviorTransformation

//  Serial.println ("acceptBehaviorTransformations");
  
  // Request messages from slave
  // NOTE: This causes the function "requestEvent" specified in "Wire.onRequest(requestEvent);" 
  //       to be called on the slave device.
  //int bytes = Wire.requestFrom (SLAVE_DEVICE_ADDRESS, I2C_MESSAGE_BYTE_SIZE); // Request 6 bytes from slave device #2
  int bytes = Wire.requestFrom (SLAVE_DEVICE_ADDRESS, BUFFER_SIZE_I2C); // Request 6 bytes from slave device #2
  
//  Serial.print ("bytes = "); Serial.println(bytes);
  int transformationDataSize = 0; // The actual transformation data received (less than or equal to 32 bytes)
  
  i2cMessageBufferSize = 0;

  // Receive messages from slave (if any)
  while (Wire.available () > 0) { // slave may send less than requested
  
    //char c = Wire.read (); // receive a byte as character
    char c = Wire.receive (); // receive a byte as character
//    Serial.print (c); // print the character
//    Serial.print(" ");
    
    // Count the bytes of actual data received (other than NULL bytes)
    if (c != '\0') {
      transformationDataSize++;
    }
    
    if (c == ')') {
      Serial.println ("\t))))))))))");
    }
    
    // Copy byte into message buffer
    i2cMessageBuffer[i2cMessageBufferSize] = c;
    i2cMessageBufferSize++;
  }
  //i2cMessageBuffer[i2cMessageBufferSize] = '\0'; // Terminate I2C message buffer
  i2cMessageBuffer[transformationDataSize] = '\0'; // Terminate I2C message buffer
//  Serial.print ("\ti2cMessageBufferSize: "); Serial.print (i2cMessageBufferSize); Serial.print ("\n");
//  Serial.print ("\ttransformationDataSize: "); Serial.print (transformationDataSize); Serial.print ("\n");

//  int transformationDataSize = i2cMessageBufferSize; // TODO: Combine i2cMessageBufferSize and transformationDataSize to make more space/time efficient.
  
  i2cMessageBufferSize = 0; // Reset I2C message buffer size
  
  // Process received data (i.e., parse the received messages)
  //if (strlen(i2cMessageBuffer) > 1) { // if (i2cMessageBufferSize > 0) {
  if (transformationDataSize > 0) { // if (i2cMessageBufferSize > 0) {
    Serial.println (i2cMessageBuffer);
//    Serial.println("Buffer > 1");

    // Search for start of message
    char* firstCharacterIndex = NULL;
    firstCharacterIndex = strchr (i2cMessageBuffer, '(');
    
    // Buffer the received message
    if (firstCharacterIndex != NULL) {
      Serial.println ("Found '('");
      firstCharacterIndex = firstCharacterIndex + 1;
      behaviorDescriptionBufferIndex = 0;
      
      // Find first of '\0', ')', or the (maximum) index of 32 (which means there should be more coming!)
      
//      strncpy (behaviorDescriptionBuffer + behaviorDescriptionBufferIndex, firstCharacterIndex + 1, );
      Serial.print ("\tfirstCharacterIndex: "); Serial.print (firstCharacterIndex); Serial.print ("\n");
      
    } else {
      
      // The '(' character was not found, so set the beginning to the first character in the buffer.
      firstCharacterIndex = i2cMessageBuffer;
      Serial.print ("\tfirst character not found! setting firstCharacterIndex to "); Serial.print (firstCharacterIndex); Serial.print ("\n");
      
    }
    
    boolean done = false;
    
    // Search for end of message
    char* lastCharacterIndex = NULL;
    lastCharacterIndex = strchr (i2cMessageBuffer, ')');
    
    // Update the last character index
    if (lastCharacterIndex != NULL) {
      Serial.println ("Found(1) ')'");
      lastCharacterIndex = lastCharacterIndex - 1;
      done = true;
    } 
    
    // Search for the '\0' character if ')' not found
    if (lastCharacterIndex == NULL) {
      //lastCharacterIndex = strchr (i2cMessageBuffer, '\0');
      lastCharacterIndex = strchr (i2cMessageBuffer, '\0');
      if (lastCharacterIndex != NULL) {
        Serial.println ("Found(2) '\0'");
        lastCharacterIndex = lastCharacterIndex - 1;
      }
    }
    
    // Search for the last character in the message
    if (lastCharacterIndex == NULL) {
      lastCharacterIndex = i2cMessageBuffer + BUFFER_SIZE_I2C;
      if (lastCharacterIndex != NULL) {
        Serial.print ("Found(3) end of string at position "); Serial.print (BUFFER_SIZE_I2C); Serial.print ("\n");
      }
    }
    Serial.print ("\tlastCharacterIndex: "); Serial.print (lastCharacterIndex); Serial.print ("\n");
    
    // Copy the received data into the local buffer
    int behaviorDescriptionSize = (lastCharacterIndex + 1) - firstCharacterIndex;
//    int behaviorDescriptionSize = (lastCharacterIndex) - firstCharacterIndex;
//    if (i2cMessageBuffer[lastCharacterIndex] == ')') {
//      behaviorDescriptionSize = behaviorDescriptionSize + 1;
//    }
    strncpy (behaviorDescriptionBuffer + behaviorDescriptionBufferIndex, firstCharacterIndex, behaviorDescriptionSize);
    behaviorDescriptionBufferIndex = behaviorDescriptionBufferIndex + behaviorDescriptionSize;
    
    behaviorDescriptionBuffer[behaviorDescriptionBufferIndex] = '\0';
    Serial.print ("\tbehaviorDescriptionBuffer: "); Serial.print (behaviorDescriptionBuffer); Serial.print ("\n");
    
    // TODO: Check for an infinite loop and prevent it (e.g., with a timer).
    
    if (done) {
      
      // Terminate the string stored in the buffer.
      behaviorDescriptionBuffer[behaviorDescriptionBufferIndex] = '\0';
      
      Serial.println (behaviorDescriptionBuffer);
      
      String split = String (behaviorDescriptionBuffer);
      int spaceCount = getValueCount (split, ' ');
      
//      Serial.println (spaceCount);
//      
//      for (int i = 0; i < spaceCount; i++) {
//        
//        String value = getValue (split, ' ', i);
//        Serial.print (i);
//        Serial.print (" ");
//        Serial.print (value);
//        Serial.print (" ");
//        
//      }
//      Serial.print ("\n");

      String first = getValue (split, ' ', 0);
      
      Serial.println (first);
      
      if (first.compareTo ("create") == 0) {
        
        String second = getValue (split, ' ', 1);
        Serial.println (second);
        
        if (second.compareTo ("substrate") == 0) {
          // TODO: create substrate
          
          String uid = getValue (split, ' ', 2);
          
          // TODO: Create the substrate with the specified UID.
          
        } else if (second.compareTo ("behavior") == 0) {
          
          String behaviorType = getValue (split, ' ', 2);
          
          // Get the propagated UUID
          int behaviorUuid = getValue (split, ' ', 3).toInt (); // TODO: Move/Remove this! Come up with a better way to do this!
          
          Serial.println (behaviorType);
          
          if (behaviorType.compareTo ("input") == 0) {
        
            // Parse parameters
            int pin = getValue (split, ' ', 4).toInt();
            String signal = getValue (split, ' ', 5);
            
            // Create behavior and add it to the behavior substrate
            Behavior* behavior = Create_Input_Behavior (substrate, pin, signal);
            (*behavior).uid = behaviorUuid; // TODO: Move/Remove this! Come up with a better way to do this!
            Sequence* sequence = (*substrate).sequences;
            Update_Behavior_Sequence (behavior, sequence);
            
            // TODO: Propagate to any subscribers to this device! (stored "beneath" the interpreter, for the device).
            
          } else if (behaviorType.compareTo ("output") == 0) {
        
            // Parse parameters
            int pin = getValue (split, ' ', 4).toInt();
            String signal = getValue (split, ' ', 5);
            String data = getValue (split, ' ', 6);
            
            // Create behavior and add it to the behavior substrate
            Behavior* behavior = Create_Output_Behavior (substrate, pin, signal, data);
            (*behavior).uid = behaviorUuid; // TODO: Move/Remove this! Come up with a better way to do this!
            Sequence* sequence = (*substrate).sequences;
            Update_Behavior_Sequence (behavior, sequence);
            
            // TODO: Propagate to any subscribers to this device! (stored "beneath" the interpreter, for the device).
            
          } else if (behaviorType.compareTo ("delay") == 0) {
        
            // Parse parameters
            int milliseconds = getValue (split, ' ', 4).toInt();
            
            // Create behavior and add it to the behavior substrate
            Behavior* behavior = Create_Delay_Behavior (substrate, milliseconds);
            (*behavior).uid = behaviorUuid; // TODO: Move/Remove this! Come up with a better way to do this!
            Sequence* sequence = (*substrate).sequences;
            Update_Behavior_Sequence (behavior, sequence);
            
            // TODO: Propagate to any subscribers to this device! (stored "beneath" the interpreter, for the device).
            
          } else if (behaviorType.compareTo ("sound") == 0) {
        
            // Parse parameters
            int note = getValue (split, ' ', 4).toInt();
            int duration = getValue (split, ' ', 5).toInt();
//            String signal = getValue (split, ' ', 4);
//            String data = getValue (split, ' ', 5);
            
            // Create behavior and add it to the behavior substrate
            Behavior* behavior = Create_Sound_Behavior (substrate, note, duration);
            (*behavior).uid = behaviorUuid; // TODO: Move/Remove this! Come up with a better way to do this!
            Sequence* sequence = (*substrate).sequences;
            Update_Behavior_Sequence (behavior, sequence);
            
            // TODO: Propagate to any subscribers to this device! (stored "beneath" the interpreter, for the device).
            
          } else {
            
            // TODO: Implement "custom" cloud "RPC" behavior.
            
          }
          
        } else if (second.compareTo ("loop") == 0) {
          
          // TODO: create loop
          
        } 
//        else if (first.compareTo ("line")) {
//          // TODO: Substrate
//        } else if (first.compareTo ("dot")) {
//          // TODO: Substrate
//        }
        
      } else if (first.compareTo ("update") == 0) {
        
        String second = getValue (split, ' ', 1);
        Serial.println (second);
        
        if (second.compareTo ("substrate") == 0) {
          // TODO: create substrate
          
          String uid = getValue (split, ' ', 2);
          
          // TODO: Create the substrate with the specified UID.
          
        } else if (second.compareTo ("behavior") == 0) {
          
          // TODO: create behavior
          int uuid = getValue (split, ' ', 2).toInt ();
          
          Serial.println (uuid);
          
          Behavior* behavior = Get_Behavior (uuid);
          Serial.print ("found behavior: "); Serial.print ((int) behavior); Serial.print ("\n");
          Sound* sound = Get_Sound_Behavior (behavior);
              
          if ((*behavior).type == BEHAVIOR_TYPE_SOUND) {
            
            int note = getValue (split, ' ', 3).toInt();
            int duration = getValue (split, ' ', 4).toInt();
            
            // TODO: Check for validity
            
            (*sound).note = note;
            (*sound).duration = duration;
          }
          
//          if (behaviorType.compareTo ("input") == 0) {
//        
//            // Parse parameters
//            int pin = getValue (split, ' ', 3).toInt();
//            String signal = getValue (split, ' ', 4);
//            
//            // Create behavior and add it to the behavior substrate
//            Behavior* behavior = Create_Input_Behavior (substrate, pin, signal);
//            Sequence* sequence = (*substrate).sequences;
//            Update_Behavior_Sequence (behavior, sequence);
//            
//            // TODO: Propagate to any subscribers to this device! (stored "beneath" the interpreter, for the device).
//            
//          }
        }
      }
      
    }
  }
}


#define DEVICE_SERIAL Serial3

boolean setupBridge () {
  DEVICE_SERIAL.begin (115200);
}

/**
 * The "behavior" data structure and interpretter
 */
// TODO: Updates behavior (i.e., the state of the program being interpretted)
void Get_Behavior_Transformations_Serial () { // consider renaming this something like acceptBehaviorTransformation

//  Serial.println ("acceptBehaviorTransformations");
  
  // Request messages from slave
  // NOTE: This causes the function "requestEvent" specified in "Wire.onRequest(requestEvent);" 
  //       to be called on the slave device.
  //int bytes = Wire.requestFrom (SLAVE_DEVICE_ADDRESS, I2C_MESSAGE_BYTE_SIZE); // Request 6 bytes from slave device #2
//  int bytes = Wire.requestFrom (SLAVE_DEVICE_ADDRESS, BUFFER_SIZE_I2C); // Request 6 bytes from slave device #2
  
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
      Serial.println ("\t))))))))))");
    }
    
    // Copy byte into message buffer
    i2cMessageBuffer[i2cMessageBufferSize] = c;
    i2cMessageBufferSize++;
  }
  //i2cMessageBuffer[i2cMessageBufferSize] = '\0'; // Terminate I2C message buffer
  i2cMessageBuffer[transformationDataSize] = '\0'; // Terminate I2C message buffer
//  Serial.print ("\ti2cMessageBufferSize: "); Serial.print (i2cMessageBufferSize); Serial.print ("\n");
//  Serial.print ("\ttransformationDataSize: "); Serial.print (transformationDataSize); Serial.print ("\n");

//  int transformationDataSize = i2cMessageBufferSize; // TODO: Combine i2cMessageBufferSize and transformationDataSize to make more space/time efficient.
  
  i2cMessageBufferSize = 0; // Reset I2C message buffer size
  
  // Process received data (i.e., parse the received messages)
  //if (strlen(i2cMessageBuffer) > 1) { // if (i2cMessageBufferSize > 0) {
  if (transformationDataSize > 0) { // if (i2cMessageBufferSize > 0) {
    Serial.println (i2cMessageBuffer);
//    Serial.println("Buffer > 1");

    // Search for start of message
    char* firstCharacterIndex = NULL;
    firstCharacterIndex = strchr (i2cMessageBuffer, '(');
    
    // Buffer the received message
    if (firstCharacterIndex != NULL) {
      Serial.println ("Found '('");
      firstCharacterIndex = firstCharacterIndex + 1;
      behaviorDescriptionBufferIndex = 0;
      
      // Find first of '\0', ')', or the (maximum) index of 32 (which means there should be more coming!)
      
//      strncpy (behaviorDescriptionBuffer + behaviorDescriptionBufferIndex, firstCharacterIndex + 1, );
      Serial.print ("\tfirstCharacterIndex: "); Serial.print (firstCharacterIndex); Serial.print ("\n");
      
    } else {
      
      // The '(' character was not found, so set the beginning to the first character in the buffer.
      firstCharacterIndex = i2cMessageBuffer;
      Serial.print ("\tfirst character not found! setting firstCharacterIndex to "); Serial.print (firstCharacterIndex); Serial.print ("\n");
      
    }
    
    boolean done = false;
    
    // Search for end of message
    char* lastCharacterIndex = NULL;
    lastCharacterIndex = strchr (i2cMessageBuffer, ')');
    
    // Update the last character index
    if (lastCharacterIndex != NULL) {
      Serial.println ("Found(1) ')'");
      lastCharacterIndex = lastCharacterIndex - 1;
      done = true;
    } 
    
    // Search for the '\0' character if ')' not found
    if (lastCharacterIndex == NULL) {
      //lastCharacterIndex = strchr (i2cMessageBuffer, '\0');
      lastCharacterIndex = strchr (i2cMessageBuffer, '\0');
      if (lastCharacterIndex != NULL) {
        Serial.println ("Found(2) '\0'");
        lastCharacterIndex = lastCharacterIndex - 1;
      }
    }
    
    // Search for the last character in the message
    if (lastCharacterIndex == NULL) {
      lastCharacterIndex = i2cMessageBuffer + BUFFER_SIZE_I2C;
      if (lastCharacterIndex != NULL) {
        Serial.print ("Found(3) end of string at position "); Serial.print (BUFFER_SIZE_I2C); Serial.print ("\n");
      }
    }
    Serial.print ("\tlastCharacterIndex: "); Serial.print (lastCharacterIndex); Serial.print ("\n");
    
    // Copy the received data into the local buffer
    int behaviorDescriptionSize = (lastCharacterIndex + 1) - firstCharacterIndex;
//    int behaviorDescriptionSize = (lastCharacterIndex) - firstCharacterIndex;
//    if (i2cMessageBuffer[lastCharacterIndex] == ')') {
//      behaviorDescriptionSize = behaviorDescriptionSize + 1;
//    }
    strncpy (behaviorDescriptionBuffer + behaviorDescriptionBufferIndex, firstCharacterIndex, behaviorDescriptionSize);
    behaviorDescriptionBufferIndex = behaviorDescriptionBufferIndex + behaviorDescriptionSize;
    
    behaviorDescriptionBuffer[behaviorDescriptionBufferIndex] = '\0';
    Serial.print ("\tbehaviorDescriptionBuffer: "); Serial.print (behaviorDescriptionBuffer); Serial.print ("\n");
    
    // TODO: Check for an infinite loop and prevent it (e.g., with a timer).
    
    if (done) {
      
      // Terminate the string stored in the buffer.
      behaviorDescriptionBuffer[behaviorDescriptionBufferIndex] = '\0';
      
      Serial.println (behaviorDescriptionBuffer);
      
      String split = String (behaviorDescriptionBuffer);
      int spaceCount = getValueCount (split, ' ');
      
//      Serial.println (spaceCount);
//      
//      for (int i = 0; i < spaceCount; i++) {
//        
//        String value = getValue (split, ' ', i);
//        Serial.print (i);
//        Serial.print (" ");
//        Serial.print (value);
//        Serial.print (" ");
//        
//      }
//      Serial.print ("\n");

      String first = getValue (split, ' ', 0);
      
      Serial.println (first);
      
      if (first.compareTo ("create") == 0) {
        
        String second = getValue (split, ' ', 1);
        Serial.println (second);
        
        if (second.compareTo ("substrate") == 0) {
          // TODO: create substrate
          
          String uid = getValue (split, ' ', 2);
          
          // TODO: Create the substrate with the specified UID.
          
        } else if (second.compareTo ("behavior") == 0) {
          
          String behaviorType = getValue (split, ' ', 2);
          
          // Get the propagated UUID
          int behaviorUuid = getValue (split, ' ', 3).toInt (); // TODO: Move/Remove this! Come up with a better way to do this!
          
          Serial.println (behaviorType);
          
          if (behaviorType.compareTo ("input") == 0) {
        
            // Parse parameters
            int pin = getValue (split, ' ', 4).toInt();
            String signal = getValue (split, ' ', 5);
            
            // Create behavior and add it to the behavior substrate
            Behavior* behavior = Create_Input_Behavior (substrate, pin, signal);
            (*behavior).uid = behaviorUuid; // TODO: Move/Remove this! Come up with a better way to do this!
            Sequence* sequence = (*substrate).sequences;
            Update_Behavior_Sequence (behavior, sequence);
            
            // TODO: Propagate to any subscribers to this device! (stored "beneath" the interpreter, for the device).
            
          } else if (behaviorType.compareTo ("output") == 0) {
        
            // Parse parameters
            int pin = getValue (split, ' ', 4).toInt();
            String signal = getValue (split, ' ', 5);
            String data = getValue (split, ' ', 6);
            
            // Create behavior and add it to the behavior substrate
            Behavior* behavior = Create_Output_Behavior (substrate, pin, signal, data);
            (*behavior).uid = behaviorUuid; // TODO: Move/Remove this! Come up with a better way to do this!
            Sequence* sequence = (*substrate).sequences;
            Update_Behavior_Sequence (behavior, sequence);
            
            // TODO: Propagate to any subscribers to this device! (stored "beneath" the interpreter, for the device).
            
          } else if (behaviorType.compareTo ("delay") == 0) {
        
            // Parse parameters
            int milliseconds = getValue (split, ' ', 4).toInt();
            
            // Create behavior and add it to the behavior substrate
            Behavior* behavior = Create_Delay_Behavior (substrate, milliseconds);
            (*behavior).uid = behaviorUuid; // TODO: Move/Remove this! Come up with a better way to do this!
            Sequence* sequence = (*substrate).sequences;
            Update_Behavior_Sequence (behavior, sequence);
            
            // TODO: Propagate to any subscribers to this device! (stored "beneath" the interpreter, for the device).
            
          } else if (behaviorType.compareTo ("sound") == 0) {
        
            // Parse parameters
            int note = getValue (split, ' ', 4).toInt();
            int duration = getValue (split, ' ', 5).toInt();
//            String signal = getValue (split, ' ', 4);
//            String data = getValue (split, ' ', 5);
            
            // Create behavior and add it to the behavior substrate
            Behavior* behavior = Create_Sound_Behavior (substrate, note, duration);
            (*behavior).uid = behaviorUuid; // TODO: Move/Remove this! Come up with a better way to do this!
            Sequence* sequence = (*substrate).sequences;
            Update_Behavior_Sequence (behavior, sequence);
            
            // TODO: Propagate to any subscribers to this device! (stored "beneath" the interpreter, for the device).
            
          } else {
            
            // TODO: Implement "custom" cloud "RPC" behavior.
            
          }
          
        } else if (second.compareTo ("loop") == 0) {
          
          // TODO: create loop
          
        } 
//        else if (first.compareTo ("line")) {
//          // TODO: Substrate
//        } else if (first.compareTo ("dot")) {
//          // TODO: Substrate
//        }
        
      } else if (first.compareTo ("update") == 0) {
        
        String second = getValue (split, ' ', 1);
        Serial.println (second);
        
        if (second.compareTo ("substrate") == 0) {
          // TODO: create substrate
          
          String uid = getValue (split, ' ', 2);
          
          // TODO: Create the substrate with the specified UID.
          
        } else if (second.compareTo ("behavior") == 0) {
          
          // TODO: create behavior
          int uuid = getValue (split, ' ', 2).toInt ();
          
          Serial.println (uuid);
          
          Behavior* behavior = Get_Behavior (uuid);
          Serial.print ("found behavior: "); Serial.print ((int) behavior); Serial.print ("\n");
          Sound* sound = Get_Sound_Behavior (behavior);
              
          if ((*behavior).type == BEHAVIOR_TYPE_SOUND) {
            
            int note = getValue (split, ' ', 3).toInt();
            int duration = getValue (split, ' ', 4).toInt();
            
            // TODO: Check for validity
            
            (*sound).note = note;
            (*sound).duration = duration;
          }
          
//          if (behaviorType.compareTo ("input") == 0) {
//        
//            // Parse parameters
//            int pin = getValue (split, ' ', 3).toInt();
//            String signal = getValue (split, ' ', 4);
//            
//            // Create behavior and add it to the behavior substrate
//            Behavior* behavior = Create_Input_Behavior (substrate, pin, signal);
//            Sequence* sequence = (*substrate).sequences;
//            Update_Behavior_Sequence (behavior, sequence);
//            
//            // TODO: Propagate to any subscribers to this device! (stored "beneath" the interpreter, for the device).
//            
//          }
        }
      }
      
    }
  }
}

//String getValue (String data, char separator, int index) {
//  
//  int found = 0;
//  int strIndex[] = { 0, -1 };
//  int maxIndex = data.length() - 1;
//
//  for (int i = 0; i <= maxIndex && found <= index; i++) {
//    if (data.charAt(i) == separator || i == maxIndex) {
//        found++;
//        strIndex[0] = strIndex[1] + 1;
//        strIndex[1] = (i == maxIndex) ? i + 1 : i;
//    }
//  }
//
//  return found > index ? data.substring (strIndex[0], strIndex[1]) : "";
//}
//
//int getValueCount (String data, char separator) {
//  
//  int count = 0;
//  int maxIndex = data.length() - 1;
//
//  for (int i = 0; i < data.length (); i++) {
//    if (data.charAt(i) == separator) {
//      count++;
//    }
//  }
//  
//  if (count > 0) {
//    count = count + 1;
//  }
//
//  return count;
//}

#endif
