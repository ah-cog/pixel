// set this to the hardware serial port you wish to use
#define MESH_SERIAL Serial1

void setup () {
  Serial.begin (9600);
  MESH_SERIAL.begin (9600);
  /* Serial2.begin (9600); */
  
  // Setup random number generator
  randomSeed (analogRead (0));
}

long platformUuid = 0L;
boolean hasPlatformUuid = false;

long getPlatformUuid () {
  const int maximumNumber = 300;
  long randomNumber = random (maximumNumber);
  return randomNumber;
}

boolean setupPlatform () {
  
  // Get the device UUID used for communication over the local mesh network.
  while (hasPlatformUuid == false) {
    
    platformUuid = getPlatformUuid (); // Assing the device UUID
    
    // Check if a valid UUID has been assigned to the platform
    if (platformUuid != 0L) {
      hasPlatformUuid = true;
      
      Serial.println (platformUuid);
      
      return false;
    }
  }
  
  return false;
}

unsigned long lastBroadcastTime = 0L;
unsigned long broadcastTimeout = 1000L;

void loop () {
  
  setupPlatform ();
  
  // TODO: Once platform has a UUID, broadcast a message to request other platforms to respond with their address (directly if possible), so this platform can set up it's table of neighbors.
  
  if (hasPlatformUuid) {
    int incomingByte;

    unsigned long currentTime = millis ();
    if (currentTime - lastBroadcastTime > broadcastTimeout) {
      
      // MESH_SERIAL.write ('!');
      String data = String (platformUuid);
      const int serialBufferSize = 32;
      char charData[serialBufferSize];
      data.toCharArray (charData, serialBufferSize);
      
      MESH_SERIAL.write (charData);
      
      lastBroadcastTime = millis ();
    }
    
    if (Serial.available() > 0) {
      incomingByte = Serial.read ();
      // Serial.print("USB received: ");
      // Serial.println(incomingByte, DEC);
      // HWSERIAL.print("USB received:");
      MESH_SERIAL.print ((char) incomingByte);
      MESH_SERIAL.flush ();
      /* Serial2.print ((char) incomingByte); */
    }
    
    // Receive any data received over the mesh network.
    if (MESH_SERIAL.available () > 0) {
      incomingByte = MESH_SERIAL.read ();
      // Serial.print("UART received: ");
      Serial.print ((char) incomingByte);
      // Serial2.print ((char) incomingByte);
      // HWSERIAL.print("UART received:");
      // HWSERIAL.println(incomingByte, DEC);
      
      // TODO: Buffer the data received over mesh until the message is completely received.
    }
    
    /*
    if (Serial2.available () > 0) {
      incomingByte = Serial2.read();
      // Serial.print("UART received: ");
      Serial.print ((char) incomingByte);
      // HWSERIAL.print ((char) incomingByte);
      // HWSERIAL.print("UART received:");
      // HWSERIAL.println(incomingByte, DEC);
    }
    */
  }
}
