#define RED_LED_PIN 3
#define GREEN_LED_PIN 4
#define BLUE_LED_PIN 5

int ledColor[3] = { 255, 255, 255 };

void setColor(int red, int green, int blue) {
  ledColor[0] = abs(red - 255);
  ledColor[1] = abs(green - 255);
  ledColor[2] = abs(blue - 255);
}

void ledOn() {
  analogWrite(RED_LED_PIN, ledColor[0]);
  analogWrite(GREEN_LED_PIN, ledColor[1]);
  analogWrite(BLUE_LED_PIN, ledColor[2]);
}

void ledOff() {
  analogWrite(RED_LED_PIN, 255);
  analogWrite(GREEN_LED_PIN, 255);
  analogWrite(BLUE_LED_PIN, 255);
}

void ledToggle() {
  if (ledColor[0] == 255 && ledColor[1] == 255 && ledColor[2] == 255) {
    ledOff();
  } else {
    ledOn();
  }
}

void fadeOn() {
  while (ledColor[0] > 0 && ledColor[1] > 0 && ledColor[2] > 0) {
    if (ledColor[0] > 0) ledColor[0] -= 20;
    if (ledColor[1] > 0) ledColor[1] -= 20;
    if (ledColor[2] > 0) ledColor[2] -= 20;
    analogWrite(RED_LED_PIN, ledColor[0]);
    analogWrite(GREEN_LED_PIN, ledColor[1]);
    analogWrite(BLUE_LED_PIN, ledColor[2]);
    delay(100);
  }
}

void fadeOff() {
  while (ledColor[0] < 255 && ledColor[1] < 255 && ledColor[2] < 255) {
    if (ledColor[0] < 255) ledColor[0] += 20;
    if (ledColor[1] < 255) ledColor[1] += 20;
    if (ledColor[2] < 255) ledColor[2] += 20;
    analogWrite(RED_LED_PIN, ledColor[0]);
    analogWrite(GREEN_LED_PIN, ledColor[1]);
    analogWrite(BLUE_LED_PIN, ledColor[2]);
    delay(100);
  }
}

/**
 * Gradually transition from the current color to the specified color.
 */
void crossfadeColor(int red, int green, int blue) {
  red = abs(red - 255);
  green = abs(green - 255);
  blue = abs(blue - 255);
  for (int i = 0; i <= 255; i++) {
    if (i >= ledColor[0] - red && ledColor[0] < red) {
      analogWrite(RED_LED_PIN, ledColor[0] + i);
    }
    if (i >= ledColor[1] - green && ledColor[1] < green) {
      analogWrite(GREEN_LED_PIN, ledColor[1] + i);
    }
    if (i >= ledColor[2] - blue && ledColor[2] < blue) {
      analogWrite(BLUE_LED_PIN, ledColor[2] + i);
    }
    //delay(10);
    //}
    //for (int i = 0; i <= 255; i++)
    //{
    if (i >= red - ledColor[0] && ledColor[0] > red) {
      analogWrite(RED_LED_PIN, ledColor[0] - i);
    }
    if (i >= green - ledColor[1] && ledColor[1] > green) {
      analogWrite(GREEN_LED_PIN, ledColor[1] - i);
    }
    if (i >= blue - ledColor[2] && ledColor[2] > blue) {
      analogWrite(BLUE_LED_PIN, ledColor[2] - i);
    }
    delay(10);
  }
  delay(10);
  analogWrite(RED_LED_PIN, red);
  analogWrite(GREEN_LED_PIN, green);
  analogWrite(BLUE_LED_PIN, blue);
  ledColor[0] = red;
  ledColor[1] = green;
  ledColor[2] = blue;
}
