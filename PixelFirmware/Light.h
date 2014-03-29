#define RED_LED_PIN 3
#define GREEN_LED_PIN 4
#define BLUE_LED_PIN 5

int ledState = 0;
int ledColor[3] = { 255, 255, 255 };

void setColor(int red, int green, int blue) {
  ledColor[0] = abs(red - 255);
  ledColor[1] = abs(green - 255);
  ledColor[2] = abs(blue - 255);
}

void ledOn() {
  setColor(255, 255, 255);
  analogWrite(RED_LED_PIN, ledColor[0]);
  analogWrite(GREEN_LED_PIN, ledColor[1]);
  analogWrite(BLUE_LED_PIN, ledColor[2]);
}

void ledOff() {
  setColor(0, 0, 0);
  analogWrite(RED_LED_PIN, ledColor[0]);
  analogWrite(GREEN_LED_PIN, ledColor[1]);
  analogWrite(BLUE_LED_PIN, ledColor[2]);
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

void ledToggle() {
  if (ledState == 0) {
    ledOff();
  } else {
    ledOn();
  }
}
