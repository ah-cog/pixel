#define DEVICE_SERIAL Serial3

String request = "{ message: \"hello there\" }"; // i.e., the JSON received and possibly updated for serial transmission by web server
String syncRequest = "{ sync: \"true\", message: \"hello there\" }"; // i.e., the JSON received and possibly updated for serial transmission by web server
boolean messageSent = false;

String serialReceiveBuffer;
String syncSerialReceiveBuffer;

#define SERIAL_SEND_BUFFER_SIZE 32
char serialSendBuffer[SERIAL_SEND_BUFFER_SIZE];

void setup () {
  delay (3000);
  Serial.begin (9600);
  DEVICE_SERIAL.begin (115200);
}

// TODO:
// - create message queue containig sync and async messages
// - add async messages to the end of the queue
// - insert sync messages to the first position in the queue that isn't occupied by another sync message, and push subsequent async messages back
// - if an async message is in progress when a sync message is added to the queue, first finish sending it, then process sync messages, one at a time

void loop () {

  if (DEVICE_SERIAL.available () > 0) {
      
      int incomingByte = DEVICE_SERIAL.read ();
      
//      Serial.print ((char) incomingByte);
      
      // Buffer the received byte
      serialReceiveBuffer += ((char) incomingByte);
      
//      // If the entire message has been received, either handle it now (for sync messages) or add it to the processing queue.
//      if (serialReceiveBuffer.endsWith ("}")) {
//        Serial.print ("Response: "); Serial.print (serialReceiveBuffer); Serial.print ("\n");
//        serialReceiveBuffer = "";
//        
//        messageSent = false;
//      }
  }
  
  if (messageSent == false) {
    Send_Message_Sync (syncRequest);
    Serial.println ("Message sent.");
  } else {
    Serial.println ("messageSent is false");
  }
  
//  int bytesSent = DEVICE_SERIAL.write ("Ping!\n");
}

boolean Send_Message_Sync (String message) {
  Serial.println ("Send_Message_Sync");
  
  // Send message over serial interface
  for (int i = 0; i < message.length(); i++) {
    DEVICE_SERIAL.write (message.charAt (i));
  }
  messageSent = true;
  
  // Wait for a response before proceeding:
  
  boolean receivedResponse = false;
  syncSerialReceiveBuffer = "";
//  int timeout = 5000; // The maximum wait time for a response before continuing
  
  while (receivedResponse == false) {
    if (DEVICE_SERIAL.available () > 0) {
        
        int incomingByte = DEVICE_SERIAL.read ();
        
  //      Serial.print ((char) incomingByte);
        
        // Buffer the received byte
        syncSerialReceiveBuffer += ((char) incomingByte);
        
        // If the entire message has been received, either handle it now (for sync messages) or add it to the processing queue.
        if (syncSerialReceiveBuffer.endsWith ("}")) {
          Serial.print ("Sync response: "); Serial.print (syncSerialReceiveBuffer); Serial.print ("\n");
          
          messageSent = false;
          receivedResponse = true;
        }
    }
  }
  
  Serial.println ("Done.");
  
}

boolean Send_Message_Async (String message) {
  
  for (int i = 0; i < message.length(); i++) {
    DEVICE_SERIAL.write (message.charAt (i));
  }

  
//  message.toCharArray (serialSendBuffer, SERIAL_SEND_BUFFER_SIZE);
//  DEVICE_SERIAL.write (serialSendBuffer);
  
}
