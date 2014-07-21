/**
 * Classify the gesture. Choose the gesture that has a "signature" time series that best  
 * matches the recent window of live data.
 */
int classifyGesture(ArrayList<ArrayList<Integer>> liveSample) {
  int minimumDeviationIndex = -1;
  int minimumDeviation = Integer.MAX_VALUE;

  for (int gestureSignatureIndex = 0; gestureSignatureIndex < getGestureCount(); gestureSignatureIndex++) {

    ArrayList<ArrayList<ArrayList<Integer>>> gestureSamples = getGestureSamples(gestureSignatureIndex);
    //ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage(gestureSamples);
    ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage2(gestureSamples, gestureSignatureOffset[gestureSignatureIndex], gestureSignatureSize[gestureSignatureIndex]);

    // Calculate the gesture's deviation from the gesture signature
    int gestureDeviation = getGestureDeviation(gestureSignatureSample, liveSample);
    int gestureInstability = getGestureInstability(gestureSignatureSample, liveSample);

    // Check if the sample's deviation
    if (minimumDeviationIndex == -1 || (gestureDeviation + gestureInstability) < minimumDeviation) {
      minimumDeviationIndex = gestureSignatureIndex;
      minimumDeviation = gestureDeviation + gestureInstability;
    }
  }

  return minimumDeviationIndex;
}

/**
 * Classify the gesture. Choose the gesture that has a "signature" time series that best  
 * matches the recent window of live data.
 */
int classifyGestureFromTransitions(ArrayList<ArrayList<Integer>> liveSample) {
  int minimumDeviationIndex = -1;
  int minimumDeviation = Integer.MAX_VALUE;

  ArrayList<Integer> possibleGestures = gestureTransitions.get(classifiedGestureIndex); // Get list of possible gestures based on current state

  // Loop through possible gestures, calculate the deviation betwen the gesture's signature 
  // and the live sensor data sample.
  for (int i = 0; i < possibleGestures.size(); i++) {

    int gestureSignatureIndex = possibleGestures.get(i); // Get index of possible gesture

    ArrayList<ArrayList<ArrayList<Integer>>> gestureSamples = getGestureSamples(gestureSignatureIndex);
    //      ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage(gestureSamples);
    //ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage2(gestureSamples, gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex]);
    ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage2(gestureSamples, gestureSignatureOffset[gestureSignatureIndex], gestureSignatureSize[gestureSignatureIndex]);

    // Calculate the gesture's deviation from the gesture signature
    int gestureDeviation = getGestureDeviation(gestureSignatureSample, liveSample);
    //int gestureInstability = 0;
    int gestureInstability = getGestureInstability(gestureSignatureSample, liveSample);
    //      println("gestureDeviation = " + gestureDeviation + ", gestureInstability = " + gestureInstability);
    //      println();

    // Check if the sample's deviation
    if (minimumDeviationIndex == -1 || (gestureDeviation + gestureInstability) < minimumDeviation) {
      minimumDeviationIndex = gestureSignatureIndex;
      minimumDeviation = gestureDeviation + gestureInstability;
    }
  }

  return minimumDeviationIndex;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureInstability(ArrayList<ArrayList<Integer>> averageSample, ArrayList<ArrayList<Integer>> liveSample) {
  int instabilityTotal = 0;
  
  // Compare the difference between the average sample for each axis and the live sample
  for (int dimension = 0; dimension < dimensionCount; dimension++) {
    ArrayList<Integer> liveSampleAxis = liveSample.get(dimension);

    int instability = getGestureAxisInstability(averageSample.get(dimension), liveSample.get(dimension));
    instabilityTotal = instabilityTotal + instability;
  }

  return instabilityTotal;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureDeviation(ArrayList<ArrayList<Integer>> averageSample, ArrayList<ArrayList<Integer>> liveSample) {
  int deltaTotal = 0;

  // Compare the difference between the average sample for each axis and the live sample
  for (int dimension = 0; dimension < dimensionCount; dimension++) {
    ArrayList<Integer> liveSampleAxis = liveSample.get(dimension);

    int delta = getGestureAxisDeviation(averageSample.get(dimension), liveSample.get(dimension));
    deltaTotal = deltaTotal + delta;
  }

  return deltaTotal;
}

/**
 * Calculate the deviation of the live gesture sample and the signature gesture sample along only one axis (x, y, or z).
 */
int getGestureAxisDeviation(ArrayList<Integer> gestureSample, ArrayList<Integer> liveSample) {

  int delta = 0; // sum of difference between average x curve and most-recent x data

  //  for (int i = liveSample.size() - comparisonWindowSize; i < liveSample.size(); i++) {
  //    if (i < liveSample.size() && i < gestureSample.size()) {
  //        int difference = abs(gestureSample.get(i) - liveSample.get(i));
  //        delta = delta + difference;
  //    }
  //  }

  for (int i = 0; i < gestureSample.size(); i++) {
    if (i < liveSample.size() && i < gestureSample.size()) {
      int difference = abs(gestureSample.get(i) - liveSample.get(i));
      delta = delta + difference;
    }
  }

  return delta;
}

/**
 * Relative instability. How relative is the live sample in comparison to a gesture's signature sample?
 */
int getGestureAxisInstability(ArrayList<Integer> gestureSample, ArrayList<Integer> liveSample) {

  int relativeInstability = 0; // sum of difference between average x curve and most-recent x data

  //  for (int i = liveSample.size() - comparisonWindowSize; i < liveSample.size() - 1; i++) {
  //    if (i < liveSample.size() && i < gestureSample.size()) {
  //        int signatureDifference = abs(gestureSample.get(i + 1) - gestureSample.get(i));
  //        int liveDifference = abs(liveSample.get(i + 1) - liveSample.get(i));
  //        int instabilityDifference = abs(signatureDifference - liveDifference);
  //        
  //        relativeInstability = relativeInstability + instabilityDifference;
  //    }
  //  }

  for (int i = 0; i < gestureSample.size() - 1; i++) {
    //  for (int i = 0; i < gestureSample.size() - 1; i++) {
    if (i < liveSample.size() && i < gestureSample.size() && gestureSample.size() <= liveSample.size()) {
      int signatureDifference = abs(gestureSample.get(i + 1) - gestureSample.get(i));
      int liveDifference = abs(liveSample.get(i + 1) - liveSample.get(i));
      int instabilityDifference = abs(signatureDifference - liveDifference);

      relativeInstability = relativeInstability + instabilityDifference;
    }
  }

  return relativeInstability;
}
