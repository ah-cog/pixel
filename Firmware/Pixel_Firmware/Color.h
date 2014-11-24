#ifndef COLOR_H
#define COLOR_H

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

#define COLORING_MODE_CUT 0
#define COLOR_APPLICATION_MODE_CROSSFADE 1
int colorApplicationMethod = COLORING_MODE_CUT; // COLOR_APPLICATION_MODE_CROSSFADE;
int crossfadeStep = 0;

int defaultModuleColor[3] = { 255, 255, 255 }; // The color associated with the module
int sequenceColor[3] = { 255, 255, 255 }; // The desired color of the LED
//int targetColor[3] = { 255, 255, 255 }; // The desired color of the LED
int targetInputColor[3] = { 255, 255, 255 }; // The desired color of the LED
int targetOutputColor[3] = { 255, 255, 255 }; // The desired color of the LED
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

// TODO: Define these on the "shell" as behaviors to replace hard-coded versions and to make malleable for users!
#define LIGHT_BEHAVIOR_NONE 7
#define LIGHT_BEHAVIOR_OFF 0
#define LIGHT_BEHAVIOR_ON 1
#define LIGHT_BEHAVIOR_DELAY_50MS 2
#define LIGHT_BEHAVIOR_DELAY_100MS 3
#define LIGHT_BEHAVIOR_DELAY_150MS 4
#define LIGHT_BEHAVIOR_DELAY_200MS 5
#define GENERATE_LIGHT_BEHAVIOR_BLINK_100MS 6

int currentLightBehavior = -1;
unsigned long currentLightBehaviorStartTime = 0;
int currentLightBehaviorStep = 0;
unsigned long currentLightBehaviorStepStartTime = 0;

void Update_Color (int red, int green, int blue);
void applyColor (int applicationMethod);

boolean queueLightBehavior(int behavior);
int dequeueLightBehavior ();

void Blink_Light (int count);
void ledOn ();
void ledOff ();

boolean Perform_Light_Behavior () {
  
  // Check for a generative blink behavior on the front of the queue
  if (currentLightBehavior == GENERATE_LIGHT_BEHAVIOR_BLINK_100MS) {
//    Serial.println ("GENERATE_LIGHT_BEHAVIOR_BLINK_100MS");
    
    // Queue blink behavior
    queueLightBehavior (GENERATE_LIGHT_BEHAVIOR_BLINK_100MS);
//    blinkLight (1);
    int count = 1;
    for (int i = 0; i < count; i++) {
      queueLightBehavior(LIGHT_BEHAVIOR_ON); // Turn light on
      queueLightBehavior(LIGHT_BEHAVIOR_DELAY_100MS); // Delay
      queueLightBehavior(LIGHT_BEHAVIOR_OFF); // Turn light off
//      if (i < (count - 1)) {
        queueLightBehavior(LIGHT_BEHAVIOR_DELAY_100MS); // Delay
//      }
    }
    
    // Dequeue light behavior
    if (lightBehaviorQueueSize > 0) {
      currentLightBehavior = dequeueLightBehavior(); // Get the next behavior in the queue
      currentLightBehaviorStep = 0; // Reset behavior step
      currentLightBehaviorStartTime = millis(); // Set start time of behavior to present time
    }
    
  } else if (currentLightBehavior == LIGHT_BEHAVIOR_NONE) {
    
    // Dequeue light behavior
    if (lightBehaviorQueueSize > 0) {
      currentLightBehavior = dequeueLightBehavior(); // Get the next behavior in the queue
      currentLightBehaviorStep = 0; // Reset behavior step
      currentLightBehaviorStartTime = millis(); // Set start time of behavior to present time
    } else {
      currentLightBehavior = -1; // Reset the current behavior to indicate it's done
    }
    
  }
  
  // Process the light behavior:
  
  if (currentLightBehavior != -1) { // If there is currently a light behavior
    // TODO: Do behavior
    
    // Set the start time of the behavior step
    if (currentLightBehaviorStep == 0) {
      currentLightBehaviorStepStartTime = millis();
    }
    
    // Actually do the behavior!
    if (currentLightBehavior == LIGHT_BEHAVIOR_OFF) {
      
      Update_Color (defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
      applyColor(COLORING_MODE_CUT);
      
      currentLightBehavior = -1; // Reset the current behavior to indicate it's done
      
    } else if (currentLightBehavior == LIGHT_BEHAVIOR_ON) {
      
      Update_Color (0, 0, 0);
      applyColor (COLORING_MODE_CUT);
  
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
//  Serial.print ("queueLightBehavior "); Serial.print (behavior); Serial.print ("\n");
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

void Apply_Input_Color (uint32_t c, uint8_t wait);
void Apply_Output_Color (uint32_t c, uint8_t wait);

//! Update the module's input and output color.
//!
void Apply_Color (uint8_t wait) {
//  for (uint16_t i = 0; i < strip.numPixels (); i++) {
//      strip.setPixelColor (i, c);
//      strip.show ();
//      delay (wait);
//  }
  Apply_Input_Color (strip.Color (targetInputColor[0], targetInputColor[1], targetInputColor[2]), wait);
  Apply_Output_Color (strip.Color (targetOutputColor[0], targetOutputColor[1], targetOutputColor[2]), wait);
}

//! Update the module's input and output color.
//!
void Apply_Input_Color (uint32_t c, uint8_t wait) {
//  uint32_t c = strip.Color (red, green, blue);
//  uint8_t wait = COLOR_MIX_DELAY;
  if (strip.numPixels () > 0 && strip.numPixels () <= 2) {
      strip.setPixelColor (0, c);
      strip.show ();
      delay (wait);
  }
}

//! Update the module's input and output color.
//!
void Apply_Output_Color (uint32_t c, uint8_t wait) {
//  uint32_t c = strip.Color (red, green, blue);
//  uint8_t wait = COLOR_MIX_DELAY;
  if (strip.numPixels () > 0 && strip.numPixels () <= 2) {
      strip.setPixelColor (1, c);
      strip.show ();
      delay (wait);
  }
}

//! Update the module's input color.
//!
void Update_Input_Color (int red, int green, int blue) {
  
  // Update input color
  targetInputColor[0] = red;
  targetInputColor[1] = green;
  targetInputColor[2] = blue;
  
  // Reset color applyer
  if (colorApplicationMethod == COLOR_APPLICATION_MODE_CROSSFADE) {
    crossfadeStep = 0;
  }
  
}

//! Update the module's output color.
//!
void Update_Output_Color (int red, int green, int blue) {
  
  // Update output color
  targetOutputColor[0] = red;
  targetOutputColor[1] = green;
  targetOutputColor[2] = blue;
  
  // Reset color applyer
  if (colorApplicationMethod == COLOR_APPLICATION_MODE_CROSSFADE) {
    crossfadeStep = 0;
  }
  
}

//! Initializes the module's color
//!
boolean setupColor () {
  strip.begin ();
  strip.show (); // Initialize all pixels to 'off'
}

//! Sets the module's default, unique color.
//!
void Update_Module_Color (int red, int green, int blue) {
  defaultModuleColor[0] = red;
  defaultModuleColor[1] = green;
  defaultModuleColor[2] = blue;
}

//! Sets the module's sequence color
//!
void setSequenceColor (int red, int green, int blue) {
  sequenceColor[0] = red;
  sequenceColor[1] = green;
  sequenceColor[2] = blue;
}

//! Sets the desired color of the module
//!
void Update_Color (int red, int green, int blue) {
  
  // Update input color
  targetInputColor[0] = red;
  targetInputColor[1] = green;
  targetInputColor[2] = blue;
  
  // Update output color
  targetOutputColor[0] = red;
  targetOutputColor[1] = green;
  targetOutputColor[2] = blue;
  
  // Reset color applyer
  if (colorApplicationMethod == COLOR_APPLICATION_MODE_CROSSFADE) {
    crossfadeStep = 0;
  }
  
}

//! Sets the color of the module to it's default color
//!
void Update_Module_Color () {
  Update_Color (defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
}

//! Performs one step of crossfading the LED's current color to the specified color
//!
void crossfadeColorStep (int red, int green, int blue) {
  //for (int i = 0; i <= 255; i++) {
  if (crossfadeStep <= 255) {
    
    int nextRed = red;
    int nextGreen = green;
    int nextBlue = blue;
    
    if (crossfadeStep >= red - ledColor[0] && ledColor[0] > red) {
      nextRed = ledColor[0] - crossfadeStep;
    }
    if (crossfadeStep >= green - ledColor[1] && ledColor[1] > green) {
      nextGreen = ledColor[1] - crossfadeStep;
    }
    if (crossfadeStep >= blue - ledColor[2] && ledColor[2] > blue) {
      nextBlue = ledColor[2] - crossfadeStep;
    }
    // TODO: Apply_Color (strip.Color (nextRed, nextGreen, nextBlue), COLOR_MIX_DELAY, strip.Color (nextRed, nextGreen, nextBlue), COLOR_MIX_DELAY); // Red
    Apply_Color (COLOR_MIX_DELAY); // Red
    
    //delay(10);
    //}
    //for (int i = 0; i <= 255; i++)
    //{
      
    if (crossfadeStep >= ledColor[0] - red && ledColor[0] < red) {
      nextRed = ledColor[0] + crossfadeStep;
    }
    if (crossfadeStep >= ledColor[1] - green && ledColor[1] < green) {
      nextGreen = ledColor[1] - crossfadeStep;
    }
    if (crossfadeStep >= ledColor[2] - blue && ledColor[2] < blue) {
      nextBlue = ledColor[2] - crossfadeStep;
    }
    Apply_Color (COLOR_MIX_DELAY); // Red
//    delay(5); // delay(10);
    
    crossfadeStep++;
    // crossfadeColorStep(red, green, blue);
    
  }
  
  if (crossfadeStep == 255) {
    Apply_Color (COLOR_MIX_DELAY); // Red
    
    ledColor[0] = red;
    ledColor[1] = green;
    ledColor[2] = blue;
    
    crossfadeStep++;
  }
}

//! Performs one step of crossfading the LED's current color to the current target co
//!
void crossfadeColorStep () {
  //crossfadeColorStep (targetColor[0], targetColor[1], targetColor[2]);
  crossfadeColorStep (targetInputColor[0], targetInputColor[1], targetInputColor[2]);
}

//! Gradually transition from the current color to the specified color.
//!
void crossfadeColor (int red, int green, int blue) {
  
//  targetColor[0] = red; // abs(red - 255);
//  targetColor[1] = green; // abs(green - 255);
//  targetColor[2] = blue; // abs(blue - 255);
  targetInputColor[0] = red; // abs(red - 255);
  targetInputColor[1] = green; // abs(green - 255);
  targetInputColor[2] = blue; // abs(blue - 255);
  
  crossfadeStep = 0;
  
  // crossfadeColorStep(targetColor[0], targetColor[1], targetColor[2]); // TODO: Move this to the main loop, but generalized, so animation happens "simultaneously" with gesture recognition, so it's very responsive to gesture and movement.
}

//! Physically changes the color of the LED by setting the PWM pins.
//!
void applyColor (int applicationMethod) {
  if (applicationMethod == COLORING_MODE_CUT) {
    
    // Update LED color buffer
//    ledColor[0] = targetColor[0];
//    ledColor[1] = targetColor[1];
//    ledColor[2] = targetColor[2];
    ledColor[0] = targetInputColor[0];
    ledColor[1] = targetInputColor[1];
    ledColor[2] = targetInputColor[2];
    
    // Write values to pins
//    Apply_Color (strip.Color(ledColor[0], ledColor[1], ledColor[2]), COLOR_MIX_DELAY); // Red
    Apply_Color (COLOR_MIX_DELAY); // Red
    
  } else if (applicationMethod == COLOR_APPLICATION_MODE_CROSSFADE) {
    if (crossfadeStep < 256) {
      crossfadeColorStep();
    }
  }
}

//! Blinks the lights the specified number of times.
//!
void Blink_Light (int count) {
  for (int i = 0; i < count; i++) {
    queueLightBehavior(LIGHT_BEHAVIOR_ON); queueLightBehavior(LIGHT_BEHAVIOR_DELAY_100MS);
    queueLightBehavior(LIGHT_BEHAVIOR_OFF);
    if (i < (count - 1)) {
      queueLightBehavior(LIGHT_BEHAVIOR_DELAY_100MS);
    }
  }
}

//! Make the light blink until it is manually stopped.
//!
void Start_Blink_Light () {
//  Serial.println ("startBlinkLight");
  queueLightBehavior(GENERATE_LIGHT_BEHAVIOR_BLINK_100MS);
}

//! Make the light stop blinking.
//!
boolean Stop_Blink_Light () {
//  Serial.println ("stopBlinkLight");
  // TODO: 
  
//  lightBehaviorQueue
//  lightBehaviorQueueSize
  
  // Search for the first generate light behavior from the front of the queue
  for (int i = 0; i < lightBehaviorQueueSize; i++) {
    if (lightBehaviorQueue[i] == GENERATE_LIGHT_BEHAVIOR_BLINK_100MS) {
      lightBehaviorQueue[i] = LIGHT_BEHAVIOR_NONE; // Replace the generate behavior with an empty behavior
      return true;
    }
  }
  return false;
}

/**
 * Applies the current target color using the current color application method.
 */
void Light_Apply_Color () {
  applyColor (colorApplicationMethod);
}

void Light_On () {
  queueLightBehavior (LIGHT_BEHAVIOR_ON);
}

void Light_Off () {
  queueLightBehavior (LIGHT_BEHAVIOR_OFF);
}

void Light_Fade_On () {
//  ledOff();
  while (ledColor[0] > 0 && ledColor[1] > 0 && ledColor[2] > 0) {
    if (ledColor[0] > 0) ledColor[0] -= 10;
    if (ledColor[1] > 0) ledColor[1] -= 10;
    if (ledColor[2] > 0) ledColor[2] -= 10;
    Light_Apply_Color ();
    delay(30);
  }
}

void Light_Fade_Off () {
  while (ledColor[0] < 255 && ledColor[1] < 255 && ledColor[2] < 255) {
    if (ledColor[0] < 255) ledColor[0] += 10;
    if (ledColor[1] < 255) ledColor[1] += 10;
    if (ledColor[2] < 255) ledColor[2] += 10;
    Light_Apply_Color ();
    delay(45);
  }
  ledColor[0] = 255;
  ledColor[1] = 255;
  ledColor[2] = 255;
  Light_Apply_Color ();
}

#endif