import processing.serial.*;
import ddf.minim.*;
import ddf.minim.ugens.*;

Minim       minim;
AudioOutput out;
Oscil       topRowWave;
Oscil       bottomRowWave;

Serial myPort;

int counter = 0;

int windowWidth  = 1024;
int windowHeight =  768;

//boolean sketchFullScreen() {
//  return true;
//}

int moduleCount = 0;

class Module {
  long UUID;
  
  int xPosition;
  int yPosition;
  int buttonSwitchState[];
  
  Module() {
    // Generate UUID for module
    UUID = moduleCount;
    moduleCount = moduleCount + 1;
    
    // Constructor
    xPosition = 512;
    yPosition = 384;
    
    buttonSwitchState = new int[4];
    buttonSwitchState[0] = 0;
    buttonSwitchState[1] = 0;
    buttonSwitchState[2] = 0;
    buttonSwitchState[3] = 0;
  }
  
  void setPosition(int x, int y) {
    xPosition = x;
    yPosition = y;
  }
  
  int getSwitchState(int i) {
    return buttonSwitchState[i];
  }
  
  void setSwitchState(int i, int state) {
    buttonSwitchState[i] = state;
  }
  
  void display() {
    rectMode(CORNERS);
    if (buttonSwitchState[0] != 0) {
      fill(0, 0, 255);
    } else {
      fill(64, 64, 64);
    }
    rect(xPosition - 95, yPosition - 100, xPosition - 5, yPosition - 10);
  
    if (buttonSwitchState[1] != 0) {
      fill(0, 0, 255);
    }  else {
      fill(64, 64, 64);
    }
    rect(xPosition + 15, yPosition - 100, xPosition + 105, yPosition - 10);
  
    if (buttonSwitchState[2] != 0) {
      fill(0, 0, 255);
    } else {
      fill(64, 64, 64);
    }
    rect(xPosition - 95, yPosition + 10, xPosition - 5, yPosition + 100);
  
    if (buttonSwitchState[3] != 0) {
      fill(0, 0, 255);
    } else {
      fill(64, 64, 64);
    }
    rect(xPosition + 15, yPosition + 10, xPosition + 105, yPosition + 100);
  }
}

Module modules[];

void setup () {
  
  modules = new Module[2];
  
  modules[0] = new Module();
  modules[0].setPosition(212, 384);
  modules[1] = new Module();
  modules[1].setPosition(712, 384);
  
  // Set the window size
  size(windowWidth, windowHeight);
//  size(displayWidth, displayHeight);

  // List all the available serial ports
  println(Serial.list());

  // Open whatever port is the one you're using with the Arduino.
//  myPort = new Serial(this, Serial.list()[6], 115200);
//  myPort.bufferUntil('\n'); // Don't generate a serialEv /ent() unless you get a newline character:

  // Set inital background
  background(0);

  //
  // Set up sound synthesizer
  //

  // Set up Minim for sound synthesis
  minim = new Minim(this);

  // Use the getLineOut method of the Minim object to get an AudioOutput object
  out = minim.getLineOut();

  // Create a sine wave Oscil, set to 440 Hz, at 0.5 amplitude
  topRowWave    = new Oscil(440, 0.5f, Waves.SQUARE);
  bottomRowWave = new Oscil(660, 0.5f, Waves.SQUARE);
}

int previousTime = 0;
void draw () {
  // NOTE: The state gets updated in the the serialEvent() function
  
  background(0);

  //
  // Synthesize sounds for column for which the counter is active
  //
  
  int currentTime = millis();
  
  // Stop sound synthesizer after a short time (after it has been activated)
  if (currentTime - previousTime > 200) {
    topRowWave.unpatch(out); // Top row
    bottomRowWave.unpatch(out);
  }

  // Update counter
  if (currentTime - previousTime > 500) {
    previousTime = currentTime;

    // Output left column sounds
    if (counter == 0) {
      if (modules[0].getSwitchState(0) != 0) {
        topRowWave.patch(out);
      }
      if (modules[0].getSwitchState(1) != 0) { 
        bottomRowWave.patch(out);
      }
      counter = 1;
    }
    
    // Output right column sounds
    else {
      if (modules[0].getSwitchState(2) != 0) { 
        topRowWave.patch(out);
      }
     if (modules[0].getSwitchState(3) != 0) {
        bottomRowWave.patch(out);
      }
      counter = 0;
    }
  }

  //
  // Draw the waveforms
  //

  stroke(255, 255, 255);
  for (int i = 0; i < out.bufferSize() - 1; i++) {
    // Output module waveform
    line(i, (windowHeight / 2.0) + out.left.get(i) * 50, i + 1, (windowHeight / 2.0) + out.left.get(i + 1) * 50);
  }

  //
  // Draw module on screen
  //
  
  for (int i = 0; i < modules.length; i++) {
    modules[i].display();
  }
}

void serialEvent(Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');

  if (inString != null) {
    // trim off any whitespace
    inString = trim(inString);
    println(inString);

    // Read data from modules
    int[] inputNumbers = int(split(inString, '\t'));
//    buttonSwitchState[0] = inputNumbers[0];
//    buttonSwitchState[1] = inputNumbers[1];
//    buttonSwitchState[2] = inputNumbers[2];
//    buttonSwitchState[3] = inputNumbers[3];
  }
}

void keyPressed() {
//  if () {
//  }
}

void stop() {
  out.close();
  minim.stop();
}

