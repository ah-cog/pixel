#include <Wire.h>

// TODO: Implement list of changes to make to send to the Master (which executes gestural and the behavior code for the module)
//   TODO: Include status: "new", "sending", "sent", "confirmed" (after which, they're deleted)

void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);           // start serial for output
}

void loop()
{
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write("13 w d o h"); // respond with message of 6 bytes as expected by master
  // Wire.write("3 14 digital high"); // Step 3: Ensures that stat of pin 14 is digital and that it is set to high, doing so as needed
  // Wire.write("4 delay 5"); // Step 4: Adds a delay of 5 seconds in the program
  // Wire.write("6 14 digital low"); // Step 6: Same as above, but sets low rather than high.
  
  // pin, operation, type, mode, value
  // "13 write digital output high"
  // "13 w d o h"
}
