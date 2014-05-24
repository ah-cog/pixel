// Notes for Teensy 3.1 (i.e., heuristics to get the CC3000 module working, based on experience):
// - Set to 48 MHz
// - Set clock to SPI_CLOCK_DIV8 (SPI_CLOCK_DIV2 also worked, but not too reliably)

// Adafruit Breakout Board:
// - Firmware V. : 1.24

#include <Wire.h>

#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "WebServer.h"

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
  
  // Setup Wi-Fi and web server
  setupWebServer();
  
  // Setup I2C communication for device-device communication
  Wire.begin(I2C_DEVICE_ADDRESS); // Join I2C bus with the device's address
  Wire.onReceive(receiveEvent);   // Register event handler to receive data from the master I2C device
  Wire.onRequest(requestEvent);   // Event handler to respond to a request for data from the I2C master device
}

boolean hasRoombaSensorData = false;
uint8_t buf[52];                                        // iRobot sensor data buffer. Packet 6 returns 52 bytes.
char httpRequestUriBuffer[50]; // HTTP request buffer
int bi = 0; // HTTP request buffer index
char* httpRequestParameters[10]; // HTTP request parameters (i.e., key/value pairs encoded in the URI like "?key1=value1&key2=value2")
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
              
            } else if (strcmp (httpRequestAddress, "/pin") == 0) {
              
              Serial.println("PARAMETERS:");
              Serial.println(httpRequestParameters[0]);
              Serial.println(httpRequestParameters[1]);
              Serial.println(httpRequestParameters[4]);
              
              // Split parameters by '='
//              String split = String(httpRequestParameters[0]); // "hi this is a split test";
//              String key = getValue(split, '=', 0);
//              String value = getValue(split, '=', 1);

              String pinParameter = String(httpRequestParameters[0]); // "hi this is a split test";
              int pin = getValue(pinParameter, '=', 1).toInt();
              
              String operationParameter = String(httpRequestParameters[1]); // "hi this is a split test";
              int operation = getValue(operationParameter, '=', 1).toInt();
              
              String valueParameter = String(httpRequestParameters[4]); // "hi this is a split test";
              int value = getValue(valueParameter, '=', 1).toInt();
              
              Serial.println("PIN/OPERATION/VALUE:");
              Serial.println(pin);
              Serial.println(operation);
              Serial.println(value);
              
//              // TODO: Parse parameters from HTTP request
//              int pin = String(httpRequestParameters[0]).toInt();
//              int value = String(httpRequestParameters[4]).toInt();
//              
//              Serial.print("pin = ");
//              Serial.print(pin);
//              Serial.print("\n");
//              
//              Serial.print("value = ");
//              Serial.print(value);
//              Serial.print("\n");
              
              //          // TODO: Only do this when /add-node is called (or whatever the URI will be)
              insertBehaviorNode(0, pin, operation, 0, 1, value);
              
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
             
            char* questionChar = NULL; // Pointer to '?' character in URI
            int questionCharIndex = 0;
            
            // Get HTTP request method (i.e., GET or POST)
            httpRequestMethod = &httpRequestUriBuffer[0];
            spaceChar = strchr(httpRequestMethod, ' '); // Get position of ' '
            spaceCharIndex = (int) (spaceChar - httpRequestUriBuffer);
            httpRequestUriBuffer[spaceCharIndex] = NULL; // Terminate the request method string
            Serial.println(httpRequestMethod);
            
            // Get HTTP request address
            httpRequestAddress = &httpRequestUriBuffer[spaceCharIndex + 1];
            spaceChar = strchr(httpRequestAddress, ' ');
            spaceCharIndex = (int) (spaceChar - httpRequestUriBuffer);
            questionChar = strchr(httpRequestAddress, '?'); // Get position of '?' (if any)
            questionCharIndex = (int) (questionChar - httpRequestUriBuffer);
            if (questionChar != NULL) {
              httpRequestUriBuffer[questionCharIndex] = NULL; // Terminate the request method string
//              if (questionCharIndex < spaceCharIndex) {
//                httpRequestUriBuffer[questionCharIndex] = NULL; // Terminate the request method string
//              } else {
//                httpRequestUriBuffer[spaceCharIndex] = NULL; // Terminate the request method string
//              }
            } else {
              httpRequestUriBuffer[spaceCharIndex] = NULL; // Terminate the request method string
            }
            //httpRequestUriBuffer[spaceCharIndex] = NULL; // Terminate the request method string
            Serial.println(httpRequestAddress);
            
            hasReceivedRequestFirstLine = true; // Flag the first line of the HTTP request as received. This line contains the URI, used to decide how to handle the request.

            /**            
             * Extract and decode URI parameters (i.e., after ?, between &)
             */
            
//            char* questionChar = NULL; // Pointer to '?' character in URI
//            int questionCharIndex = 0;
            
            // httpRequestParameterString = &httpRequestUriBuffer[spaceCharIndex + 1];
            if (questionChar != NULL) {
              // httpRequestParameterString = &httpRequestUriBuffer[questionCharIndex + 1];
              // questionChar = strchr(httpRequestParameterString, '?'); // Search for the '?' character
            } else {
              httpRequestParameterString = httpRequestAddress; // Start searching for '?' at the beginning of the URI.
              questionChar = strchr(httpRequestParameterString, '?'); // Search for the '?' character
            }
//            questionChar = strchr(httpRequestParameterString, '?'); // Search for the '?' character
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

/**
 * function that executes whenever data is received from master
 * this function is registered as an event, see setup()
 */
void receiveEvent (int howMany) {
//  while (1 < Wire.available ()) { // loop through all but the last
//    char c = Wire.read (); // receive byte as a character
//    Serial.print (c); // print the character
//  }
//  int x = Wire.read ();    // receive byte as an integer
//  Serial.println (x);         // print the integer

  while (Wire.available () > 0) { // loop through all but the last
    char c = Wire.read (); // receive byte as a character
    Serial.print (c); // print the character
  }
  Serial.println();
  
  // TODO: Parse the message
  // TODO: Handle initial processing for message
  // TODO: Add to incoming I2C message queue (to process)
  // TODO: (elsewhere) Process messages one by one, in order
}

/**
 * function that executes whenever data is requested by master
 * this function is registered as an event, see setup()
 */
void requestEvent () {
//  Wire.write ("13 w d o h "); // respond with message of 6 bytes as expected by master

  Serial.print("Count: ");
  Serial.println(behaviorNodeCount);
  
  char buf[4]; // "-2147483648\0"
//  Wire.write (itoa(behaviorNodeCount, buf, 10));
  
  if (behaviorNodeCount > 0) {
//    Wire.write (behaviorNodes[0].pin); Wire.write (" ");
//    Wire.write (behaviorNodes[0].operation); Wire.write (" ");
//    Wire.write (behaviorNodes[0].type); Wire.write (" ");
//    Wire.write (behaviorNodes[0].mode); Wire.write (" ");
//    Wire.write (behaviorNodes[0].value); Wire.write (" ");

    // Send serialized behavior
    Wire.write (itoa(behaviorNodes[0].pin, buf, 10));       Wire.write (" ");
    Wire.write (itoa(behaviorNodes[0].operation, buf, 10)); Wire.write (" ");
    Wire.write (itoa(behaviorNodes[0].type, buf, 10));      Wire.write (" ");
    Wire.write (itoa(behaviorNodes[0].mode, buf, 10));      Wire.write (" ");
    Wire.write (itoa(behaviorNodes[0].value, buf, 10));     Wire.write (" ");
    
    // Remove the behavior from the processing queue once it's been sent over I2C
    removeBehaviorNode(0);
  }
  
  // Wire.write("3 14 digital high"); // Step 3: Ensures that stat of pin 14 is digital and that it is set to high, doing so as needed
  // Wire.write("4 delay 5"); // Step 4: Adds a delay of 5 seconds in the program
  // Wire.write("6 14 digital low"); // Step 6: Same as above, but sets low rather than high.
  
  // pin, operation, type, mode, value
  // "13 write digital output high"
  // "13 w d o h"
}

String getValue(String data, char separator, int index)
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
