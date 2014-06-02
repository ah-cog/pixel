#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "BehaviorInterpretter.h"

//#include <Adafruit_CC3000.h>
//#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ  1  // Note: MUST be assigned to an interrupt pin.
#define ADAFRUIT_CC3000_VBAT 3  // Note: Can be assigned to any pin.
#define ADAFRUIT_CC3000_CS   10 // Note: Can be assigned to any pin.
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV4); // you can change this clock speed

#define WLAN_SSID "Hackerspace" // Cannot be longer than 32 characters!
#define WLAN_PASS "MakingIsFun!"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY WLAN_SEC_WPA2

#define LISTEN_PORT 80 // What TCP port to listen on for connections.  The echo protocol uses port 7.

Adafruit_CC3000_Server httpServer(LISTEN_PORT);

/**
 * Data structures
 */

char httpRequestUriBuffer[50]; // HTTP request buffer
int bi = 0; // HTTP request buffer index
char* httpRequestParameters[10]; // HTTP request parameters (i.e., key/value pairs encoded in the URI like "?key1=value1&key2=value2")
int httpRequestParameterCount = 0;
//char* httpRequestParameterDictionary[10][2]; // HTTP request parameters (i.e., key/value pairs encoded in the URI like "?key1=value1&key2=value2")
// httpRequestParameterDictionary[paramIndex][key] // key = 0 (returns char*)
// httpRequestParameterDictionary[paramIndex][value] // value = 1 (returns char*)

/**
 * Function declarations (i.e., function signatures)
 */

// Adafruit CC3000 Breakout Board
boolean setupWebServer();
boolean handleClientConnection(Adafruit_CC3000_ClientRef& client);
uint16_t checkFirmwareVersion(void);
boolean displayConnectionDetails (void);

// HTTP Request Handlers
boolean handleDefaultHttpRequest(Adafruit_CC3000_ClientRef& client);

// Utilities
String getValue(String data, char separator, int index);

/**
 * Function definitions
 */

boolean setupWebServer() {
  
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
}

boolean handleClientConnection(Adafruit_CC3000_ClientRef& client) {
  
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
            
            if (strcmp (httpRequestAddress, "/pin") == 0) {
              
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
              insertBehaviorNode(0, operation, pin, 0, 1, value);
              
              // Send a standard HTTP response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              
              // TODO: flush
              // TODO: close
              
              break;
              
            } else if (strcmp (httpRequestAddress, "/delay") == 0) {
              
              // TODO: Only do this when /add-node is called (or whatever the URI will be)
              insertBehaviorNode(0, 2, 1, 0, 1, 0);
              
              // Send a standard HTTP response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              
              // TODO: flush
              // TODO: close
              
              break;
              
            } else if (strcmp (httpRequestAddress, "/erase") == 0) {
              
              //          // TODO: Only do this when /add-node is called (or whatever the URI will be)
              insertBehaviorNode(0, 3, 1, 0, 1, 0);
              
              // Send a standard HTTP response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              
              // TODO: flush
              // TODO: close
              
              break;
              
            } else {
              
              // TODO: Default, catch-all GET handler that scaffolds further action
              
              // Send a standard 404 HTTP response header
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              client.println();
              
              break;
            } 
            
          } else if (strcmp (httpRequestMethod, "GET") == 0) {
            
            if (strcmp (httpRequestAddress, "/") == 0) {
              
              handleDefaultHttpRequest(client);
              break;
              
            } else if (strcmp (httpRequestAddress, "/pin") == 0) {
              
              Serial.println("PARAMETERS:");
              Serial.println(httpRequestParameters[0]);
//              Serial.println(httpRequestParameters[1]);
//              Serial.println(httpRequestParameters[4]);
              
              // Split parameters by '='
//              String split = String(httpRequestParameters[0]); // "hi this is a split test";
//              String key = getValue(split, '=', 0);
//              String value = getValue(split, '=', 1);

              String pinParameter = String(httpRequestParameters[0]); // "hi this is a split test";
              int pin = getValue(pinParameter, '=', 1).toInt();
              
              client.print("pin = ");
              client.println(deviceReportedModel[pin].value);
              
//              String operationParameter = String(httpRequestParameters[1]); // "hi this is a split test";
//              int operation = getValue(operationParameter, '=', 1).toInt();
//              
//              String valueParameter = String(httpRequestParameters[4]); // "hi this is a split test";
//              int value = getValue(valueParameter, '=', 1).toInt();
              
//              Serial.println("PIN/OPERATION/VALUE:");
//              Serial.println(pin);
//              Serial.println(operation);
//              Serial.println(value);
              
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
//              insertBehaviorNode(0, operation, pin, 0, 1, value);

//              client.println("pin 13 = ");
//              client.println(pin);
              
              // Send a standard HTTP response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              
              // TODO: flush
              // TODO: close
              
              break;
              
            } else {
              
              // TODO: Default, catch-all GET handler that scaffolds further action
              
              // Send a standard 404 HTTP response header
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              client.println();
              
              break;
              
            }
            
          } else { // Unrecognized HTTP request method (possibly an error, such as a malformed request)
              
              // TODO: Default, catch-all GET handler that scaffolds further action
              
              // Send a standard 404 HTTP response header
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              client.println();
              
              break;
          }
        }
        
        // Check if the received character is the newline character (i.e., the end of a line)
        if (c == '\n') {
          
          // Increase the HTTP request line count
          httpRequestLineCount++;
          
          // Check if the first line (i.e., the HTTP header) has been received, and if so, parse the header.
          if (httpRequestLineCount == 1) {
            httpRequestUriBuffer[bi] = NULL; // Terminate string
            
//            Serial.print("REQUEST: ");
//            Serial.println(httpRequestUriBuffer);
//            Serial.println();
            
            /**
             * Parse HTTP request header
             */
            
            // Search for the '?' character in the header
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
              
              // NOTE: The following can be removed. The logic is basically paranoia.
              // if (questionCharIndex < spaceCharIndex) {
              //   httpRequestUriBuffer[questionCharIndex] = NULL; // Terminate the request method string
              // } else {
              //   httpRequestUriBuffer[spaceCharIndex] = NULL; // Terminate the request method string
              // }
              
            } else {
              httpRequestUriBuffer[spaceCharIndex] = NULL; // Terminate the request method string
            }
            Serial.println(httpRequestAddress);
            
            hasReceivedRequestFirstLine = true; // Flag the first line of the HTTP request as received. This line contains the URI, used to decide how to handle the request.

            /**            
             * Extract and decode URI parameters (i.e., after ?, between &)
             */
            
            // char* questionChar = NULL; // Pointer to '?' character in URI
            // int questionCharIndex = 0;
            
            // Search for list of parameters encoded in the URI (i.e., search for the '?' character in the URI, which indicates the start of a list of parameters).
            if (questionChar == NULL) {
              httpRequestParameterString = httpRequestAddress; // Start searching for '?' at the beginning of the URI.
              questionChar = strchr(httpRequestParameterString, '?'); // Search for the '?' character
            }
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
  
//  client.println("function addNode(pin, operation, type, mode, value) {");
//  client.println("  var http = new XMLHttpRequest();");
//  client.println("  var url = \"/addnode\";");
//  client.println("  var params = \"pin=\" + pin + \"&operation=\" + operation + \"&type=\" + type + \"&mode=\" + mode + \"&value=\" + value + \"\";");
//  // client.println("  var params = \"\";");
//  client.println("  http.open(\"POST\", url, true);");
//  // Send the proper header information along with the request
//  client.println("  http.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");");
//  // client.println("http.setRequestHeader(\"Content-length\", params.length);");
//  // client.println("http.setRequestHeader(\"Connection\", \"close\");");
//  client.println("  http.onreadystatechange = function() { //Call a function when the state changes.");
//  client.println("    if(http.readyState == 4 && http.status == 200) {");
//  client.println("      console.log(http.responseText);");
//  client.println("    }");
//  client.println("  }");
//  client.println("  http.send(params);");
//  client.println("};");
  
  client.println("function pin(pin, operation, type, mode, value) {");
  client.println("  var http = new XMLHttpRequest();");
  client.println("  var url = \"/pin\";");
  client.println("  var params = \"pin=\" + pin + \"&operation=\" + operation + \"&type=\" + type + \"&mode=\" + mode + \"&value=\" + value + \"\";");
  client.println("  url = url.concat('?', params);");
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
  
  client.println("function readPin(pin, operation, type, mode, value) {");
  client.println("  var http = new XMLHttpRequest();");
  client.println("  var url = \"/pin\";");
  client.println("  var params = \"pin=\" + pin + \"&operation=\" + operation + \"&type=\" + type + \"&mode=\" + mode + \"&value=\" + value + \"\";");
  client.println("  url = url.concat('?', params);");
  // client.println("  var params = \"\";");
  client.println("  http.open(\"GET\", url, true);");
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
  
  client.println("<input type=\"button\" value=\"on 13\" onclick=\"javascript:pin(13, 1, 0, 1, 1);\" /><br />");
  // client.println("<input type=\"button\" value=\"on 13\" onclick=\"javascript:lightOn();\" /><br />");
  client.println("<input type=\"button\" value=\"off 13\" onclick=\"javascript:pin(13, 1, 0, 1, 0);\" /><br />");
  // client.println("<input type=\"button\" value=\"off 13\" onclick=\"javascript:lightOff();\" /><br />");
  
  client.println("<input type=\"button\" value=\"delay 1000 ms\" onclick=\"javascript:delay(1000);\" /><br />");
  
  client.println("<input type=\"button\" value=\"erase\" onclick=\"javascript:erase();\" /><br />");
  
  client.println("<input type=\"button\" value=\"read 13\" onclick=\"javascript:pin(13, 0, 0, 1, 1);\" /><br />");
  
  client.println("<input type=\"button\" value=\"get 13\" onclick=\"javascript:readPin(13, 0, 0, 1, 1);\" /><br />");

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

//
// Tries to read the IP address and other connection details
//
boolean displayConnectionDetails (void) {
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

#endif
