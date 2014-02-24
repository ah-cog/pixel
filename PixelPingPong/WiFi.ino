uint16_t checkFirmwareVersion(void) {
  uint8_t  major, minor;
  uint16_t version = 0;
  
#ifndef CC3000_TINY_DRIVER
  if(!cc3000.getFirmwareVersion(&major, &minor)) {
    Serial.println(F("Unable to retrieve the firmware version!\r\n"));
  } else {
    Serial.print(F("Firmware V. : "));
    Serial.print(major); Serial.print(F(".")); Serial.println(minor);
    version = ((uint16_t)major << 8) | minor;
  }
#endif
  return version;
}
