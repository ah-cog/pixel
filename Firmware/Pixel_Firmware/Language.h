#ifndef LANGUAGE_H
#define LANGUAGE_H

void Perform_Shell_Behavior (String consoleMessage);

String previousConsoleInput = "";
char consoleBuffer[64] = { 0 };
int consoleBufferSize = 0;

/**
 * This is the basic shell for the device.
 */
void Get_Console () {
  
//  terminalBufferSize = 0;

  // Read data on serial terminal (if any)
  while (Serial.available () > 0) {

    int incomingByte = Serial.read (); // receive a byte as character
    char c = (char) incomingByte;
    
    if (c == '\n') {
      consoleBuffer[consoleBufferSize] = '\0'; // Terminate console message buffer
      
      // Create a String from the terminal buffer
      String consoleMessage = String (consoleBuffer);
      
      if (consoleMessage.length () == 0) {
        consoleMessage = previousConsoleInput;
      } else {
        
        previousConsoleInput = consoleMessage;
        
        // TODO: Don't separate "shell" behavior from other behaviors!
        Perform_Shell_Behavior (consoleMessage);
        
        consoleBufferSize = 0;
      }
      
      break;
    }
    
    // Copy byte into message buffer
    consoleBuffer[consoleBufferSize] = c;
    consoleBufferSize++;
  }
}

// TODO: Make this callable remotely from other nodes...
// TODO: Add command to set default module (e.g., "enter"), and return to the current module (e.g., "exit")
void Perform_Shell_Behavior (String message) {
  
  // Echo the message
  Serial.print ("> ");
  Serial.print (message);
  Serial.print ("\n");
  
  int wordCount = getValueCount (message, ' ');
  String firstWord = getValue (message, ' ', 0);
  
  // Parse and process the message
  if (firstWord.compareTo ("ip") == 0) {
    
    Serial.println (ipAddress);
    
  } else if (firstWord.compareTo ("self") == 0) { // self
    
    Serial.println (platformUuid);
    
  } else if (firstWord.compareTo ("neighbors") == 0) { // others
    
    for (int i = 0; i < neighborCount; i++) {
      Serial.print (i);
      Serial.print ("\t");
      Serial.print (neighbors[i]);
      Serial.print ("\n");
    }
    
  } else if (firstWord.compareTo ("outputs") == 0) {
    
    for (int i = 0; i < nextModuleCount; i++) {
      Serial.print (i);
      Serial.print ("\t");
      Serial.print (nextModules[i]);
      Serial.print ("\n");
    }
    
  } else if (firstWord.compareTo ("inputs") == 0) {
    
    for (int i = 0; i < previousModuleCount; i++) {
      Serial.print (i);
      Serial.print ("\t");
      Serial.print (previousModules[i]);
      Serial.print ("\n");
    }
    
  } else if (firstWord.compareTo ("color") == 0) {
    
    // Check second word
    String secondWord = getValue (message, ' ', 1);
    
    if (secondWord.compareTo ("input") == 0) {
      
      int red = getValue (message, ' ', 2).toInt ();
      int green = getValue (message, ' ', 3).toInt ();
      int blue = getValue (message, ' ', 4).toInt ();
      
      Update_Input_Color (red, green, blue);
      
      Serial.println ("Updating input color.");
      
    } else if (secondWord.compareTo ("output") == 0) {
      
      int red = getValue (message, ' ', 2).toInt ();
      int green = getValue (message, ' ', 3).toInt ();
      int blue = getValue (message, ' ', 4).toInt ();
      
      Update_Output_Color (red, green, blue);
      
      Serial.println ("Updating output color.");
      
    } else {
    
      int red = getValue (message, ' ', 1).toInt ();
      int green = getValue (message, ' ', 2).toInt ();
      int blue = getValue (message, ' ', 3).toInt ();
      
      Update_Input_Color (red, green, blue);
      Update_Output_Color (red, green, blue);
      
      Serial.println ("Updating colors.");
    }
    
    delay (1000);
    
  } else if (firstWord.compareTo ("play") == 0) {
    
    // e.g., "play note 1047 1000"
    String sound = getValue (message, ' ', 1);
    int note = getValue (message, ' ', 2).toInt ();
    int duration = getValue (message, ' ', 3).toInt ();
    Play_Note (note, duration);
//          delay (1000);
//          Stop_Sound ();

  } else if (firstWord.compareTo ("orientation") == 0) {
    
    Serial.print ("(");
    Serial.print (roll); Serial.print (", ");
    Serial.print (pitch); Serial.print (", ");
    Serial.print (yaw); Serial.print (")\n");
    
  } else if (firstWord.compareTo ("ping") == 0) {
    
    // TODO: Broadcast ping to all other devices on mesh, requesting them to report their address.
    
    // TODO: Add TCP/IP ping for Wi-Fi, too!
    
  } else if (firstWord.compareTo ("enter") == 0) {
    
    // e.g., "enter neighbor 34"
    
  } else if (firstWord.compareTo ("exit") == 0) {
    
    // e.g., "exit"
    
  } else if (firstWord.compareTo ("remember") == 0) {
    
    // TODO: Add (key, value) pair to memory
    
  } else if (firstWord.compareTo ("recall") == 0) {
    
    // TODO: Recall (key, value) pair by key
    
  } else if (firstWord.compareTo ("propagate") == 0) {
    
    // TODO: Prefix command with "propagate" to broadcast
    
  } else if (firstWord.compareTo ("mirror") == 0) {
    
    // TODO: Prefix command with "propagate" to broadcast
    
  } else if (firstWord.compareTo ("mimmic") == 0) {
    
    // TODO: Prefix command with "propagate" to broadcast
    
  } else if (firstWord.compareTo ("imagine") == 0) { // simulate --> creates a simulated programmable machine that can be enacted on this device
    
    // TODO: Prefix command with "propagate" to broadcast
    
  } else if (firstWord.compareTo ("pretend") == 0) { // enacts a simulated machine on this hardware while preserving the one currently on the device for easy reversion
    
    // TODO: Prefix command with "propagate" to broadcast
    
  } else if (firstWord.compareTo ("reboot") == 0) {
    
    Serial.println ("BBL.");
    Restart ();
    
  } else {
    
    Serial.println ("That doesn't do anything.");
    
  }
}

#endif
