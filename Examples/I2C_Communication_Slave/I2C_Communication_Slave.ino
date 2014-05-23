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
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV4); // you can change this clock speed

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
//  insertBehaviorNode(0, 3, 4, 0, 1, 1);
//  insertBehaviorNode(1, 4, 5, 0, 1, 0);
//  insertBehaviorNode(2, 5, 6, 0, 1, 1);
//  insertBehaviorNode(3, 6, 7, 0, 1, 0);
//  insertBehaviorNode(4, 7, 8, 0, 1, 1);
//  insertBehaviorNode(5, 8, 9, 0, 1, 0);
//  insertBehaviorNode(6, 20, 10, 0, 1, 1);
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

boolean hasRoombaSensorData = false;
uint8_t buf[52];                                        // iRobot sensor data buffer. Packet 6 returns 52 bytes.
char httpRequestUriBuffer[50]; // HTTP request buffer
int bi = 0; // HTTP request buffer index
char* httpRequestParameters[4]; // HTTP request parameters (i.e., key/value pairs encoded in the URI like "?key1=value1&key2=value2")
int httpRequestParameterCount = 0;
//char* httpRequestParameterDictionary[10][2]; // HTTP request parameters (i.e., key/value pairs encoded in the URI like "?key1=value1&key2=value2")
// httpRequestParameterDictionary[paramIndex][key] // key = 0 (returns char*)
// httpRequestParameterDictionary[paramIndex][value] // value = 1 (returns char*)

void loop () {
  
//  Serial.println("tick");
    
  // Try to get a client which is connected.
  Adafruit_CC3000_ClientRef client = httpServer.available();
  
  if (client) {
    Serial.println("new client connected");
    
    bi = 0; // Reset the HTTP request buffer
    int httpRequestLineCount = 0;
    boolean hasReceivedRequestFirstLine = false;
    boolean hasReceivedRequest = false;
//    boolean hasHttpRequest = false;

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    
    // Parse HTTP request header
    char* spaceChar = NULL;
    int spaceCharIndex = 0;
    int httpRequestMethodIndex = 0;
    char* httpRequestMethod = &httpRequestUriBuffer[0]; // Poit the string to the beginning of the HTTP request
    char* httpRequestAddress = NULL;
    char* httpRequestParameterString = NULL;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // Serial.write(c);
        
        // Buffer the first line of the received HTTP request
        if (httpRequestLineCount == 0) {
          httpRequestUriBuffer[bi] = c;
          bi++;
        }
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          
          Serial.println("Status: Received complete HTTP request.");
          hasReceivedRequest = true;
          
        }
        
        if (hasReceivedRequest) {
          
          if (strcmp (httpRequestMethod, "POST") == 0) {
            
            if (strcmp (httpRequestAddress, "/rotate") == 0) { // TODO: Remove this?
              
              Serial.println("Action: Rotate");
              
              // rotate
              
//              roomba.driveDirect(-300 , 300);  // Left/Right Wheel velocity (mm/s)
              delay(500);
            
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              
              // TODO:
              
              break;
              
            } else if (strcmp (httpRequestAddress, "/dock") == 0) {
              
//              roomba.dock(); // "Causes roomba to immediately seek the docking station. No equivalent for Create."
              
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              
              break;
              
            } else if (strcmp (httpRequestAddress, "/drivers") == 0) {
              
//              roomba.drivers(ROOMBA_MASK_DRIVER_2); // "Causes roomba to immediately seek the docking station. No equivalent for Create."
//              #define ROOMBA_MASK_DRIVER_0 0x1
//              #define ROOMBA_MASK_DRIVER_1 0x2
//              #define ROOMBA_MASK_DRIVER_2 0x4
              
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              
              break;
              
            } else if (strcmp (httpRequestAddress, "/utility/togglePower") == 0) {
              
              // Momentarily writes high to the iRobot's "Power Control Toggle" pin. This 
              // pin "Turns iRobot Create on or off on a low-to-high transition".
//              digitalWrite(iRobotPowerControlTogglePin, LOW);
//              digitalWrite(iRobotPowerControlTogglePin, HIGH);
//              delay(50); // TODO: Determine if this needed!
//              digitalWrite(iRobotPowerControlTogglePin, LOW);
              
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              break;
              
            } else if (strcmp (httpRequestAddress, "/utility/isCharging") == 0) {
              
              // Momentarily writes high to the iRobot's "Power Control Toggle" pin. This 
              // pin "Turns iRobot Create on or off on a low-to-high transition".
//              int robotChargingPinState = digitalRead(iRobotIsChargingPin);
              
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              // Return state in JSON format
              client.print("{ \"robotChargingPinState\": ");
//              client.print(robotChargingPinState);
              client.print(" }");
              break;
              
            } else if (strcmp (httpRequestAddress, "/reset") == 0) {
              
//              roomba.reset(); // "Resets the Roomba. It will emit its startup message Caution, this may take several seconds to complete."
              
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              break;
              
            } else if (strcmp (httpRequestAddress, "/start") == 0) {
              
//              roomba.start(); // "Starts the Open Interface and sets the mode to Passive. You must send this before sending any other commands. Initialises the serial port to the baud rate given in the constructor."
              
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              break;
              
            } 
//            else if (strcmp (httpRequestAddress, "/drive") == 0) {
//              
//              // TODO: Parse parameters from ?velocity=300&radius=DriveStraight&duration=500&canInterrupt=true
//              
//              roomba.drive(300, roomba.DriveStraight); // DriveStraight is a special case. See Public Types in Roomba Class Reference
//              // "radius" enum values defined in Roomba library:
//              // roomba.DriveStraight                = 0x8000
//              // roomba.DriveInPlaceClockwise        = 0xFFFF
//              // roomba.DriveInPlaceCounterClockwise = 0x0001
//              delay(500);
//              roomba.drive(0, 0);
//              
//              // roomba.drive(velocity, radius); // DriveStraight is a special case. See Public Types in Roomba Class Reference
//              // delay(duration);
//              
//              // send a standard http response header
//              client.println("HTTP/1.1 200 OK");
//              client.println("Content-Type: text/html");
//              client.println("Connection: close");  // the connection will be closed after completion of the response
//              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
//              client.println();
//              
//              break;
//              
//            }
            else if (strcmp (httpRequestAddress, "/directDrive") == 0) {
              
              // TODO: Parse parameters from ?leftWheelVelocity=300&righWheelVelocity=-300&duration=1000&canInterrupt=true
              
//              roomba.driveDirect(300, 300);   // Left/Right Wheel velocity (mm/s)
              delay(500); // TODO: Make this "non-blocking" with a timer/operation status data structure
//              roomba.drive(0, 0);
              
              //roomba.driveDirect(leftWheelVelocity, righWheelVelocity);   // Left/Right Wheel velocity (mm/s)
              //delay(duration);
              
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              
              break;
              
            } else if (strcmp (httpRequestAddress, "/addnode") == 0) {
              
              //          // TODO: Only do this when /add-node is called (or whatever the URI will be)
              insertBehaviorNode(6, 20, 10, 0, 1, 1);
              
              // Send a standard HTTP response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              
              // TODO: flush
              // TODO: close
              
              break;
              
            } else if (strcmp (httpRequestAddress, "/lighton") == 0) {
              
              //          // TODO: Only do this when /add-node is called (or whatever the URI will be)
              insertBehaviorNode(0, 13, 1, 0, 1, 1);
              
              // Send a standard HTTP response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              
              // TODO: flush
              // TODO: close
              
              break;
              
            } else if (strcmp (httpRequestAddress, "/lightoff") == 0) {
              
              //          // TODO: Only do this when /add-node is called (or whatever the URI will be)
              insertBehaviorNode(0, 13, 1, 0, 1, 0);
              
              // Send a standard HTTP response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              
              // TODO: flush
              // TODO: close
              
              break;
              
            } else if (strcmp (httpRequestAddress, "/delay") == 0) {
              
              //          // TODO: Only do this when /add-node is called (or whatever the URI will be)
              insertBehaviorNode(0, 0, 1, 0, 1, 0);
              
              // Send a standard HTTP response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              
              // TODO: flush
              // TODO: close
              
              break;
              
            } else if (strcmp (httpRequestAddress, "/erase") == 0) {
              
              //          // TODO: Only do this when /add-node is called (or whatever the URI will be)
              insertBehaviorNode(0, 2, 1, 0, 1, 0);
              
              // Send a standard HTTP response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              
              // TODO: flush
              // TODO: close
              
              break;
              
            } else {
              
              // TODO: Default, catch-all GET handler
              
              // send a standard http response header
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              
              break;
              
            } 
            
          } else if (strcmp (httpRequestMethod, "GET") == 0) {
            
            if (strcmp (httpRequestAddress, "/") == 0) {
            
              //          // TODO: Only do this when /add-node is called (or whatever the URI will be)
//              insertBehaviorNode(6, 20, 10, 0, 1, 1);
              
              
              
              handleDefaultHttpRequest(client);
              break;
              
            } else {
              
              // TODO: Default, catch-all GET handler
              
              // send a standard http response header
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              
              break;
              
            }
            
          } else { // Unrecognized HTTP request method (possibly an error, such as a malformed request)
            
            // Send a standard http response header
            client.println("HTTP/1.1 404 Not Found");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            break;
          }
        }
        
        
        if (c == '\n') {
          
          // Increase the HTTP request line count
          httpRequestLineCount++;
          
          // Parse HTTP header
          if (httpRequestLineCount == 1) {
            httpRequestUriBuffer[bi] = NULL; // Terminate string
            
//            Serial.print("REQUEST: ");
//            Serial.println(httpRequestUriBuffer);
//            Serial.println();
            
            /**
             * Parse HTTP request header
             */
            
            // Get HTTP request method (i.e., GET or POST)
            httpRequestMethod = &httpRequestUriBuffer[0];
            spaceChar = strchr(httpRequestMethod, ' ');
            spaceCharIndex = (int) (spaceChar - httpRequestUriBuffer);
            httpRequestUriBuffer[spaceCharIndex] = NULL; // Terminate the request method string
            Serial.println(httpRequestMethod);
            
            // Get HTTP request address
            httpRequestAddress = &httpRequestUriBuffer[spaceCharIndex + 1];
            spaceChar = strchr(httpRequestAddress, ' ');
            spaceCharIndex = (int) (spaceChar - httpRequestUriBuffer);
            httpRequestUriBuffer[spaceCharIndex] = NULL; // Terminate the request method string
            Serial.println(httpRequestAddress);
            
            hasReceivedRequestFirstLine = true; // Flag the first line of the HTTP request as received. This line contains the URI, used to decide how to handle the request.

            /**            
             * Extract and decode URI parameters (i.e., after ?, between &)
             */
            
            char* questionChar = NULL; // Pointer to '?' character in URI
            int questionCharIndex = 0;
            
            // httpRequestParameterString = &httpRequestUriBuffer[spaceCharIndex + 1];
            httpRequestParameterString = httpRequestAddress; // Start searching for '?' at the beginning of the URI.
            questionChar = strchr(httpRequestParameterString, '?'); // Search for the '?' character
            httpRequestParameterCount = 0; // Reset parameter count
            
            Serial.print("Looking for '?'... ");
            if (questionChar != NULL) {
            // if (httpRequestParameterString != NULL) {
              Serial.print("Found!\n");
              
              // Find the '?' character, denoting the beginning of the parameter list
              questionCharIndex = (int) (questionChar - httpRequestUriBuffer); // Get index of '?' character in HTTP request string (of form "POST /drive?velocity=300&radius=15")
              httpRequestParameterString = &httpRequestUriBuffer[questionCharIndex + 1]; // Search for the beginning of the paramter list encoded in the HTTP request
              
              char* ampersandChar = NULL;
              int ampersandCharIndex = -1; // This is used to track the indices of '&' characters during parameter parsing
              ampersandCharIndex = questionCharIndex; // Initialize to position of the last found space character
              
              // Iterate over parameter list and extract parameters
              boolean haveExtractedParameters = false; // Flag indicating whether all parameters have been extracted
              while (!haveExtractedParameters) {
              
                // Search for the next parameter enoded in the URI (if any), by searching for the next occurrence of '&' or ' ' (denoting the end of the parameter list)
                httpRequestParameters[httpRequestParameterCount] = &httpRequestUriBuffer[ampersandCharIndex + 1];
                ampersandChar = strchr(httpRequestParameters[httpRequestParameterCount], '&');
                
                if (ampersandChar != NULL) { // Check if a '&' character was found
                  ampersandCharIndex = (int) (ampersandChar - httpRequestUriBuffer);
                  httpRequestUriBuffer[ampersandCharIndex] = NULL; // Terminate the parameter key/value pair string
                  
                  httpRequestParameterCount++; // Count the parameter and continue to next parameter (if any)
                  
                } else {
                  
                  // If no '&' character found, then find the end of the parameter list
                  // spaceChar = strchr(httpRequestParameterString, ' '); // Find end of parameter list
                  spaceChar = strchr(httpRequestParameterString, '\0'); // Find end of parameter list
                  // if (endChar != NULL) { // Check if a ' ' character was found
                  spaceCharIndex = (int) (spaceChar - httpRequestUriBuffer);
                  httpRequestUriBuffer[spaceCharIndex] = NULL; // Terminate the parameter key/value pair string
                  
                  httpRequestParameterCount++;
                  
                  haveExtractedParameters = true;
                  // }
                  
                  // TODO: Remove the following line. I don't think it's needed since the flag will break the loop on the next iteration.
                  // break; // Break out of the while loop
                }
              }
              
              /**
               * We have extracted parameter key/value pairs. Now, extract them, separating the key and value in each pair.
               */

              // Iterate over paramters' key/value pairs and parse each of them.
              if (haveExtractedParameters) {
                for (int i = 0; i < httpRequestParameterCount; i++) {
                  Serial.println(httpRequestParameters[i]);
                }
              }
              
            } else {
              // The '?' character was not found in the URI, so assume that no parameters exist.
              Serial.print("NOT found!\n");
            }
            
          }
          
          // you're starting a new line
          currentLineIsBlank = true;
          
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
        
        
        
//        if (hasReceivedRequestFirstLine) {
//          hasReceivedRequest = true;
//        }
        
        
        
        
      }
    }
    // give the web browser time to receive the data
    delay(1);
    
    // close the connection:
    client.close();
    Serial.println("client disonnected");
  }
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

boolean handleDefaultHttpRequest(Adafruit_CC3000_ClientRef& client) {
  
  // Send a standard HTTP response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  
  
  
  client.println("<script type=\"text/javascript\">");
  client.println("function addNode(pin, operation, type, mode, value) {");
  client.println("  var http = new XMLHttpRequest();");
  client.println("  var url = \"/addnode\";");
  client.println("  var params = \"pin=\" + pin + \"&operation=\" + operation + \"&type=\" + type + \"&mode=\" + mode + \"&value=\" + value + \"\";");
  // client.println("  var params = \"\";");
  client.println("  http.open(\"POST\", url, true);");
  // Send the proper header information along with the request
  client.println("  http.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");");
  // client.println("http.setRequestHeader(\"Content-length\", params.length);");
  // client.println("http.setRequestHeader(\"Connection\", \"close\");");
  client.println("  http.onreadystatechange = function() { //Call a function when the state changes.");
  client.println("    if(http.readyState == 4 && http.status == 200) {");
  client.println("      console.log(http.responseText);");
  client.println("    }");
  client.println("  }");
  client.println("  http.send(params);");
  client.println("};");
  
  
  client.println("function lightOn() {");
  client.println("  var http = new XMLHttpRequest();");
  client.println("  var url = \"/lighton\";");
  client.println("  var params = \"\";");
  client.println("  http.open(\"POST\", url, true);");
  // Send the proper header information along with the request
  client.println("  http.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");");
  // client.println("http.setRequestHeader(\"Content-length\", params.length);");
  // client.println("http.setRequestHeader(\"Connection\", \"close\");");
  client.println("  http.onreadystatechange = function() { //Call a function when the state changes.");
  client.println("    if(http.readyState == 4 && http.status == 200) {");
  client.println("      console.log(http.responseText);");
  client.println("    }");
  client.println("  }");
  client.println("  http.send(params);");
  client.println("};");
  
  
  client.println("function lightOff() {");
  client.println("  var http = new XMLHttpRequest();");
  client.println("  var url = \"/lightoff\";");
  client.println("  var params = \"\";");
  // client.println("  var params = \"\";");
  client.println("  http.open(\"POST\", url, true);");
  // Send the proper header information along with the request
  client.println("  http.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");");
  // client.println("http.setRequestHeader(\"Content-length\", params.length);");
  // client.println("http.setRequestHeader(\"Connection\", \"close\");");
  client.println("  http.onreadystatechange = function() { //Call a function when the state changes.");
  client.println("    if(http.readyState == 4 && http.status == 200) {");
  client.println("      console.log(http.responseText);");
  client.println("    }");
  client.println("  }");
  client.println("  http.send(params);");
  client.println("};");
  
  
  client.println("function delay(milliseconds) {");
  client.println("  var http = new XMLHttpRequest();");
  client.println("  var url = \"/delay\";");
  client.println("  var params = \"milliseconds=\" + milliseconds + \"\";");
  client.println("  http.open(\"POST\", url, true);");
  // Send the proper header information along with the request
  client.println("  http.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");");
  // client.println("http.setRequestHeader(\"Content-length\", params.length);");
  // client.println("http.setRequestHeader(\"Connection\", \"close\");");
  client.println("  http.onreadystatechange = function() { //Call a function when the state changes.");
  client.println("    if(http.readyState == 4 && http.status == 200) {");
  client.println("      console.log(http.responseText);"); // TODO: Make this actually "activate" the "ghost" node that has been added... once confirmed in the program!
  client.println("    }");
  client.println("  }");
  client.println("  http.send(params);");
  client.println("};");
  
  
  client.println("function erase() {");
  client.println("  var http = new XMLHttpRequest();");
  client.println("  var url = \"/erase\";");
  // client.println("  var params = \"milliseconds=\" + milliseconds + \"\";");
  client.println("  var params = \"\";");
  client.println("  http.open(\"POST\", url, true);");
  // Send the proper header information along with the request
  client.println("  http.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");");
  // client.println("http.setRequestHeader(\"Content-length\", params.length);");
  // client.println("http.setRequestHeader(\"Connection\", \"close\");");
  client.println("  http.onreadystatechange = function() { //Call a function when the state changes.");
  client.println("    if(http.readyState == 4 && http.status == 200) {");
  client.println("      console.log(http.responseText);"); // TODO: Make this actually "activate" the "ghost" node that has been added... once confirmed in the program!
  client.println("    }");
  client.println("  }");
  client.println("  http.send(params);");
  client.println("};");
  
  
  client.println("</script>");
  

  
  client.println("</head>");

  client.println("<h1>Looper</h1>");
  
  client.println("<h2>Behavior</h2>");
  client.println("TODO: Show program behavior sequence here (just a list).");
  
  client.println("<h2>Beahvior Nodes (i.e., the language)</h2>");
  // client.println("<input type=\"button\" value=\"add node\" onclick=\"javascript:addNode(20, 10, 0, 1, 1);\" /><br />");
  
  client.println("<input type=\"button\" value=\"on 13\" onclick=\"javascript:lightOn();\" /><br />");
  client.println("<input type=\"button\" value=\"off 13\" onclick=\"javascript:lightOff();\" /><br />");
  
  client.println("<input type=\"button\" value=\"delay 1000 ms\" onclick=\"javascript:delay(1000);\" /><br />");
  
  client.println("<input type=\"button\" value=\"erase\" onclick=\"javascript:erase();\" /><br />");

//  client.println("<h2>Pins</h2>");
//  // output the value of each analog input pin
//  for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
//    int sensorReading = analogRead(analogChannel);
//    client.print("analog input ");
//    client.print(analogChannel);
//    client.print(" is ");
//    client.print(sensorReading);
//    client.println("<br />");
//  }

  client.println("</html>");
  
//  client.flush();
}
