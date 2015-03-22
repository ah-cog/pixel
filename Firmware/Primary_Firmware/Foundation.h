#ifndef FOUNDATION_H
#define FOUNDATION_H

#define EEPROM_SIZE 512

boolean Clear_Eeprom () {
  // write a 0 to all bytes of the EEPROM
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write (i, 0);
  }
}

#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

boolean Restart () {
  
  // TODO: Send pre-Restart Direct_Message (flush all direct messages whenever this is called)
  // Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice arrival");
  
  // 0000101111110100000000000000100
  // Assert [2]SYSRESETREQ
  WRITE_RESTART(0x5FA0004);
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
  
    // Version 4 UUID: https://en.wikipedia.org/wiki/Universally_unique_identifier
//    char generatedFoundationUuid[] = "c6ade405-3b5d-4783-8d2e-ac53d429a857"; // Module 1
//    char generatedFoundationUuid[] = "d9c95b97-fbcc-484f-bc61-2572c4a00d9c"; // Module 2
    char generatedFoundationUuid[] = "cad165c7-2238-4455-9f85-7f025a9ddb6f"; // Module 3
//    char generatedFoundationUuid[] = "5f29d296-d444-49e5-8988-5b0bd71b3dcc"; // Module 4
//    char generatedFoundationUuid[] = "118b8b18-a851-49fa-aef9-b8f5b18da90d"; // Module 5
    // TODO: char* generateFoundationUuid ()
    
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
  
  // Print the foundation's UUID
  Serial.print ("Foundation UUID: "); for (int i = 0; i < UUID_SIZE; i++) { Serial.print ((char) foundationUuid[i]); } Serial.print ("\n");
  
}

#endif
