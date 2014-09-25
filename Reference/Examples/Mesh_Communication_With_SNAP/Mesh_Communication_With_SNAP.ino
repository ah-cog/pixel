// set this to the hardware serial port you wish to use
#define MESH_SERIAL Serial1

void setup() {
	Serial.begin (9600);
        MESH_SERIAL.begin (9600);
        Serial2.begin (9600);
}

long platformUuid = 0L;
boolean hasPlatformUuid = false;

long getPlatformUuid () {
  return 4L;
}

boolean setupPlatform () {
  
  // Get the device UUID used for communication over the local mesh network.
  while (hasPlatformUuid == false) {
    
    platformUuid = getPlatformUuid (); // Assing the device UUID
    
    // Check if a valid UUID has been assigned to th
    if (platformUuid != 0L) {
      hasPlatformUuid = true;
      
      return false;
    }
  }
  
  return false;
}

void loop () {
  
  setupPlatform ();
  Serial.println (platformUuid);
  
  // TODO: Once platform has a UUID, broadcast a message to request other platforms to respond with their address (directly if possible), so this platform can set up it's table of neighbors.
  
  int incomingByte;
  
  if (Serial.available() > 0) {
    incomingByte = Serial.read ();
    // Serial.print("USB received: ");
    // Serial.println(incomingByte, DEC);
    // HWSERIAL.print("USB received:");
    MESH_SERIAL.print ((char) incomingByte);
    MESH_SERIAL.flush ();
    Serial2.print ((char) incomingByte);
  }
  
  // Receive any data received over the mesh network.
  if (MESH_SERIAL.available () > 0) {
    incomingByte = MESH_SERIAL.read();
    // Serial.print("UART received: ");
    Serial.print ((char) incomingByte);
    // Serial2.print ((char) incomingByte);
    // HWSERIAL.print("UART received:");
    // HWSERIAL.println(incomingByte, DEC);
    
    // TODO: Buffer the data received over mesh until the message is completely received.
  }
  
  if (Serial2.available () > 0) {
    incomingByte = Serial2.read();
    // Serial.print("UART received: ");
    Serial.print ((char) incomingByte);
    // HWSERIAL.print ((char) incomingByte);
    // HWSERIAL.print("UART received:");
    // HWSERIAL.println(incomingByte, DEC);
  }
}
