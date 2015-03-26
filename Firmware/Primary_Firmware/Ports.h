#ifndef PORTS_H
#define PORTS_H

#define MODULE_INPUT_PIN A9 // A1
#define MODULE_OUTPUT_PIN 5

#define TOUCH_SAMPLES (5000)   // maximum number of separate readings to take
#define TOUCH_TIMEOUT (50) // (100) // (500)    // maximum sample integration time in milliseconds

//boolean outputPinRemote = false; // Flag indicating whether the output port is on this module or another module

int lastInputValue = 0;
boolean inputStateChanged = false;

double touchInputMean = 0;

boolean Setup_Input_Port ();

boolean Setup_Ports () {
  
  // Set up pin mode for I/O
  pinMode (MODULE_OUTPUT_PIN, OUTPUT); // pinMode(13, OUTPUT);
  
  Setup_Input_Port ();
}

#define SAMPLE_CAPACITY 100
int sampleCount = 0;
int sampleIterator = 0;
double samples[SAMPLE_CAPACITY];

int t1;
long tStart;  // starting time
long datSum;  // reset our accumulated sum of input values to zero
int sMax;
int sMin;
long n;            // count of how many readings so far
double x, mean, delta, sumsq, m2, variance, stdev;  // to calculate standard deviation

boolean Setup_Input_Port () {
  
  // Initialize variables for computing touch input
  datSum = 0;
  sumsq = 0; // initialize running squared sum of differences
  n = 0;     // have not made any ADC readings yet
  mean = 0; // start off with running mean at zero
  m2 = 0;
  sMax = 0;
  sMin = 65535;
  
  // Initialize samples
  for (int i = 0; i < SAMPLE_CAPACITY; i++) {
    samples[i] = 0;
  }
  
  return true; // Setup was successful
  
}

// TODO: Update the parameter to be an actual Channel structure
double Get_Capacitive_Input_Continuous (int inputPin) {
  
  // Store current value on the channel's timeline
  lastInputValue = touchInputMean; // Store the previous input value in history
  
  // Update the channel wit hthe current value
  tStart = millis();
//  for (int i = 0; i < TOUCH_SAMPLES && ((millis() - tStart) < TOUCH_TIMEOUT); i++) {
    x = touchRead (inputPin); // Read touch value on pin
    
    // Calculate the sum of the samples data
//    datSum += x;
    datSum -= samples[sampleIterator]; // Subtract the value to be replaced (the least recent value)
    samples[sampleIterator] = x; // Replace the least recent value with the most recent value
    datSum += samples[sampleIterator]; // Add the most recent value to the total sum of the samples (for the following computations)
    sampleIterator = (sampleIterator + 1) % SAMPLE_CAPACITY; // Advance the iterator (tracks the least recent value in the circular queue)
    
    if (x > sMax) sMax = x;
    if (x < sMin) sMin = x;
          // from http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
    n = SAMPLE_CAPACITY; // n++;
    delta = x - mean;
    mean += delta / n;
    m2 += (delta * (x - mean));
//  }
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
  
  return touchInputMean;
}

//void Get_Input_Port () {
//  
//  int t1;
//  long tStart;  // starting time
//  long datSum;  // reset our accumulated sum of input values to zero
//  int sMax;
//  int sMin;
//  long n;            // count of how many readings so far
//  double x, mean, delta, sumsq, m2, variance, stdev;  // to calculate standard deviation
//
//  datSum = 0;
//  sumsq = 0; // initialize running squared sum of differences
//  n = 0;     // have not made any ADC readings yet
//  mean = 0; // start off with running mean at zero
//  m2 = 0;
//  sMax = 0;
//  sMin = 65535;
//         
//  tStart = millis();
//  for (int i = 0; i < TOUCH_SAMPLES && ((millis() - tStart) < TOUCH_TIMEOUT); i++) {
//    x = touchRead(MODULE_INPUT_PIN);
//    datSum += x;
//    if (x > sMax) sMax = x;
//    if (x < sMin) sMin = x;
//          // from http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
//    n++;
//    delta = x - mean;
//    mean += delta/n;
//    m2 += (delta * (x - mean));
//  }
//  variance = m2 / (n-1);  // (n-1):Sample Variance  (n): Population Variance
//  stdev = sqrt(variance);  // Calculate standard deviation
//
////  Serial.print(mean);
////  Serial.print(", ");  
////  Serial.print(n);
////  Serial.print(", ");  
////  Serial.print(sMax - sMin);
////  Serial.print(", ");  
////  Serial.println(stdev, 3);
//  
//  touchInputMean = mean;
//}

#endif
