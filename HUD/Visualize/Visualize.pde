// Graphing sketch


// This program takes ASCII-encoded strings
// from the serial port at 9600 baud and graphs them. It expects values in the
// range 0 to 1023, followed by a newline, or newline and carriage return

// Created 20 Apr 2005
// Updated 18 Jan 2008
// by Tom Igoe
// This example code is in the public domain.

import processing.serial.*;
import ddf.minim.*;
import ddf.minim.ugens.*;

Minim       minim;
AudioOutput out;
Oscil       wave1;
Oscil       wave2;
Oscil       wave3;
Oscil       wave4;

Serial myPort;        // The serial port
int xPos = 1;         // horizontal position of the graph

int PC = 0;

void setup () {
  // set the window size:
  size(1024, 760);        

  // List all the available serial ports
  println(Serial.list());
  // I know that the first port in the serial list on my mac
  // is always my  Arduino, so I open Serial.list()[0].
  // Open whatever port is the one you're using.
  myPort = new Serial(this, Serial.list()[6], 115200);
  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');
  // set inital background:
  background(0);
  
  
  minim = new Minim(this);
  
  // use the getLineOut method of the Minim object to get an AudioOutput object
  out = minim.getLineOut();
  
  // create a sine wave Oscil, set to 440 Hz, at 0.5 amplitude
  wave1 = new Oscil( 240, 0.5f, Waves.SQUARE );
  wave2 = new Oscil( 440, 0.5f, Waves.SQUARE );
  wave3 = new Oscil( 640, 0.5f, Waves.SQUARE );
  wave4 = new Oscil( 840, 0.5f, Waves.SQUARE );
  // patch the Oscil to the output
//  wave4.patch( out );
}
int epoch = 0;
void draw () {
  // everything happens in the serialEvent()
  background(0);
  
   int currentTime = millis();
   if (currentTime - epoch > 200) {
    wave2.unpatch( out );
    wave3.unpatch( out ); 
   }
  if (currentTime - epoch > 500) {
    epoch = currentTime;
    wave2.unpatch( out );
    wave3.unpatch( out );
    if (PC == 0) {
      if (buttonSwitchState[0] != 0) {
        wave2.patch( out );
      }
      if (buttonSwitchState[1] != 0) {
        wave3.patch( out );
      }
      PC = 1;
    } else {
      if (buttonSwitchState[2] != 0) {
        wave2.patch( out );
      }
      if (buttonSwitchState[3] != 0) {
        wave3.patch( out );
      }
      PC = 0;
    }
  }
  
  //
  // Draw module
  //
  
  rectMode(CORNERS);
  if (buttonSwitchState[0] != 0) {
    fill(0, 0, 255);
  } else {
    fill(64, 64, 64);
  }
  rect(417, 239, 507, 329);
  
  if (buttonSwitchState[1] != 0) {
    fill(0, 0, 255);
  } else {
    fill(64, 64, 64);
  }
  rect(527, 239, 617, 329);
  
  if (buttonSwitchState[2] != 0) {
    fill(0, 0, 255);
  } else {
    fill(64, 64, 64);
  }
  rect(417, 339, 507, 429);
  
  if (buttonSwitchState[3] != 0) {
    fill(0, 0, 255);
  } else {
    fill(64, 64, 64);
  }
  rect(527, 339, 617, 429);
  
  
  
   // draw the waveforms
  stroke(255, 255, 255);
  for(int i = 0; i < out.bufferSize() - 1; i++)
  {
    line( i, ((768.0 / 2.0) - 50) + out.left.get(i)*50, i+1, ((768.0 / 2.0) - 50) + out.left.get(i+1)*50 );
//    line( i, ((768.0 / 3.0) - 50) + out.left.get(i)*50, i+1, ((768.0 / 3.0) - 50) + out.left.get(i+1)*50 );
//    line( i, ((768.0 / 2.0) - 50) + out.left.get(i)*50, i+1, ((768.0 / 2.0) - 50) + out.left.get(i+1)*50 );
//    line( i, 150 + out.right.get(i)*50, i+1, 150 + out.right.get(i+1)*50 );
  }
}

int buttonSwitchState[] = { 0, 0, 0, 0 };
int buttonSoundState[] = { 0, 0, 0, 0 };

void serialEvent (Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');

  if (inString != null) {
    // trim off any whitespace:
    inString = trim(inString);
    println(inString);
    
    int[] nums = int(split(inString, '\t'));
    buttonSwitchState[0] = nums[0];
    buttonSwitchState[1] = nums[1];
    buttonSwitchState[2] = nums[2];
    buttonSwitchState[3] = nums[3];
    
    
//    // convert to an int and map to the screen height:
//    int inByte = Integer.parseInt(inString); 
////    float inByte = float(inString); 
////    inByte = map(inByte, 0, 1023, 0, height);
//
//    // draw the line:
//    stroke(127, 34, 255);
//    line(xPos, height, xPos, height - inByte);

    // at the edge of the screen, go back to the beginning:
    if (xPos >= width) {
      xPos = 0;
      background(0);
    } 
    else {
      // increment the horizontal position:
      xPos++;
    }
  }
}

