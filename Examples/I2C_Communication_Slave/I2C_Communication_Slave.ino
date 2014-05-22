// Notes for Teensy 3.1 (i.e., heuristics to get the CC3000 module working, based on experience):
// - Set to 48 MHz
// - Set clock to SPI_CLOCK_DIV8 (SPI_CLOCK_DIV2 also worked, but not too reliably)

// Adafruit Breakout Board:
// - Firmware V. : 1.24

#include <Wire.h>
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"

// <wifi setup>

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   1  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  3
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV2); // you can change this clock speed

#define WLAN_SSID "Hackerspace" // Cannot be longer than 32 characters!
#define WLAN_PASS "MakingIsFun!"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY WLAN_SEC_WPA2

#define LISTEN_PORT 80 // What TCP port to listen on for connections.  The echo protocol uses port 7.

Adafruit_CC3000_Server httpServer(LISTEN_PORT);

// </wifi setup>

#include "BehaviorInterpretter.h"

#define I2C_DEVICE_ADDRESS 4

// TODO: Implement list of changes to make to send to the Master (which executes gestural and the behavior code for the module)
// - TODO: Include status: "new", "sending", "sent", "confirmed" (after which, they're deleted)

// TODO: Implement web server and request handlers that, in response, update the queue of
//       changes to (1) make to Looper, and (2) to queue for sending to the other device 
//       over I2C upon request.

void setup () {
  Serial.begin(115200);             // start serial for output
  Serial.println("Slave device initializing...");
  
  ////// WIFI
  
  /* Initialise the module */
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin()) {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1) {
      Serial.println(F("Couldn't begin()! Check your wiring?"));
    };
  }
  
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  
  uint16_t firmware = checkFirmwareVersion();
  if (firmware < 0x113) {
    Serial.println(F("Wrong firmware version!"));
    for(;;);
  } 
  
//  displayMACAddress();
  
  
  
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP()) {
    delay(100); // ToDo: Insert a DHCP timeout!
//    Serial.println("DHCP Timeout");
  }

  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  //
  // You can safely remove this to save some flash memory!
  //
  Serial.println(F("\r\nNOTE: This sketch may cause problems with other sketches"));
  Serial.println(F("since the .disconnect() function is never called, so the"));
  Serial.println(F("AP may refuse connection requests from the CC3000 until a"));
  Serial.println(F("timeout period passes.  This is normal behaviour since"));
  Serial.println(F("there isn't an obvious moment to disconnect with a server.\r\n"));
  
  // Start listening for connections
  httpServer.begin();
  
  Serial.println(F("Listening for connections..."));
  
  
  
  
  
  Wire.begin(I2C_DEVICE_ADDRESS); // Join I2C bus with the device's address
  Wire.onReceive(receiveEvent);   // Register event handler to receive data from the master I2C device
  Wire.onRequest(requestEvent);   // Event handler to respond to a request for data from the I2C master device
  
  
  // Add testing operations to queue (these would usually come from the web UI)
  insertBehaviorNode(0, 3, 4, 0, 1, 1);
  insertBehaviorNode(1, 4, 5, 0, 1, 1);
  insertBehaviorNode(2, 5, 6, 0, 1, 1);
  insertBehaviorNode(3, 6, 7, 0, 1, 1);
  insertBehaviorNode(4, 7, 8, 0, 1, 1);
  insertBehaviorNode(5, 8, 9, 0, 1, 1);
  insertBehaviorNode(6, 20, 10, 0, 1, 1);
}

uint16_t checkFirmwareVersion(void)
{
  uint8_t major, minor;
  uint16_t version;
  
#ifndef CC3000_TINY_DRIVER  
  if(!cc3000.getFirmwareVersion(&major, &minor))
  {
    Serial.println(F("Unable to retrieve the firmware version!\r\n"));
    version = 0;
  }
  else
  {
    Serial.print(F("Firmware V. : "));
    Serial.print(major); Serial.print(F(".")); Serial.println(minor);
    version = major; version <<= 8; version |= minor;
  }
#endif
  return version;
}

void loop () {
  
//  Serial.println("tick");
    
  // Try to get a client which is connected.
  Adafruit_CC3000_ClientRef client = httpServer.available();
  if (client) {
    Serial.println("Client connected.");
    boolean currentLineIsBlank = true;
    while(client.connected()) {
      // Check if there is data available to read.
      if (client.available() > 0) {
        // Read a byte and write it to all clients.
        uint8_t c = client.read();
        //client.write(c);
        
//        Serial.write(c);
        
        if (c == '\n' && currentLineIsBlank) {
          
          // Process URL
          // TODO:
          
          
          // TODO: Only do this when /add-node is called (or whatever the URI will be)
          insertBehaviorNode(6, 20, 10, 0, 1, 1);
          
          
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("<strong>Hello</strong>");
          client.println("</html>");
          break;
        }
        
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    
    // give the web browser time to receive the data
    delay(1);
    
    // Close the connection
    client.close();
  }
  
//  Serial.print("Count: ");
//  Serial.println(behaviorNodeCount);
  
//  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent (int howMany) {
  while (1 < Wire.available ()) { // loop through all but the last
    char c = Wire.read (); // receive byte as a character
    Serial.print (c); // print the character
  }
  int x = Wire.read ();    // receive byte as an integer
  Serial.println (x);         // print the integer
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent () {
//  Wire.write ("13 w d o h "); // respond with message of 6 bytes as expected by master
  
  
  Serial.print("Count: ");
  Serial.println(behaviorNodeCount);
  
  char buf[4]; // "-2147483648\0"
//  Wire.write (itoa(behaviorNodeCount, buf, 10));
  
  if (behaviorNodeCount > 0) {
//    Wire.write (behaviorNodes[0].pin); Wire.write (" ");
    Wire.write (itoa(behaviorNodes[0].pin, buf, 10)); Wire.write (" ");
//    Wire.write (behaviorNodes[0].operation); Wire.write (" ");
    Wire.write (itoa(behaviorNodes[0].operation, buf, 10)); Wire.write (" ");
//    Wire.write (behaviorNodes[0].type); Wire.write (" ");
    Wire.write (itoa(behaviorNodes[0].type, buf, 10)); Wire.write (" ");
//    Wire.write (behaviorNodes[0].mode); Wire.write (" ");
    Wire.write (itoa(behaviorNodes[0].mode, buf, 10)); Wire.write (" ");
//    Wire.write (behaviorNodes[0].value); Wire.write (" ");
    Wire.write (itoa(behaviorNodes[0].value, buf, 10)); Wire.write (" ");
    
    removeBehaviorNode(0);
  }
  
  // Wire.write("3 14 digital high"); // Step 3: Ensures that stat of pin 14 is digital and that it is set to high, doing so as needed
  // Wire.write("4 delay 5"); // Step 4: Adds a delay of 5 seconds in the program
  // Wire.write("6 14 digital low"); // Step 6: Same as above, but sets low rather than high.
  
  // pin, operation, type, mode, value
  // "13 write digital output high"
  // "13 w d o h"
}



//
// Tries to read the IP address and other connection details
//
bool displayConnectionDetails (void) {
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv)) {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  } else {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
