#ifndef PORTS_H
#define PORTS_H

#define MODULE_INPUT_PIN A1
#define MODULE_OUTPUT_PIN 5

#define TOUCH_SAMPLES (5000)   // maximum number of separate readings to take
#define TOUCH_TIMEOUT (50) // (100) // (500)    // maximum sample integration time in milliseconds

boolean outputPinRemote = false; // Flag indicating whether the output port is on this module or another module

int lastInputValue = 0;
boolean inputStateChanged = false;

double touchInputMean = 0;

boolean setupPorts() {
  // Set up pin mode for I/O
  pinMode(MODULE_OUTPUT_PIN, OUTPUT); // pinMode(13, OUTPUT);
}

void getInputPort() {
  int t1;
  long tStart;  // starting time
  long datSum;  // reset our accumulated sum of input values to zero
  int sMax;
  int sMin;
  long n;            // count of how many readings so far
  double x, mean, delta, sumsq, m2, variance, stdev;  // to calculate standard deviation

  datSum = 0;
  sumsq = 0; // initialize running squared sum of differences
  n = 0;     // have not made any ADC readings yet
  mean = 0; // start off with running mean at zero
  m2 = 0;
  sMax = 0;
  sMin = 65535;
         
  tStart = millis();
  for (int i = 0; i < TOUCH_SAMPLES && ((millis() - tStart) < TOUCH_TIMEOUT); i++) {
    x = touchRead(MODULE_INPUT_PIN);
    datSum += x;
    if (x > sMax) sMax = x;
    if (x < sMin) sMin = x;
          // from http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
    n++;
    delta = x - mean;
    mean += delta/n;
    m2 += (delta * (x - mean));
  }
  variance = m2 / (n-1);  // (n-1):Sample Variance  (n): Population Variance
  stdev = sqrt(variance);  // Calculate standard deviation

//  Serial.print(mean);
//  Serial.print(", ");  
//  Serial.print(n);
//  Serial.print(", ");  
//  Serial.print(sMax - sMin);
//  Serial.print(", ");  
//  Serial.println(stdev, 3);
  
  touchInputMean = mean;
}

#endif
