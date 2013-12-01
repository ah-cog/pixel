#include <SoftwareSerial.h>
#include <RadioBlock.h>

//Pins connected to RadioBlock pins 1/2/3/4
RadioBlockSerialInterface interface = RadioBlockSerialInterface(-1,-1,8,4);

uint8_t payload[] = { 2 };

#define CODE_TEMP   1
#define CODE_ALARM  2
#define CODE_LED    3
#define CODE_VALVE  4

#define OUR_ADDRESS   0x1514
#define THEIR_ADDRESS 0x0003

void setup()  
{
  interface.begin();  

  //Give RadioBlock time to init
  delay(500);

  //Tell the world we are alive  
  interface.setLED(true);
  delay(1000);
  interface.setLED(false);
  
  interface.setChannel(15);
  interface.setPanID(0xBAAD);
  interface.setAddress(OUR_ADDRESS);
    
  Serial.begin(9600); 
  Serial.println("Starting...");
}

void loop() // run over and over
{
  //New Message
  if (interface.getResponse().isAvailable()) {
    Serial.println("New Response...");
  }
  
  //Oops?
  if (interface.getResponse().isError()) {
    Serial.println("You had ONE job Arduino. ONE job.");
  }
  
  delay(2000);
  
  //We use the 'setupMessage()' call if we want to use a bunch of data,
  //otherwise can use sendData() calls to directly send a few bytes
  
  //This is the OTHER guys address
  interface.setupMessage(THEIR_ADDRESS);
  
  // Package the data payload for transmission
  interface.addData(1, (unsigned char) 0x13); // TYPE_UINT8
  interface.addData(1, (char) 0x14); // TYPE_INT8
  interface.addData(3, (unsigned short int) 0xFFFD); // TYPE_UINT16
  interface.addData(1, (short) 0xABCD); // TYPE_INT16
  interface.addData(14, (unsigned long) 0xDDDDCCAA); // TYPE_UINT32
  interface.addData(9, (long) 0xFF03CCAA); // TYPE_INT32
  
//  //Send state of pot (potentimeter, not drug manufacturing)
//  interface.addData(CODE_VALVE, analogRead(1));
//  
//  //Toggle other other guys LED on RadioBlock
//  interface.addData(CODE_LED, 1);
 
  //Send data OTA
  interface.sendMessage();  
}
