#ifndef LIGHT_H
#define LIGHT_H

#define RED_LED_PIN 3
#define GREEN_LED_PIN 4
#define BLUE_LED_PIN 5

// Types of lighting:
// - switch (on or off)
// - cut (to specified color, immediately)
// - crossfade (to specified color, from current color)

#define COLOR_APPLICATION_MODE_CUT 0
#define COLOR_APPLICATION_MODE_CROSSFADE 1
int colorApplicationMethod = COLOR_APPLICATION_MODE_CROSSFADE;
int crossfadeStep = 0;

int defaultModuleColor[3] = { 255, 255, 255 }; // The color associated with the module
int targetColor[3] = { 255, 255, 255 }; // The desired color of the LED
int ledColor[3] = { 255, 255, 255 }; // The current actual color of the LED

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
 * Sets the desired color of the module
 */
void setColor(int red, int green, int blue) {
  targetColor[0] = abs(red - 255);
  targetColor[1] = abs(green - 255);
  targetColor[2] = abs(blue - 255);
  
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
    if (crossfadeStep >= red - ledColor[0] && ledColor[0] > red) {
      analogWrite(RED_LED_PIN, ledColor[0] - crossfadeStep);
    }
    if (crossfadeStep >= green - ledColor[1] && ledColor[1] > green) {
      analogWrite(GREEN_LED_PIN, ledColor[1] - crossfadeStep);
    }
    if (crossfadeStep >= blue - ledColor[2] && ledColor[2] > blue) {
      analogWrite(BLUE_LED_PIN, ledColor[2] - crossfadeStep);
    }
    
    //delay(10);
    //}
    //for (int i = 0; i <= 255; i++)
    //{
      
    if (crossfadeStep >= ledColor[0] - red && ledColor[0] < red) {
      analogWrite(RED_LED_PIN, ledColor[0] + crossfadeStep);
    }
    if (crossfadeStep >= ledColor[1] - green && ledColor[1] < green) {
      analogWrite(GREEN_LED_PIN, ledColor[1] + crossfadeStep);
    }
    if (crossfadeStep >= ledColor[2] - blue && ledColor[2] < blue) {
      analogWrite(BLUE_LED_PIN, ledColor[2] + crossfadeStep);
    }
    delay(5); // delay(10);
    
    crossfadeStep++;
    // crossfadeColorStep(red, green, blue);
    
  }
  
  if (crossfadeStep == 255) {
    delay(5); // delay(10);
    analogWrite(RED_LED_PIN, red);
    analogWrite(GREEN_LED_PIN, green);
    analogWrite(BLUE_LED_PIN, blue);
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
  
  targetColor[0] = abs(red - 255);
  targetColor[1] = abs(green - 255);
  targetColor[2] = abs(blue - 255);
  
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
    analogWrite(RED_LED_PIN,   ledColor[0]);
    analogWrite(GREEN_LED_PIN, ledColor[1]);
    analogWrite(BLUE_LED_PIN,  ledColor[2]);
    
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
