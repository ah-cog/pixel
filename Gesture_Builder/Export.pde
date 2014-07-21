void openGestureData() {

  String gestureFilePath = "data/gestureSampleSet.json";
//  File gestureFile = new File(gestureFilePath);
//  if (!gestureFile.exists()) {
//    PrintWriter emptyGestureFile = createWriter(gestureFilePath);
//    emptyGestureFile.println("[]"); // Write empty JSON array for storing gestures
//    emptyGestureFile.flush();
//    emptyGestureFile.close();
//  }
//  if (gestureFile.exists()) {
    // Load existing gesture data file
    gestureSampleSet = loadJSONArray(gestureFilePath);
//  }

  // updateCurrentGesture();
}


/**
 * Store gesture data sample, consisting of a sequence of data points 
 * collected over time, during a specific duration, specified manually 
 * by a human.
 */
void storeGestureData() {
  
  JSONObject gestureDataPoint = new JSONObject();
  
  gestureDataPoint.setString("timestamp", str(dataTimestamp));
  gestureDataPoint.setString("roll", str(roll));
  gestureDataPoint.setString("pitch", str(pitch));
  gestureDataPoint.setString("yaw", str(yaw));
  gestureDataPoint.setString("gyroX", str(gyroX));
  gestureDataPoint.setString("gyroY", str(gyroY));
  gestureDataPoint.setString("gyroZ", str(gyroZ));
  gestureDataPoint.setString("accelerometerX", str(accelerometerX));
  gestureDataPoint.setString("accelerometerY", str(accelerometerY));
  gestureDataPoint.setString("accelerometerZ", str(accelerometerZ));
  gestureDataPoint.setString("magnetometerX", str(magnetometerX));
  gestureDataPoint.setString("magnetometerY", str(magnetometerY));
  gestureDataPoint.setString("magnetometerZ", str(magnetometerZ));
  gestureDataPoint.setString("pressure", str(pressure));
  gestureDataPoint.setString("altitude", str(altitude));
  gestureDataPoint.setString("temperature", str(temperature));

  // gestureDataSample.setJSONObject(gestureSensorSampleCount, gestureData);
  gestureDataSample.append(gestureDataPoint);
  
  gestureSensorSampleCount++;
}

/**
 * Writes the current gesture model to a .h header file for use with Arduino sketches.
 */
void saveGestureModelFile() {

  PrintWriter gestureFile = createWriter("Gestures.h");

  // GESTURE_COUNT 9
  // AXIS_COUNT 3
  // GESTURE_SIGNATURE_MAXIMUM_SIZE 50
  int GESTURE_COUNT = getGestureCount();
  int AXIS_COUNT = 3;
  int GESTURE_SIGNATURE_MAXIMUM_SIZE = getGestureSignatureMaximumSize();
  int GESTURE_CANDIDATE_SIZE = GESTURE_SIGNATURE_MAXIMUM_SIZE; // The gesture candidate can be no larger than the maximum gesture signature size, so limit it to that.

  gestureFile.println("#define GESTURE_COUNT " + GESTURE_COUNT + "");
  gestureFile.println("#define AXIS_COUNT " + AXIS_COUNT + "");
  gestureFile.println("#define GESTURE_SIGNATURE_MAXIMUM_SIZE " + GESTURE_SIGNATURE_MAXIMUM_SIZE + "");
//  gestureFile.println("#define DEFAULT_GESTURE_SIGNATURE_SIZE " + DEFAULT_GESTURE_SIGNATURE_SIZE + "");
  gestureFile.println("#define GESTURE_CANDIDATE_SIZE " + GESTURE_CANDIDATE_SIZE + " // The number of most recent live data points to store");
  gestureFile.println();
  
  gestureFile.println("int classifiedGestureIndex = 0;");
  gestureFile.println("int previousClassifiedGestureIndex = -1;");
  gestureFile.println("unsigned long lastGestureClassificationTime = 0L; // Time of last gesture classification");
  gestureFile.println();

  gestureFile.println("char* gestureName[GESTURE_COUNT] = {");
  for (int i = 0; i < getGestureCount(); i++) {
    if (i < getGestureCount() - 1) {
      gestureFile.println("\t\"" + gestureName[i] + "\",");
    } else {
      gestureFile.println("\t\"" + gestureName[i] + "\"");
    }
  }
  gestureFile.println("};");
  gestureFile.println();

//  gestureFile.println("int gestureSignatureOffset[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };");
//  gestureFile.println();

  gestureFile.print("int gestureSignatureSize[] = { ");
  for (int i = 0; i < getGestureCount(); i++) {
    if (i < getGestureCount() - 1) {
      // gestureFile.println("\tDEFAULT_GESTURE_SIGNATURE_SIZE,");
      gestureFile.print("" + gestureSignatureSize[i] + ", ");
    } else {
      // gestureFile.println("\tDEFAULT_GESTURE_SIGNATURE_SIZE");
      gestureFile.println("" + gestureSignatureSize[i] + " };");
    }
  }
//  gestureFile.println("};");
  gestureFile.println();

  gestureFile.println("int gestureSustainDuration[GESTURE_COUNT] = {");
  gestureFile.println("\t0, // \"at rest\"");
  gestureFile.println("\t600, // \"swing\"");
  gestureFile.println("\t800, // \"tap to another, as left\"");
  gestureFile.println("\t800,  // \"tap to another, as right\"");
  gestureFile.println("\t200, // \"shake\"");
  gestureFile.println("\t100, // \"tilt left\"");
  gestureFile.println("\t100, // \"tilt right\"");
  gestureFile.println("\t100, // \"tilt forward\"");
  gestureFile.println("\t100 // \"tilt backward\"");
  gestureFile.println("};");
  gestureFile.println();

  gestureFile.println("int gestureTransitions[GESTURE_COUNT][GESTURE_COUNT] = {");
  for (int i = 0; i < gestureTransitions.size(); i++) {
    gestureFile.print("\t{");

    for (int j = 0; j < gestureTransitions.size(); j++) {
      if (j < gestureTransitions.get(i).size()) {
        gestureFile.print(" " + gestureTransitions.get(i).get(j));
      } 
      else {
        gestureFile.print(" -1");
      }

      if (j < gestureTransitions.size() - 1) {
        gestureFile.print(",");
      }
    }

    // Write either "}," or "}" to the file, depending on whether the data for the last axis are being written or not
    if (i < gestureTransitions.size() - 1) {
      gestureFile.println(" },");
    } 
    else {
      gestureFile.println(" }");
    }
  }
  gestureFile.println("};");

  gestureFile.println("int gestureCandidateSize = 0; // Current size of the live gesture data");

  gestureFile.println("int gestureCandidate[AXIS_COUNT][GESTURE_CANDIDATE_SIZE] = {");
  for (int axis = 0; axis < 3; axis++) {
    gestureFile.print("\t{");
    for (int i = 0; i < GESTURE_CANDIDATE_SIZE; i++) {
      gestureFile.print(" 0");
      if (i < GESTURE_CANDIDATE_SIZE - 1) {
        gestureFile.print(",");
      } 
      else {
        // Write either "}," or "}" to the file, depending on whether the data for the last axis are being written or not
        if (axis < 3 - 1) {
          gestureFile.println(" },");
        } 
        else {
          gestureFile.println(" }");
        }
      }
    }
  }
  gestureFile.println("};");

  gestureFile.println("\n");

  gestureFile.println("int gestureSampleCount = 0;");
  gestureFile.println("int gestureSensorSampleCount = 0;");

  gestureFile.println("int gestureSignature[GESTURE_COUNT][AXIS_COUNT][GESTURE_SIGNATURE_MAXIMUM_SIZE] = {");

  for (int gestureSignatureIndex = 0; gestureSignatureIndex < getGestureCount(); gestureSignatureIndex++) {

    ArrayList<ArrayList<ArrayList<Integer>>> gestureSamples = getGestureSamples(gestureSignatureIndex);
    //ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage(gestureSamples);
    ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage2(gestureSamples, gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex]);
    //println("Size: " + gestureSignatureSample.get(0).size());

    gestureFile.println("\t{");

    for (int axis = 0; axis < gestureSignatureSample.size(); axis++) {
      //print("gestureSignature[" + gestureSignatureIndex + "][" + axis + "] = { ");

      gestureFile.print("\t\t{ ");

      int gestureSignatureSize = gestureSignatureSample.get(0).size(); // 50;

      for (int pointIndex = 0; pointIndex < gestureSignatureSize; pointIndex++) {
        gestureFile.print("" + gestureSignatureSample.get(axis).get(pointIndex));
        if (pointIndex < gestureSignatureSize - 1) {
          gestureFile.print(", ");
        } 
        else {
          // Write either "}," or "}" to the file, depending on whether the data for the last axis are being written or not
          if (axis < gestureSignatureSample.size() - 1) {
            gestureFile.println(" },");
          } 
          else {
            gestureFile.println(" }");
          }
        }
      }
    }

    // Write "}," or "}" to the end of the line, depending on whether it's the last gesture or not
    if (gestureSignatureIndex < getGestureCount() - 1) {
      gestureFile.println("\t},");
    } 
    else {
      gestureFile.println("\t}");
    }
  }
  gestureFile.println("};");

  gestureFile.flush(); // Write remaining data to file
  gestureFile.close(); // Finish writing to the file and close it
}
