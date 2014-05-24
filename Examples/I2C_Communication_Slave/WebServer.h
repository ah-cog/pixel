#ifndef WEB_SERVER_H
#define WEB_SERVER_H

//#include <Adafruit_CC3000.h>
//#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"

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

// Declare function signatures
uint16_t checkFirmwareVersion(void);
bool displayConnectionDetails (void);
boolean handleDefaultHttpRequest(Adafruit_CC3000_ClientRef& client);

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
  
  
//  client.println("function lightOn() {");
//  client.println("  var http = new XMLHttpRequest();");
//  client.println("  var url = \"/lighton\";");
//  client.println("  var params = \"\";");
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
//  
//  
//  client.println("function lightOff() {");
//  client.println("  var http = new XMLHttpRequest();");
//  client.println("  var url = \"/lightoff\";");
//  client.println("  var params = \"\";");
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

#endif
