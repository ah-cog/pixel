#define DEVICE_SERIAL Serial3

void setup () {
  Serial.begin (9600);
  DEVICE_SERIAL.begin (115200);
}

void loop () {
  if (DEVICE_SERIAL.available () > 0) {
      
      int incomingByte = DEVICE_SERIAL.read ();
      
      Serial.print ((char) incomingByte);
  }
  
//  int bytesSent = DEVICE_SERIAL.write ("Ping!\n");
}
