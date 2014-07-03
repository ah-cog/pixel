#ifndef LIGHT_H
#define LIGHT_H

#define LED_OUTPUT_PIN 6
#define LED_COUNT 2
//#define RED_LED_PIN 3
//#define GREEN_LED_PIN 4
//#define BLUE_LED_PIN 5

#define COLOR_MIX_DELAY 0

// Types of lighting:
// - switch (on or off)
// - cut (to specified color, immediately)
// - crossfade (to specified color, from current color)

#define COLOR_APPLICATION_MODE_CUT 0
#define COLOR_APPLICATION_MODE_CROSSFADE 1
int colorApplicationMethod = COLOR_APPLICATION_MODE_CUT; // COLOR_APPLICATION_MODE_CROSSFADE;
int crossfadeStep = 0;

int defaultModuleColor[3] = { 255, 255, 255 }; // The color associated with the module
int sequenceColor[3] = { 255, 255, 255 }; // The desired color of the LED
int targetColor[3] = { 255, 255, 255 }; // The desired color of the LED
int ledColor[3] = { 255, 255, 255 }; // The current actual color of the LED

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_OUTPUT_PIN, NEO_GRB + NEO_KHZ800);

// Mesh incoming message queue
#define LIGHT_BEHAVIOR_QUEUE_CAPACITY 20
//unsigned short int meshIncomingMessages[MESH_INCOMING_QUEUE_CAPACITY] = { 0 };
int lightBehaviorQueue[LIGHT_BEHAVIOR_QUEUE_CAPACITY]; // 0 = off, 1 = on, 2 = delay 50 ms, 3 = delay 100 ms, 4 = delay 150 ms, 5 = delay 200 ms
int lightBehaviorQueueSize = 0;

#define LIGHT_BEHAVIOR_OFF 0
#define LIGHT_BEHAVIOR_ON 1
#define LIGHT_BEHAVIOR_DELAY_50MS 2
#define LIGHT_BEHAVIOR_DELAY_100MS 3
#define LIGHT_BEHAVIOR_DELAY_150MS 4
#define LIGHT_BEHAVIOR_DELAY_200MS 5

int currentLightBehavior = -1;
unsigned long currentLightBehaviorStartTime = 0;
int currentLightBehaviorStep = 0;
unsigned long currentLightBehaviorStepStartTime = 0;

void setColor(int red, int green, int blue);
void applyColor(int applicationMethod);

int dequeueLightBehavior();

void blinkLight();
void ledOn();
void ledOff();

boolean doLightBehavior() {
  if (currentLightBehavior != -1) { // If there is currently a light behavior
    // TODO: Do behavior
    
    // Set the start time of the behavior step
    if (currentLightBehaviorStep == 0) {
      currentLightBehaviorStepStartTime = millis();
    }
    
    // Actually do the behavior!
    if (currentLightBehavior == LIGHT_BEHAVIOR_OFF) {
      
      setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
      applyColor(COLOR_APPLICATION_MODE_CUT);
      
      currentLightBehavior = -1; // Reset the current behavior to indicate it's done
      
    } else if (currentLightBehavior == LIGHT_BEHAVIOR_ON) {
      
      setColor(0, 0, 0);
      applyColor(COLOR_APPLICATION_MODE_CUT);
  
      currentLightBehavior = -1; // Reset the current behavior to indicate it's done
      
    } else if (currentLightBehavior == LIGHT_BEHAVIOR_DELAY_50MS) {
      
      unsigned long currentTime = millis();
      if (currentTime > (currentLightBehaviorStepStartTime + 50)) {
        currentLightBehavior = -1; // Reset the current behavior to indicate it's done
      }
      
    } else if (currentLightBehavior == LIGHT_BEHAVIOR_DELAY_100MS) {
      
      unsigned long currentTime = millis();
      if (currentTime > (currentLightBehaviorStepStartTime + 100)) {
        currentLightBehavior = -1; // Reset the current behavior to indicate it's done
      }
      
    } else if (currentLightBehavior == LIGHT_BEHAVIOR_DELAY_150MS) {
      
      unsigned long currentTime = millis();
      if (currentTime > (currentLightBehaviorStepStartTime + 150)) {
        currentLightBehavior = -1; // Reset the current behavior to indicate it's done
      }
      
    } else if (currentLightBehavior == LIGHT_BEHAVIOR_DELAY_200MS) {
      
      unsigned long currentTime = millis();
      if (currentTime > (currentLightBehaviorStepStartTime + 200)) {
        currentLightBehavior = -1; // Reset the current behavior to indicate it's done
      }
      
    }
    
    // Continue to the next step
    currentLightBehaviorStep++;
    
    // TODO: When finish behavior, set currentLightBehavior to -1
    return true;
    
  } else {
    
    // There's no current light behavior, so check if there are any queued up.
    if (lightBehaviorQueueSize > 0) {
      currentLightBehavior = dequeueLightBehavior(); // Get the next behavior in the queue
      currentLightBehaviorStep = 0; // Reset behavior step
      currentLightBehaviorStartTime = millis(); // Set start time of behavior to present time
      return true;
    } else {
      return false;
    }
  }
}

/**
 * Push a message onto the queue of messages to be processed and sent via the mesh network.
 */
boolean queueLightBehavior(int behavior) {
  // TODO: Add message to queue... and use sendMessage to send the messages...
  
  if (lightBehaviorQueueSize < LIGHT_BEHAVIOR_QUEUE_CAPACITY) {
    // Add light behavior to queue
    lightBehaviorQueue[lightBehaviorQueueSize] = behavior;
    lightBehaviorQueueSize++; // Increment the queue size
  }
}

/**
 * Sends the top message on the mesh's message queue.
 */
int dequeueLightBehavior() {
  
  if (lightBehaviorQueueSize > 0) {
    
    // Get the next message from the front of the queue
    int behavior = lightBehaviorQueue[0]; // Get message on front of queue
    lightBehaviorQueueSize--;
    
    // Shift the remaining messages forward one position in the queue
    for (int i = 0; i < LIGHT_BEHAVIOR_QUEUE_CAPACITY - 1; i++) {
      lightBehaviorQueue[i] = lightBehaviorQueue[i + 1];
    }
    lightBehaviorQueue[LIGHT_BEHAVIOR_QUEUE_CAPACITY - 1] = -1; // Set last message to "noop"
    
    return behavior;
  }
  
  return -1;
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

/**
 * Initializes the module's lights.
 */
boolean setupLight() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

/**
 * Sets the module's default, unique color.
 */
void setModuleColor(int red, int green, int blue) {
  defaultModuleColor[0] = red;
  defaultModuleColor[1] = green;
  defaultModuleColor[2] = blue;
}

/**
 * Sets the module's sequence color
 */
void setSequenceColor(int red, int green, int blue) {
  sequenceColor[0] = red;
  sequenceColor[1] = green;
  sequenceColor[2] = blue;
}

/**
 * Sets the desired color of the module
 */
void setColor(int red, int green, int blue) {
  targetColor[0] = red;
  targetColor[1] = green;
  targetColor[2] = blue;
  
  if (colorApplicationMethod == COLOR_APPLICATION_MODE_CROSSFADE) {
    crossfadeStep = 0;
  }
}

/**
 * Sets the color of the module to it's default color
 */
void setModuleColor() {
  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
}

/**
 * Performs one step of crossfading the LED's current color to the specified color
 */
void crossfadeColorStep(int red, int green, int blue) {
  //for (int i = 0; i <= 255; i++) {
  if (crossfadeStep <= 255) {
    
    int newRed = red;
    int newGreen = green;
    int newBlue = blue;
    
    if (crossfadeStep >= red - ledColor[0] && ledColor[0] > red) {
      newRed = ledColor[0] - crossfadeStep;
    }
    if (crossfadeStep >= green - ledColor[1] && ledColor[1] > green) {
      newGreen = ledColor[1] - crossfadeStep;
    }
    if (crossfadeStep >= blue - ledColor[2] && ledColor[2] > blue) {
      newBlue = ledColor[2] - crossfadeStep;
    }
    colorWipe(strip.Color(newRed, newGreen, newBlue), COLOR_MIX_DELAY); // Red
    
    //delay(10);
    //}
    //for (int i = 0; i <= 255; i++)
    //{
      
    if (crossfadeStep >= ledColor[0] - red && ledColor[0] < red) {
      newRed = ledColor[0] + crossfadeStep;
    }
    if (crossfadeStep >= ledColor[1] - green && ledColor[1] < green) {
      newGreen = ledColor[1] - crossfadeStep;
    }
    if (crossfadeStep >= ledColor[2] - blue && ledColor[2] < blue) {
      newBlue = ledColor[2] - crossfadeStep;
    }
    colorWipe(strip.Color(newRed, newGreen, newBlue), COLOR_MIX_DELAY); // Red
//    delay(5); // delay(10);
    
    crossfadeStep++;
    // crossfadeColorStep(red, green, blue);
    
  }
  
  if (crossfadeStep == 255) {
    colorWipe(strip.Color(red, green, blue), COLOR_MIX_DELAY); // Red
    
    ledColor[0] = red;
    ledColor[1] = green;
    ledColor[2] = blue;
    
    crossfadeStep++;
  }
}

/**
 * Performs one step of crossfading the LED's current color to the current target co
 */
void crossfadeColorStep() {
  crossfadeColorStep(targetColor[0], targetColor[1], targetColor[2]);
}

/**
 * Gradually transition from the current color to the specified color.
 */
void crossfadeColor(int red, int green, int blue) {
  
  targetColor[0] = red; // abs(red - 255);
  targetColor[1] = green; // abs(green - 255);
  targetColor[2] = blue; // abs(blue - 255);
  
  crossfadeStep = 0;
  
  // crossfadeColorStep(targetColor[0], targetColor[1], targetColor[2]); // TODO: Move this to the main loop, but generalized, so animation happens "simultaneously" with gesture recognition, so it's very responsive to gesture and movement.
}

/**
 * Physically changes the color of the LED by setting the PWM pins.
 */
void applyColor(int applicationMethod) {
  if (applicationMethod == COLOR_APPLICATION_MODE_CUT) {
    
    // Update LED color buffer
    ledColor[0] = targetColor[0];
    ledColor[1] = targetColor[1];
    ledColor[2] = targetColor[2];
    
    // Write values to pins
    colorWipe(strip.Color(ledColor[0], ledColor[1], ledColor[2]), COLOR_MIX_DELAY); // Red
    
  } else if (applicationMethod == COLOR_APPLICATION_MODE_CROSSFADE) {
    if (crossfadeStep < 256) {
      crossfadeColorStep();
    }
  }
}

/**
 * Blinks the lights the specified number of times.
 */
void blinkLight(int count) {
  for (int i = 0; i < count; i++) {
    queueLightBehavior(LIGHT_BEHAVIOR_ON); queueLightBehavior(LIGHT_BEHAVIOR_DELAY_100MS);
    queueLightBehavior(LIGHT_BEHAVIOR_OFF);
    if (i < (count - 1)) {
      queueLightBehavior(LIGHT_BEHAVIOR_DELAY_100MS);
    }
  }
}

/**
 * Applies the current target color using the current color application method.
 */
void applyColor() {
  applyColor(colorApplicationMethod);
}

void ledOn() {
  queueLightBehavior(LIGHT_BEHAVIOR_ON);
}

void ledOff() {
  queueLightBehavior(LIGHT_BEHAVIOR_OFF);
}


//void ledToggle() {
//  if (ledColor[0] == 255 && ledColor[1] == 255 && ledColor[2] == 255) {
//    ledOff();
//  } else {
//    ledOn();
//  }
//}

void fadeOn() {
//  ledOff();
  while (ledColor[0] > 0 && ledColor[1] > 0 && ledColor[2] > 0) {
    if (ledColor[0] > 0) ledColor[0] -= 10;
    if (ledColor[1] > 0) ledColor[1] -= 10;
    if (ledColor[2] > 0) ledColor[2] -= 10;
    applyColor();
    delay(30);
  }
}

void fadeOff() {
  while (ledColor[0] < 255 && ledColor[1] < 255 && ledColor[2] < 255) {
    if (ledColor[0] < 255) ledColor[0] += 10;
    if (ledColor[1] < 255) ledColor[1] += 10;
    if (ledColor[2] < 255) ledColor[2] += 10;
    applyColor();
    delay(45);
  }
  ledColor[0] = 255;
  ledColor[1] = 255;
  ledColor[2] = 255;
  applyColor();
}

#endif
