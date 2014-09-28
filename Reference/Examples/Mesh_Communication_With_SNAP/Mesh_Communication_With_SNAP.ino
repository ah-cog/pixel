// set this to the hardware serial port you wish to use
#define MESH_SERIAL Serial1

String getValue(String data, char separator, int index);

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

#define SERIAL_BUFFER_LIMIT 64
int serialBufferSize = 0;
char serialBuffer[SERIAL_BUFFER_LIMIT];

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

#define NEIGHBOR_LIMIT 50
int neighborCount = 0;
int neighbors[NEIGHBOR_LIMIT];

boolean isReading = false;
boolean isWriting = false;

void loop () {
  
  setupPlatform ();
  
  // TODO: Once platform has a UUID, broadcast a message to request other platforms to respond with their address (directly if possible), so this platform can set up it's table of neighbors.
  
  if (hasPlatformUuid) {

    // Broadcast device's address (UUID)
    unsigned long currentTime = millis ();
    if (currentTime - lastBroadcastTime > broadcastTimeout) {
      
      if (isReading == false) {
        isWriting = true;
      
        // MESH_SERIAL.write ('!');
        String data = String ("{ uuid: ") + String (platformUuid) + String (" , type: 'keep-alive' }");
//        Serial.println (data);
        const int serialBufferSize = 64;
        char charData[serialBufferSize];
        data.toCharArray (charData, serialBufferSize);
        
//        Serial.println (charData);
        
        int bytesSent = MESH_SERIAL.write (charData);
//        Serial.print ("sent "); Serial.print (bytesSent); Serial.print (" bytes\n\n");
        
        lastBroadcastTime = millis ();
        
        if (bytesSent >= data.length ()) {
          isWriting = false;
        }
      }
    }
    
//    // Relay data received via the serial console over mesh
//    // TODO: (?) Remove this, eventually!
//    if (Serial.available() > 0) {
//      incomingByte = Serial.read ();
//      // Serial.print("USB received: ");
//      // Serial.println(incomingByte, DEC);
//      // HWSERIAL.print("USB received:");
//      MESH_SERIAL.print ((char) incomingByte);
//      MESH_SERIAL.flush ();
//      /* Serial2.print ((char) incomingByte); */
//    }
    
    // Receive any data received over the mesh network.
    if (isWriting == false) {
      if (MESH_SERIAL.available () > 0) {
        isReading = true;
        
        int incomingByte = MESH_SERIAL.read ();
        // Serial.print("UART received: ");
//        Serial.print ((char) incomingByte);
        
        if (incomingByte == '}') {
          
          // Terminate the buffer
          serialBuffer[serialBufferSize] = incomingByte;
          serialBufferSize = (serialBufferSize + 1) % SERIAL_BUFFER_LIMIT;
          serialBuffer[serialBufferSize] = '\0';
          
          // TODO: Terminate the buffer and return it for parsing!
          String uuidParameter = String (serialBuffer);
          int neighborUuid = getValue(uuidParameter, ' ', 2).toInt ();
          
          boolean hasNeighbor = false;
          for (int i = 0; i < neighborCount; i++) {
            if (neighbors[i] == neighborUuid) {
              hasNeighbor = true;
              break;
            }
          }
          if (hasNeighbor == false) {
            neighbors[neighborCount] = neighborUuid;
            neighborCount++;
            Serial.print ("Added neighbor "); Serial.print (neighborCount); Serial.print (": "); Serial.print (neighborUuid); Serial.print ("\n");
          }
          
          // TODO: Check timestamps when last received a broadcast, and ping those not reached for a long time, and remove them if needed.
          
          serialBufferSize = 0;
          
          Serial.println (neighborUuid);
          
        } else {
          
          serialBuffer[serialBufferSize] = incomingByte;
          serialBufferSize = (serialBufferSize + 1) % SERIAL_BUFFER_LIMIT;
          
        }
        // Serial2.print ((char) incomingByte);
        // HWSERIAL.print("UART received:");
        // HWSERIAL.println(incomingByte, DEC);
        
        // TODO: Buffer the data received over mesh until the message is completely received.
        
        isReading = false;
      }
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

String getValue (String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
