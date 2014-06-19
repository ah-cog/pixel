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

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
//uint32_t Wheel(byte WheelPos) {
//  if(WheelPos < 85) {
//   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//  } else if(WheelPos < 170) {
//   WheelPos -= 85;
//   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//  } else {
//   WheelPos -= 170;
//   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//  }
//}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

//void rainbow(uint8_t wait) {
//  uint16_t i, j;
//
//  for(j=0; j<256; j++) {
//    for(i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel((i+j) & 255));
//    }
//    strip.show();
//    delay(wait);
//  }
//}

// Slightly different, this makes the rainbow equally distributed throughout
//void rainbowCycle(uint8_t wait) {
//  uint16_t i, j;
//
//  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
//    for(i=0; i< strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
//    }
//    strip.show();
//    delay(wait);
//  }
//}



boolean setupLight() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

/**
 * Sets the module's default, unique color
 */
void setModuleColor(int red, int green, int blue) {
  defaultModuleColor[0] = red;
  defaultModuleColor[1] = green;
  defaultModuleColor[2] = blue;
//  defaultModuleColor[0] = abs(red - 255);
//  defaultModuleColor[1] = abs(green - 255);
//  defaultModuleColor[2] = abs(blue - 255);
}

/**
 * Sets the module's sequence color
 */
void setSequenceColor(int red, int green, int blue) {
  sequenceColor[0] = red;
  sequenceColor[1] = green;
  sequenceColor[2] = blue;
//  sequenceColor[0] = abs(red - 255);
//  sequenceColor[1] = abs(green - 255);
//  sequenceColor[2] = abs(blue - 255);
}

/**
 * Sets the desired color of the module
 */
void setColor(int red, int green, int blue) {
//  targetColor[0] = abs(red - 255);
//  targetColor[1] = abs(green - 255);
//  targetColor[2] = abs(blue - 255);
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
      //analogWrite(RED_LED_PIN, ledColor[0] - crossfadeStep);
//      colorWipe(strip.Color(ledColor[0] - crossfadeStep, ledColor[1], ledColor[2]), COLOR_MIX_DELAY); // Red
      newRed = ledColor[0] - crossfadeStep;
    }
    if (crossfadeStep >= green - ledColor[1] && ledColor[1] > green) {
//      analogWrite(GREEN_LED_PIN, ledColor[1] - crossfadeStep);
//        colorWipe(strip.Color(ledColor[0], ledColor[1] - crossfadeStep, ledColor[2]), COLOR_MIX_DELAY); // Red
      newGreen = ledColor[1] - crossfadeStep;
    }
    if (crossfadeStep >= blue - ledColor[2] && ledColor[2] > blue) {
//      analogWrite(BLUE_LED_PIN, ledColor[2] - crossfadeStep);
//       colorWipe(strip.Color(ledColor[0], ledColor[1], ledColor[2] - crossfadeStep), COLOR_MIX_DELAY); // Red
      newBlue = ledColor[2] - crossfadeStep;
    }
    colorWipe(strip.Color(newRed, newGreen, newBlue), COLOR_MIX_DELAY); // Red
    
    //delay(10);
    //}
    //for (int i = 0; i <= 255; i++)
    //{
      
    if (crossfadeStep >= ledColor[0] - red && ledColor[0] < red) {
//      analogWrite(RED_LED_PIN, ledColor[0] + crossfadeStep);
//      colorWipe(strip.Color(ledColor[0] + crossfadeStep, ledColor[1], ledColor[2]), COLOR_MIX_DELAY); // Red
      newRed = ledColor[0] + crossfadeStep;
    }
    if (crossfadeStep >= ledColor[1] - green && ledColor[1] < green) {
//      analogWrite(GREEN_LED_PIN, ledColor[1] + crossfadeStep);
//      colorWipe(strip.Color(ledColor[0], ledColor[1] + crossfadeStep, ledColor[2]), COLOR_MIX_DELAY); // Red
      newGreen = ledColor[1] - crossfadeStep;
    }
    if (crossfadeStep >= ledColor[2] - blue && ledColor[2] < blue) {
//      analogWrite(BLUE_LED_PIN, ledColor[2] + crossfadeStep);
//      colorWipe(strip.Color(ledColor[0], ledColor[1], ledColor[2] + crossfadeStep), COLOR_MIX_DELAY); // Red
      newBlue = ledColor[2] - crossfadeStep;
    }
    colorWipe(strip.Color(newRed, newGreen, newBlue), COLOR_MIX_DELAY); // Red
//    delay(5); // delay(10);
    
    crossfadeStep++;
    // crossfadeColorStep(red, green, blue);
    
  }
  
  if (crossfadeStep == 255) {
//    delay(5); // delay(10);
//    analogWrite(RED_LED_PIN, red);
//    analogWrite(GREEN_LED_PIN, green);
//    analogWrite(BLUE_LED_PIN, blue);
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
//    analogWrite(RED_LED_PIN,   ledColor[0]);
//    analogWrite(GREEN_LED_PIN, ledColor[1]);
//    analogWrite(BLUE_LED_PIN,  ledColor[2]);
    colorWipe(strip.Color(ledColor[0], ledColor[1], ledColor[2]), COLOR_MIX_DELAY); // Red
    
  } else if (applicationMethod == COLOR_APPLICATION_MODE_CROSSFADE) {
    if (crossfadeStep < 256) {
      crossfadeColorStep();
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
//  ledColor[0] = 0;
//  ledColor[1] = 0;
//  ledColor[2] = 0;
  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
  applyColor(COLOR_APPLICATION_MODE_CUT);
}

void ledOff() {
  setColor(0, 0, 0);
  applyColor(COLOR_APPLICATION_MODE_CUT);
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

void fadeLow() {
  int threshold = 200;
  while (ledColor[0] < threshold && ledColor[1] < threshold && ledColor[2] < threshold) {
    if (ledColor[0] < 255) ledColor[0] += 10;
    if (ledColor[1] < 255) ledColor[1] += 10;
    if (ledColor[2] < 255) ledColor[2] += 10;
    applyColor();
    delay(45);
  }
  ledColor[0] = threshold;
  ledColor[1] = threshold;
  ledColor[2] = threshold;
  applyColor();
}

#endif
