

void recordGestureExample() {
  // TODO: Add countdown so can prepare for gesture!
    // TODO: Add boolean flag to enable/disable countdown
    
   if (isRecordingGesture == false) {
//     backgroundColor[0] = 232; backgroundColor[1] = 94; backgroundColor[2] = 83;
     backgroundColor = #cc0000;
     isRecordingGesture = true;
     gestureSensorSampleCount = 0;
     //gestureDataFile = createWriter("gestureData.txt");
     gestureDataSample = new JSONArray();
   } else {
//     backgroundColor[0] = 255; backgroundColor[1] = 255; backgroundColor[2] = 255;
     backgroundColor = #F0F1F0;
     isRecordingGesture = false;
     // gestureDataFile.flush(); // Writes the remaining data to the file
     // gestureDataFile.close(); // Finishes the file
     
     JSONObject gestureSample = new JSONObject();
     gestureSample.setString("gesture", gestureName[gestureIndex]);
     gestureSample.setJSONArray("sample", gestureDataSample);
     
     //JSONArray gestureSampleSet;
     gestureSampleSet = loadJSONArray("data/gestureSampleSet.json"); // Load existing file
     // TODO: Add "tried gesture" to gesture sample
     //gestureSampleSet.append(gestureDataSample);
     gestureSampleSet.append(gestureSample);
     //gestureSampleSet.setJSONArray(gestureSampleCount, gestureDataSample);
     
     saveJSONArray(gestureSampleSet, "data/gestureSampleSet.json");
     
     gestureSampleCount++;
     
     // Update currently displayed gesture with new data
     updateCurrentGesture();
   } 
}

/**
 * Set up the gesture transitions that can occur from each gesture.
 */
void setupGestureTransitions() {

  // Set the gesture transitions
  for (int i = 0; i < gestureName.length; i++) {
    ArrayList<Integer> currentTransitions = new ArrayList<Integer>(); // Create list for current gesture's transitions

    if (gestureName[i] == "at rest") {
      currentTransitions.add(getGestureIndex("at rest"));
      currentTransitions.add(getGestureIndex("swing"));
      currentTransitions.add(getGestureIndex("tilt left"));
      currentTransitions.add(getGestureIndex("tilt right"));
      currentTransitions.add(getGestureIndex("tilt forward"));
      currentTransitions.add(getGestureIndex("tilt backward"));
      currentTransitions.add(getGestureIndex("tap to another, as left"));
      currentTransitions.add(getGestureIndex("tap to another, as right"));
    } 
    else if (gestureName[i] == "swing") {
      currentTransitions.add(getGestureIndex("at rest"));
      currentTransitions.add(getGestureIndex("shake"));
      currentTransitions.add(getGestureIndex("tilt left"));
      currentTransitions.add(getGestureIndex("tilt right"));
      currentTransitions.add(getGestureIndex("tilt forward"));
      currentTransitions.add(getGestureIndex("tilt backward"));
      currentTransitions.add(getGestureIndex("tap to another, as left"));
      currentTransitions.add(getGestureIndex("tap to another, as right"));
    }  
    else if (gestureName[i] == "tap to another, as left") {
      currentTransitions.add(getGestureIndex("at rest"));
    } 
    else if (gestureName[i] == "tap to another, as right") {
      currentTransitions.add(getGestureIndex("at rest"));
    }
    else if (gestureName[i] == "shake") {
      currentTransitions.add(getGestureIndex("shake"));
      currentTransitions.add(getGestureIndex("at rest"));
    }
    else if (gestureName[i] == "tilt left") {
      currentTransitions.add(getGestureIndex("tilt left"));
      currentTransitions.add(getGestureIndex("at rest"));
    } 
    else if (gestureName[i] == "tilt right") {
      currentTransitions.add(getGestureIndex("tilt right"));
      currentTransitions.add(getGestureIndex("at rest"));
    } 
    else if (gestureName[i] == "tilt forward") {
      currentTransitions.add(getGestureIndex("tilt forward"));
      currentTransitions.add(getGestureIndex("at rest"));
    } 
    else if (gestureName[i] == "tilt backward") {
      currentTransitions.add(getGestureIndex("tilt backward"));
      currentTransitions.add(getGestureIndex("at rest"));
    }

    gestureTransitions.add(currentTransitions);
  }

  // Set the initial state
  classifiedGestureIndex = 0;
}

/**
 * Gets the index associated with the specified gesture.
 */
int getGestureIndex(String findGestureName) {
  for (int i = 0; i < gestureName.length; i++) {
    if (gestureName[i].equals(findGestureName)) {
      return i;
    }
  }
  return -1;
}

/**
 * Returns the number of gestures.
 */
int getGestureCount() {
  return gestureName.length;
}

void updateCurrentGesture() {

  completeGestureSamples.clear();

  gestureSamples.clear();
  maximumSampleSize[gestureIndex] = 0;
  maximumSampleDuration = Integer.MIN_VALUE;

  gestureSampleNames.clear();
  completeGestureSampleNames.clear();

  // Populate gesture data arrays
  for (int i = 0; i < gestureSampleSet.size(); i++) {
    JSONObject gestureSample = gestureSampleSet.getJSONObject(i);
    JSONArray gestureSamplePoints = gestureSample.getJSONArray("sample");

    int sampleTimeStart = 0, sampleTimeEnd = 0, sampleDuration = 0, sampleTimeOffset = 0;

    ArrayList<ArrayList<Integer>> singleGestureSample = new ArrayList<ArrayList<Integer>>();

    // Populate data arrays
    if (gestureSample.getString("gesture").equals(gestureName[gestureIndex])) {
      ArrayList<Integer> gestureSamplePointX = new ArrayList<Integer>(); // i.e., Accelerometer X, Y, Z
      ArrayList<Integer> gestureSamplePointY = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointZ = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointGyroX = new ArrayList<Integer>(); // i.e., Gyro X, Y, Z
      ArrayList<Integer> gestureSamplePointGyroY = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointGyroZ = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointT = new ArrayList<Integer>(); // i.e., Time

      // Get gesture duration and set to maximum duration (if that's the case)
      sampleTimeStart = int(gestureSamplePoints.getJSONObject(0).getString("timestamp"));
      sampleTimeEnd = int(gestureSamplePoints.getJSONObject(gestureSamplePoints.size() - 1).getString("timestamp"));
      sampleDuration = sampleTimeEnd - sampleTimeStart;
      if (sampleDuration > maximumSampleDuration) {
        maximumSampleDuration = sampleDuration;
      }
      sampleTimeOffset = int(gestureSamplePoints.getJSONObject(0).getString("timestamp")); // The offset for the current sample (reset offset so time starts at zero)

      for (int j = 0; j < gestureSamplePoints.size(); j++) {
        JSONObject gestureSamplePoint = gestureSamplePoints.getJSONObject(j);
        gestureSamplePointX.add(int(gestureSamplePoint.getString("accelerometerX")));
        gestureSamplePointY.add(int(gestureSamplePoint.getString("accelerometerY")));
        gestureSamplePointZ.add(int(gestureSamplePoint.getString("accelerometerZ")));
        gestureSamplePointGyroX.add(int(gestureSamplePoint.getString("gyroX")));
        gestureSamplePointGyroY.add(int(gestureSamplePoint.getString("gyroY")));
        gestureSamplePointGyroZ.add(int(gestureSamplePoint.getString("gyroZ")));
        gestureSamplePointT.add(int(gestureSamplePoint.getString("timestamp")) - sampleTimeOffset);
      }

      singleGestureSample.add(gestureSamplePointX); // Accelerometer X, Y, Z
      singleGestureSample.add(gestureSamplePointY);
      singleGestureSample.add(gestureSamplePointZ);
      singleGestureSample.add(gestureSamplePointGyroX); // Gyro X, Y, Z
      singleGestureSample.add(gestureSamplePointGyroY);
      singleGestureSample.add(gestureSamplePointGyroZ);
      singleGestureSample.add(gestureSamplePointT); // Time

      // Update maximum sample count
      if (gestureSamplePoints.size() > maximumSampleSize[gestureIndex]) {
        maximumSampleSize[gestureIndex] = gestureSamplePoints.size();
      }

      // println(maximumSampleSize);

      gestureSamples.add(singleGestureSample);

      String gestureName = gestureSample.getString("gesture");
      gestureSampleNames.add(gestureName);

      completeGestureSamples.add(singleGestureSample);
      completeGestureSampleNames.add(gestureName);
    
    } else {

      ArrayList<Integer> gestureSamplePointX = new ArrayList<Integer>(); // i.e., Accelerometer X, Y, Z
      ArrayList<Integer> gestureSamplePointY = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointZ = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointGyroX = new ArrayList<Integer>(); // i.e., Gyro X, Y, Z
      ArrayList<Integer> gestureSamplePointGyroY = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointGyroZ = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointT = new ArrayList<Integer>(); // i.e., Time

      // Get gesture duration and set to maximum duration (if that's the case)
      sampleTimeStart = int(gestureSamplePoints.getJSONObject(0).getString("timestamp"));
      sampleTimeEnd = int(gestureSamplePoints.getJSONObject(gestureSamplePoints.size() - 1).getString("timestamp"));
      sampleDuration = sampleTimeEnd - sampleTimeStart;
      sampleTimeOffset = int(gestureSamplePoints.getJSONObject(0).getString("timestamp")); // The offset for the current sample (reset offset so time starts at zero)

      for (int j = 0; j < gestureSamplePoints.size(); j++) {
        JSONObject gestureSamplePoint = gestureSamplePoints.getJSONObject(j);
        gestureSamplePointX.add(int(gestureSamplePoint.getString("accelerometerX")));
        gestureSamplePointY.add(int(gestureSamplePoint.getString("accelerometerY")));
        gestureSamplePointZ.add(int(gestureSamplePoint.getString("accelerometerZ")));
        gestureSamplePointGyroX.add(int(gestureSamplePoint.getString("gyroX")));
        gestureSamplePointGyroY.add(int(gestureSamplePoint.getString("gyroY")));
        gestureSamplePointGyroZ.add(int(gestureSamplePoint.getString("gyroZ")));
        gestureSamplePointT.add(int(gestureSamplePoint.getString("timestamp")) - sampleTimeOffset);
      }

      singleGestureSample.add(gestureSamplePointX); // Accelerometer X, Y, Z
      singleGestureSample.add(gestureSamplePointY);
      singleGestureSample.add(gestureSamplePointZ);
      singleGestureSample.add(gestureSamplePointGyroX); // Gyro X, Y, Z
      singleGestureSample.add(gestureSamplePointGyroY);
      singleGestureSample.add(gestureSamplePointGyroZ);
      singleGestureSample.add(gestureSamplePointT); // Time

      // Update maximum sample count
      completeGestureSamples.add(singleGestureSample);

      String gestureName = gestureSample.getString("gesture");
      completeGestureSampleNames.add(gestureName);
    }
  }
}

/**
 * Get the accelerometer data samples for all gestures. Cache the gesture samples in memory. 
 */
ArrayList<ArrayList<ArrayList<ArrayList<Integer>>>> getGestureSamples() {

  // Initialize caching flags to "false"
  while (hasCachedGestureSamples.size () < getGestureCount()) {
    hasCachedGestureSamples.add(false);
  }

  // Initialize cache
  while (cachedGestureSamples.size () < getGestureCount()) {
    cachedGestureSamples.add(new ArrayList<ArrayList<ArrayList<Integer>>>());
  }

  // Cache gesture samples for each gesture and flag them as cached for future reference
  for (int i = 0; i < getGestureCount(); i++) {
    if (!hasCachedGestureSamples.get(i)) {
      ArrayList<ArrayList<ArrayList<Integer>>> cachedGestureSample = getGestureSamples(i);
      cachedGestureSamples.set(i, cachedGestureSample); // Cache the samples
      hasCachedGestureSamples.set(i, true); // Flag the samples as cached
    }
  }

  return cachedGestureSamples;
}

int getGestureSignatureMaximumSize() {
  int maximumSize = 0;
  for (int i = 0; i < getGestureCount(); i++) {
    if (gestureSignatureSize[i] > maximumSize) {
      maximumSize = gestureSignatureSize[i];
    }
  }
  return maximumSize;
}

/**
 * Returns a list of all gesture samples for the gesture with the specified index.
 * The returned list is in the following format:
 *    samples[sampleIndex][axis][pointIndex], where
 *      sampleIndex : [0, sampleCount]
 *      axis : [0, 1, 2] -> [x, y, z]
 *      pointIndex : [0, pointIndexCount]
 */
ArrayList<ArrayList<ArrayList<Integer>>> getGestureSamples(int gestureIndex) {

  if (hasCachedGestureSamples.size() > gestureIndex && hasCachedGestureSamples.get(gestureIndex)) {

    return cachedGestureSamples.get(gestureIndex);
    
  } else {

    // Declare the list that will store the gesture samples for the gesture with the specified index
    ArrayList<ArrayList<ArrayList<Integer>>> selectedGestureSamples = new ArrayList<ArrayList<ArrayList<Integer>>>();

    // Populate gesture data arrays
    for (int i = 0; i < gestureSampleSet.size(); i++) {
      JSONObject gestureSample = gestureSampleSet.getJSONObject(i);
      JSONArray gestureSamplePoints = gestureSample.getJSONArray("sample");

      // Variables to store time characteristics of the current, single gesture sample
      int sampleTimeStart  = 0; // Gesture sample start time
      int sampleTimeEnd    = 0; // Gesture sample end time
      int sampleDuration   = 0; // Gesture sample duration
      int sampleTimeOffset = 0; // Gesture time offset // TODO: Rather than sliding everything to the left, find the best fit for each to each other

      // List to store the time process (multiple series)
      ArrayList<ArrayList<Integer>> singleGestureSample = new ArrayList<ArrayList<Integer>>();

      // Populate lists of gesture sample data
      if (gestureSample.getString("gesture").equals(gestureName[gestureIndex])) {

        // Time series data for gesture
        ArrayList<Integer> gestureSamplePointX = new ArrayList<Integer>();
        ArrayList<Integer> gestureSamplePointY = new ArrayList<Integer>();
        ArrayList<Integer> gestureSamplePointZ = new ArrayList<Integer>();
        ArrayList<Integer> gestureSamplePointGyroX = new ArrayList<Integer>();
        ArrayList<Integer> gestureSamplePointGyroY = new ArrayList<Integer>();
        ArrayList<Integer> gestureSamplePointGyroZ = new ArrayList<Integer>();
        ArrayList<Integer> gestureSamplePointT = new ArrayList<Integer>();

        // Get gesture duration and set to maximum duration (if that's the case)
        sampleTimeStart = int(gestureSamplePoints.getJSONObject(0).getString("timestamp"));
        sampleTimeEnd = int(gestureSamplePoints.getJSONObject(gestureSamplePoints.size() - 1).getString("timestamp"));
        sampleDuration = sampleTimeEnd - sampleTimeStart;
        if (sampleDuration > maximumSampleDuration) {
          maximumSampleDuration = sampleDuration;
        }
        sampleTimeOffset = int(gestureSamplePoints.getJSONObject(0).getString("timestamp")); // The offset for the current sample (reset offset so time starts at zero)

        for (int j = 0; j < gestureSamplePoints.size(); j++) {
          JSONObject gestureSamplePoint = gestureSamplePoints.getJSONObject(j);
          
          gestureSamplePointX.add(int(gestureSamplePoint.getString("accelerometerX")));
          gestureSamplePointY.add(int(gestureSamplePoint.getString("accelerometerY")));
          gestureSamplePointZ.add(int(gestureSamplePoint.getString("accelerometerZ")));
          
          gestureSamplePointGyroX.add(int(gestureSamplePoint.getString("gyroX")));
          gestureSamplePointGyroY.add(int(gestureSamplePoint.getString("gyroY")));
          gestureSamplePointGyroZ.add(int(gestureSamplePoint.getString("gyroZ")));
          
          gestureSamplePointT.add(int(gestureSamplePoint.getString("timestamp")) - sampleTimeOffset);
        }

        // Add data point to time the series
        singleGestureSample.add(gestureSamplePointX);
        singleGestureSample.add(gestureSamplePointY);
        singleGestureSample.add(gestureSamplePointZ);
        singleGestureSample.add(gestureSamplePointGyroX);
        singleGestureSample.add(gestureSamplePointGyroY);
        singleGestureSample.add(gestureSamplePointGyroZ);
        singleGestureSample.add(gestureSamplePointT);

        // Update maximum sample count
        if (gestureSamplePoints.size() > maximumSampleSize[gestureIndex]) {
          maximumSampleSize[gestureIndex] = gestureSamplePoints.size();
        }

        // Add gesture process to list that will be returned
        selectedGestureSamples.add(singleGestureSample);
      }
    }
    return selectedGestureSamples;
  }
}

/**
 * Returns a subsample of the gesture signature of the specified size starting from the specified offset.
 */
ArrayList<ArrayList<Integer>> getGestureSampleAverage2(ArrayList<ArrayList<ArrayList<Integer>>> currentGestureSamples, int offset, int size) {

  // Get a list containing the average of gesture examples (i.e., the average sample)
  ArrayList<ArrayList<Integer>> averageSample = getGestureSampleAverage(currentGestureSamples);

  // Initialize lists for the subsample from the gesture signature (i.e., the average of all sample)
  ArrayList<ArrayList<Integer>> averageSubsample = new ArrayList<ArrayList<Integer>>();
  averageSubsample.add(new ArrayList<Integer>()); // i.e., Accelerometer X
  averageSubsample.add(new ArrayList<Integer>()); // i.e., Accelerometer Y
  averageSubsample.add(new ArrayList<Integer>()); // i.e., Accelerometer Z
  averageSubsample.add(new ArrayList<Integer>()); // i.e., Gyro X
  averageSubsample.add(new ArrayList<Integer>()); // i.e., Gyro Y
  averageSubsample.add(new ArrayList<Integer>()); // i.e., Gyro Z

  // Calculate the starting index of the sublist
  //  int sublistStartIndex = 0;
  //  if (size < averageSample.get(0).size()) {
  //    sublistStartIndex = (averageSample.get(0).size() - size); // Set new start index for list
  //  }

  int sublistStartIndex = 0;
  if (size < averageSample.get(0).size()) {
    sublistStartIndex = offset; // Set new start index for list
  }

  //  println();
  //  println("gestureIndex = " + gestureIndex);
  //  println("offset = " + offset);
  //  println("size = " + size);
  //  println("averageSample.get(" + gestureIndex + ").size() = " + averageSample.get(0).size());
  //  println("sublistStartIndex = " + sublistStartIndex);

  // Create sublist. Copy elements from gesture signature to a new list structure (the sublist).
  // if (offset >= 0 && (offset + size) < averageSample.get(0).size()) {
  //  if (offset >= 0 && (offset + size) < averageSample.get(0).size()) {
  //    for (int i = offset; i < offset + size; i++) {
  //for (int i = sublistStartIndex; i < averageSample.get(0).size(); i++) {
  for (int i = sublistStartIndex; i < offset + size; i++) {
    averageSubsample.get(0).add(averageSample.get(0).get(i)); // i.e., Accelerometer X
    averageSubsample.get(1).add(averageSample.get(1).get(i)); // i.e., Accelerometer Y
    averageSubsample.get(2).add(averageSample.get(2).get(i)); // i.e., Accelerometer Z
    
    if (dimensionCount > 3) {
      averageSubsample.get(3).add(averageSample.get(3).get(i)); // i.e., Gyro X
      averageSubsample.get(4).add(averageSample.get(4).get(i)); // i.e., Gyro Y
      averageSubsample.get(5).add(averageSample.get(5).get(i)); // i.e., Gyro Z
    }
  }

  //    println("offset = " + offset + ", size = " + size + ", size(sample) = " + averageSample.get(0).size() + ", size(slice) = " + averageSubsample.get(0).size());

  return averageSubsample;
}

// TODO: Get gesture subsample using offset (the offset doesn't affect the live subsample the data is compared)

//  println("offset = " + offset + ", size = " + size + ", size(sample) = " + averageSample.get(0).size() + ", size(slice) = " + averageSubsample.get(0).size());
//  return averageSample;  
//}

/**
 * Returns the average time series curve (i.e., the average gesture signature)
 * used for gesture classification.
 *
 * Note: This function was made to be called after
 *       ArrayList<ArrayList<ArrayList<Integer>>> getGestureSamples(int gestureIndex).
 */
ArrayList<ArrayList<Integer>> getGestureSampleAverage(ArrayList<ArrayList<ArrayList<Integer>>> currentGestureSamples) {

  ArrayList<ArrayList<Integer>> gestureSampleAverageSum = new ArrayList<ArrayList<Integer>>();
  ArrayList<ArrayList<Integer>> gestureSampleAverageCount = new ArrayList<ArrayList<Integer>>();

  // Create array to store average of accelerometer sample data
  if (gestureSampleAverageSum.size() < dimensionCount) {
    gestureSampleAverageSum.add(new ArrayList<Integer>()); // i.e., Accelerometer X
    gestureSampleAverageSum.add(new ArrayList<Integer>()); // i.e., Accelerometer Y
    gestureSampleAverageSum.add(new ArrayList<Integer>()); // i.e., Accelerometer Z
    gestureSampleAverageSum.add(new ArrayList<Integer>()); // i.e., Gyro X
    gestureSampleAverageSum.add(new ArrayList<Integer>()); // i.e., Gyro Y
    gestureSampleAverageSum.add(new ArrayList<Integer>()); // i.e., Gyro Z
  }

  // Create array to store the number of accelerometer sample data points (for the division in the computation of the accelerometer point averages)
  if (gestureSampleAverageCount.size() < dimensionCount) {
    gestureSampleAverageCount.add(new ArrayList<Integer>()); // i.e., Accelerometer X
    gestureSampleAverageCount.add(new ArrayList<Integer>()); // i.e., Accelerometer Y
    gestureSampleAverageCount.add(new ArrayList<Integer>()); // i.e., Accelerometer Z
    gestureSampleAverageCount.add(new ArrayList<Integer>()); // i.e., Gyro X
    gestureSampleAverageCount.add(new ArrayList<Integer>()); // i.e., Gyro Y
    gestureSampleAverageCount.add(new ArrayList<Integer>()); // i.e., Gyro Z
  }

  // Compute averages
  int sampleCount = 0;
  for (int i = 0; i < currentGestureSamples.size(); i++) {

    ArrayList<ArrayList<Integer>> singleGestureSample = currentGestureSamples.get(i);

    if (singleGestureSample.size() > 0) {
      sampleCount++;

      for (int dimension = 0; dimension < dimensionCount; dimension++) {

        // Update list sizes
        while (gestureSampleAverageSum.get (dimension).size() < singleGestureSample.get(dimension).size()) {
          gestureSampleAverageSum.get(dimension).add(0);
          gestureSampleAverageCount.get(dimension).add(0);
        }

        // Update averages
        for (int j = 0; j < singleGestureSample.get(dimension).size(); j++) {

          // Update average
          int cumulativeGestureSample = singleGestureSample.get(dimension).get(j) + gestureSampleAverageSum.get(dimension).get(j);
          gestureSampleAverageSum.get(dimension).set(j, cumulativeGestureSample); // Update value

            // Update average count
          int gestureSampleCount = gestureSampleAverageCount.get(dimension).get(j) + 1;
          gestureSampleAverageCount.get(dimension).set(j, gestureSampleCount); // Update value
        }
      }
    }
  }

  // Compute average of accelerometer x-axis, y-axis, and z-axis data
  for (int dimension = 0; dimension < dimensionCount; dimension++) {
    // Compute average of accelerometer data for current axis
    for (int j = 0; j < gestureSampleAverageSum.get(dimension).size(); j++) {
      int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(dimension).get(j)) / float(gestureSampleAverageCount.get(dimension).get(j)));
      gestureSampleAverageSum.get(dimension).set(j, cumulativeGestureSample); // Update value
    }
  }

  return gestureSampleAverageSum;
}

/**
 * Utilities
 */

void printGestureSignatures() {

  for (int gestureSignatureIndex = 0; gestureSignatureIndex < getGestureCount(); gestureSignatureIndex++) {

    ArrayList<ArrayList<ArrayList<Integer>>> gestureSamples = getGestureSamples(gestureSignatureIndex);
    //ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage(gestureSamples);
    ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage2(gestureSamples, gestureSignatureOffset[gestureSignatureIndex], gestureSignatureSize[gestureSignatureIndex]);
    println("Size: " + gestureSignatureSample.get(0).size());

    for (int axis = 0; axis < gestureSignatureSample.size(); axis++) {
      print("gestureSignatures[" + gestureSignatureIndex + "][" + axis + "] = { ");
      int gestureSignatureSize = gestureSignatureSample.get(0).size(); // 50;
      for (int pointIndex = 0; pointIndex < gestureSignatureSize; pointIndex++) {
        print("" + gestureSignatureSample.get(axis).get(pointIndex));
        if (pointIndex < gestureSignatureSize - 1) {
          print(", ");
        } 
        else {
          print(" };");
        }
      }
      println();
    }
    println();
  }
}
