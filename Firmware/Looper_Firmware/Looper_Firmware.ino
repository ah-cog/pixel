/*
"Pixel" Firmware (for the Visual Language, i.e., Looper), Rendition 2
Authors: Michael Gubbels
*/

// Notes for Teensy 3.1 (i.e., heuristics to get the CC3000 module working, based on experience):
// - Set "Board" to Teensy 3.1
// - Set "CPU Speed" to 24 MHz or 48 MHz (96 MHz doesn't seem to work)
// - Set clock to SPI_CLOCK_DIV8 (SPI_CLOCK_DIV2 also worked, but not too reliably)
//
// Notes about Adafruit CC3000 Breakout Board:
// - Firmware V. 1.24

#include <Wire.h>
#include <Adafruit_CC3000.h>
#include <SPI.h>

#define DEVICE_SERIAL Serial3

#include "WebServer.h"
#include <UDPServer.h>

#define UDP_READ_BUFFER_SIZE 36
#define LISTEN_PORT_UDP 4445
UDPServer udpServer(LISTEN_PORT_UDP);

boolean Setup_Udp_Server () {
  udpServer.begin();
  
  return true;
}

// TODO: Implement list of changes to make to send to the Master (which executes gestural and the behavior code for the module)
// - TODO: Include status: "new", "sending", "sent", "confirmed" (after which, they're deleted)

// TODO: Implement web server and request handlers that, in response, update the queue of
//       changes to (1) make to Looper, and (2) to queue for sending to the other device 
//       over I2C upon request.

//#define DEVICE_SERIAL Serial3

boolean Setup_Serial_Bridge () {
  DEVICE_SERIAL.begin (115200);
}

void setup () {
  
  Setup_Serial_Bridge ();

  Serial.begin (115200); // Start serial for output
  Serial.println (F ("Looper Firmware"));
  
  // Setup Wi-Fi, HTTP server, and UDP server
  Setup_WiFi_Communication ();
  Setup_Http_Server ();
  Setup_Udp_Server ();
}

boolean hasTouch = false;

/**
 * The event loop (i.e., this function is called repeatedly when the board is on)
 */
void loop () {
  
  if (hasTouch == false) {
    if (udpServer.available()) {
  
        char buffer[UDP_READ_BUFFER_SIZE] = { 0 };
        int n = udpServer.readData(buffer, UDP_READ_BUFFER_SIZE);  // n contains # of bytes read into buffer
        buffer[n] = '\0';
        
        char* hostIpAddress; // Remote host address (i.e., Pixel VPE)
  
  //      Serial.print("n: "); Serial.println(n);
  //      for (int i = 0; i < n; ++i) {
  //         uint8_t c = buffer[i];
  ////         Serial.print("c: "); Serial.println(c);
  //         // ... Do whatever you want with 'c' here ...
  //      }
  
        // TODO: Make sure the packet starts with "Pixel" (or whatever the packet signature is, so only parse packets with valid addresses)
        if (buffer[0] != 'P') {
          return;
        }
  
        hostIpAddress = &buffer[7];
        Serial.println (hostIpAddress);
        
        // Send IP Address to send of UDP packet
        String moduleIp = Get_IP_Address (ipAddress); // Get this module's IP address to send to remote host
     
     
        // 192.168.0.1
        // ^   ^   ^ ^
        char* ipPart1 = hostIpAddress;
        char* ipPart2 = strchr (ipPart1, '.'); ipPart2[0] = '\0'; ++ipPart2;
        char* ipPart3 = strchr (ipPart2, '.'); ipPart3[0] = '\0'; ++ipPart3;
        char* ipPart4 = strchr (ipPart3, '.'); ipPart4[0] = '\0'; ++ipPart4;
        char* portStr = strchr (ipPart4, ':'); portStr[0] = '\0'; ++portStr;
        
        int ip1  = atoi(ipPart1); // convert IP octets and port to integer data type
        int ip2  = atoi(ipPart2);
        int ip3  = atoi(ipPart3);
        int ip4  = atoi(ipPart4);
        int port = atoi(portStr);
        
        --ipPart2; ipPart2[0] = '.'; // restore the '.' and ':' in the address for later use
        --ipPart3; ipPart3[0] = '.';
        --ipPart4; ipPart4[0] = '.';
        --portStr; portStr[0] = ':';
        
  //      Serial.println (ip1);
  //      Serial.println (ip2);
  //      Serial.println (ip3);
  //      Serial.println (ip4);
  //      Serial.println (port);
        
        
        
        // Send HTTP GET request to Pixel VPE
        
        unsigned long startTime, t;
        unsigned long ip = cc3000.IP2U32 (ip1, ip2, ip3, ip4);
        
        client = cc3000.connectTCP (ip, port);
        
        // Connect to numeric IP
        Serial.print (F("OK\r\nConnecting to server..."));
        t = millis();
        do {
          client = cc3000.connectTCP(ip, port);
        } while ((!client.connected ()) && ((millis () - t) < connectTimeout));
        
        if (client.connected()) { // Success!
        
          Serial.print(F("OK\r\nIssuing HTTP request..."));
          
          // Set up endpoint
          char endpoint[48];
          String endpointStr = String("/modules?ip=") + moduleIp;
          endpointStr.toCharArray (endpoint, 48);
          
          Serial.println (endpoint);
          
          char agent[] = "Pixel/1.0";
      
          // Unlike the hash prep, parameters in the HTTP request don't require sorting.
          client.fastrprint(F("GET ")); client.fastrprint(endpoint); client.fastrprint(F(" HTTP/1.1\r\n"));
          client.fastrprint(F("Host: ")); client.fastrprint(hostIpAddress);
          client.fastrprint(F("\r\nUser-Agent: ")); client.fastrprint(F(agent));
  //        client.fastrprint(F("\r\nConnection: keep-alive\r\n"));
          client.fastrprint(F("\r\nConnection: close\r\n"));
      
          Serial.print(F("OK\r\nAwaiting response..."));
  //        int c = 0;
          // Dirty trick: instead of parsing results, just look for opening
          // curly brace indicating the start of a successful JSON response.
  //        while(((c = timedRead()) > 0) && (c != '{'));
  //        if(c == '{')   Serial.println(F("success!"));
  //        else if(c < 0) Serial.println(F("timeout"));
  //        else           Serial.println(F("error (invalid Twitter credentials?)"));
          client.close();
  //        return (c == '{');
  
          hasTouch = true; // Has successfully connected to Looper mobile interface
          
        } else { // Couldn't contact server
          Serial.println(F("failed"));
  //        return false;
      }
        
        
    }
  }
   
   
   
    
  // Try to get a client which is connected.
  Adafruit_CC3000_ClientRef client = tcpServer.available ();
  
  if (client) {
    Handle_Client_Connection (client);
  }
  
  // Propagate data to the main device
//  if (propagator != NULL) {
////    Serial.println ((int) (*propagator).transformation);
//    if ((*propagator).transformation != NULL) {
//      Propagate (propagator, SERIAL_CHANNEL);
//  //    Delete_Propagator (propagator);
//    }
//  }
  
//  // Create behavior substrate
//  substrate = Create_Substrate ();
//  
//  // Create sequence (i.e., a "(behavior) transformation context"... this is a transformation context for behaviors like a pottery wheel is a context for transforming clay in a particular way) for behaviors
//  // In this analogy, transforming behaviors in a unconstrained graph would be akin to transforming clay with only one's hands (and possibly, but not likely, some basic clay knives... these are likely other things in the analogous situation).
//  Sequence* sequence = Create_Sequence (substrate);
//  boolean isAdded = Update_Sequence_Substrate (sequence, substrate);
  
//  // Create behaviors in the sequence:
//  Sequence* sequence = (*substrate).sequences;
//  int sequenceType = Get_Sequence_Type (sequence);
//  Serial.print ("sequenceType: "); Serial.print (sequenceType); Serial.print ("\n");
//  
//  // Create Input Behavior
//  Behavior* inputBehavior = Create_Input_Behavior (substrate, 5, "digital", "off");
//  Update_Behavior_Sequence (inputBehavior, sequence);
//  
//  // Create Output Behavior
//  Behavior* outputBehavior = Create_Output_Behavior (substrate, 5, "digital", "on");
//  Update_Behavior_Sequence (outputBehavior, sequence);
//  
//  // Create Delay Behavior
//  Behavior* delayBehavior = Create_Delay_Behavior (substrate, 1000);
//  Update_Behavior_Sequence (delayBehavior, sequence);
  
//  // Create performer
//  Performer* performer = Create_Performer (substrate);
  
  // Perform the next behavior
//  while (true) {
//    boolean performanceResult = Perform_Behavior (performer);
//    delay (800);
//  }

//  Propagator* propagator = Create_Propagator ();
//  Propagation* transformation = Create_Propagation ("create substrate 55ff68064989"); // 55ff68064989495329092587
////  String data = Get_Propagation_Data (transformation);
////  Serial.println (data);
//  Queue_Propagation (propagator, transformation);
//  Propagate (propagator, I2C_CHANNEL);
//  Delete_Propagator (propagator);
//  
//  delay (2000);
  
//  while (true);
}
