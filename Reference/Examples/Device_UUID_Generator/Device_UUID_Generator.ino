/*
 * EEPROM UUID
 *
 * Creates a UUID and stores it in the EEPROM.
 *
 * This example code is in the public domain.
 */

#include <EEPROM.h>

boolean clearEeprom () {
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
}


// Foundation State:
boolean hasFoundationUuid = false;

#define UUID_SIZE 36
int foundationUuidSignatureMemoryAddress = 0;
int foundationUuidMemoryAddress = foundationUuidSignatureMemoryAddress + 1;
char foundationUuid[UUID_SIZE];
boolean setupFoundation () {
  // Check if UUID has been written to EEPROM, and if so, read it into RAM (and load it into the Looper engine).
  // If not, generate a UUID into memory
  
  // Read the UUID signature byte. If it is equal to '!', then assume a UUID has been written.
  byte foundationUuidSignatureByte = EEPROM.read (foundationUuidSignatureMemoryAddress);

  // Check if the UUID signature byte (i.e., '!') has been written to memory.
  if (foundationUuidSignatureByte != '!') {
    // The UUID is not present in EEPROM, so generate one and write it to EEPROM before proceeding.
  
    // Version 4 UUID: c6ade405-3b5d-4783-8d2e-ac53d429a857
    // char generatedFoundationUuid[] = "c6ade405-3b5d-4783-8d2e-ac53d429a857"; // Module 1
    char generatedFoundationUuid[] = "d9c95b97-fbcc-484f-bc61-2572c4a00d9c"; // Module 2
    
    // Write UUID signature to EEPROM
    EEPROM.write (foundationUuidSignatureMemoryAddress, '!'); // Write foundation UUID signature (i.e., the '!' symbol)
  
    // Write UUID to EEPROM
    int i = 0;
    for (int address = foundationUuidMemoryAddress; address < (foundationUuidMemoryAddress + UUID_SIZE); address++) {
      EEPROM.write (address, generatedFoundationUuid[i]);
      i++;
    }
    
  }
  
  // Read UUID from EEPROM
  int i = 0;
  for (int address = foundationUuidMemoryAddress; address < (foundationUuidMemoryAddress + UUID_SIZE); address++) {
    foundationUuid[i] = EEPROM.read (address);
    i++;
  }
  
  Serial.print ("Foundation UUID: "); for (int i = 0; i < UUID_SIZE; i++) { Serial.print ((char) foundationUuid[i]); } Serial.print ("\n");
  
}

void setup()
{
  delay (2000);
  
  if (hasFoundationUuid == false) {
    setupFoundation ();
    hasFoundationUuid = true;
  }
  
  // initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}

void loop()
{ 
//  // read a byte from the current address of the EEPROM
//  value = EEPROM.read(address);
//  
//  Serial.print(address);
//  Serial.print("\t");
//  Serial.print(value, DEC);
//  Serial.println();
//  
//  // advance to the next address of the EEPROM
//  address = address + 1;
//  
//  // there are only 512 bytes of EEPROM, from 0 to 511, so if we're
//  // on address 512, wrap around to address 0
//  if (address == 512)
//    address = 0;
//    
//  delay(500);
}
