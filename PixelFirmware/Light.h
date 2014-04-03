#define RED_LED_PIN 3
#define GREEN_LED_PIN 4
#define BLUE_LED_PIN 5

// Types of lighting:
// - switch (on or off)
// - cut (to specified color, immediately)
// - crossfade (to specified color, from current color)

// TODO:
// - int colorTransitionMode = CUT, CROSSFADE

int moduleColor[3] = { 255, 255, 255 }; // The color associated with the module
int targetColor[3] = { 255, 255, 255 }; // The desired color of the LED
int ledColor[3] = { 255, 255, 255 }; // The current actual color of the LED

void setModuleColor(int red, int green, int blue) {
  moduleColor[0] = abs(red - 255);
  moduleColor[1] = abs(green - 255);
  moduleColor[2] = abs(blue - 255);
}

/**
 * Physically changes the color of the LED by setting the PWM pins.
 */
void applyColor() {
  // Write values to pins
  analogWrite(RED_LED_PIN, ledColor[0]);
  analogWrite(GREEN_LED_PIN, ledColor[1]);
  analogWrite(BLUE_LED_PIN, ledColor[2]);
}

void setColor(int red, int green, int blue) {
  ledColor[0] = abs(red - 255);
  ledColor[1] = abs(green - 255);
  ledColor[2] = abs(blue - 255);
  
  applyColor();
}

void ledOn() {
  ledColor[0] = 0;
  ledColor[1] = 0;
  ledColor[2] = 0;
  applyColor();
}

void ledOff() {
  ledColor[0] = 255;
  ledColor[1] = 255;
  ledColor[2] = 255;
  applyColor();
}

void ledToggle() {
  if (ledColor[0] == 255 && ledColor[1] == 255 && ledColor[2] == 255) {
    ledOff();
  } else {
    ledOn();
  }
}

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

int crossfadeStep = 0;
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

void crossfadeColorStep() {
  crossfadeColorStep(targetColor[0], targetColor[1], targetColor[2]);
}

void crossfadeColorStepComplete(int red, int green, int blue) {
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
    crossfadeColorStep(red, green, blue);
    
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
 * Gradually transition from the current color to the specified color.
 */
void crossfadeColorComplete(int red, int green, int blue) {
  
  targetColor[0] = abs(red - 255);
  targetColor[1] = abs(green - 255);
  targetColor[2] = abs(blue - 255);
  
  crossfadeStep = 0;
  
  crossfadeColorStepComplete(targetColor[0], targetColor[1], targetColor[2]); // TODO: Move this to the main loop, but generalized, so animation happens "simultaneously" with gesture recognition, so it's very responsive to gesture and movement.
}
