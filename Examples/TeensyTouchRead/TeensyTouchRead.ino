/*  Test capacitive touch sensor function on Teensy 3.0
    using Teensy 3.0 beta 6 (Windows) release    J.Beale Oct. 23 2012
*/

#define TOUCH_SAMPLES (5000)   // maximum number of separate readings to take
#define TOUCH_TIMEOUT (50) // (100) // (500)    // maximum sample integration time in milliseconds

int led = 13;

void setup() {                
  pinMode(led, OUTPUT);     
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  // delay(3000);               // wait for user to start up logging program
  Serial.begin(115200);       // baud rate is ignored with Teensy USB ACM i/o
  // delay(2000);
  Serial.println("val,n,p-p,stdev");
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
}

double touchInputMean = 0;

void loop() {
  getInputPort();
  
  Serial.print(touchInputMean);
  
  // Send output if input surpasses "touch" or "connected" thresholds
  if (touchInputMean > 3000) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
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
    x = touchRead(A1);
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
