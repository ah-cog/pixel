void drawGestureTitle() {
  if (showGesturePrompt) {
    fill(0); 
    textFont(gestureFont); 
    textAlign(CENTER);
    text("" + gestureName[classifiedGestureIndex] + "", (width / 2), (height / 4) - 50);
  }
}

void drawExampleGestureTitle() {
  if (showGesturePrompt) {
    if (classifiedGestureIndex != -1) {
      fill(0); 
      textFont(classifiedGestureFont); 
      textAlign(CENTER);
      text("showing " + gestureName[gestureIndex] + "  ", (width / 2), (height / 4) + 20);
    }
  }
}

//void drawGestureSignatureBoundaries() {
//  // gestureSignatureBoundingIndices
//  // gestureIndex
//  
//  // Draw lines connecting all points
//  smooth();
//  strokeWeight(1);
//  
//  // int sampleSize = averageSample.get(0).size();
//  int scaledBoundLeft = int((float(gestureSignatureBoundingIndices[gestureIndex][0]) / float(maximumSampleSize)) * width);
//  int scaledBoundRight = int((float(gestureSignatureBoundingIndices[gestureIndex][1]) / float(maximumSampleSize)) * width);
//  //int scaledWidth = int((float(liveGestureSize) / float(averageSample.get(0).size())) * width);
////  if (liveGestureSample.size() > 0) {
////    stroke(255,0,0); drawPlot(liveGestureSample.get(0), 0, height / 2, scaledWidth, height, 0, 1000);
////    stroke(0,255,0); drawPlot(liveGestureSample.get(1), 0, height / 2, scaledWidth, height, 0, 1000);
////    stroke(0,0,255); drawPlot(liveGestureSample.get(2), 0, height / 2, scaledWidth, height, 0, 1000);
////  }
//  
//  
//  // Draw starting boundary of gesture signature
//  stroke(128, 128, 128);
//  line(scaledBoundLeft, 0, scaledBoundLeft, height);
//  //line(gestureSignatureBoundingIndices[gestureIndex][0], 0, gestureSignatureBoundingIndices[gestureIndex][0], height);
//  
//  // Draw stopping boundary of gesture signature 
//  stroke(128, 128, 128);
//  line(scaledBoundRight, 0, scaledBoundRight, height);
//  //line(scaledWidth, 0, scaledWidth, height);
//  //line(gestureSignatureBoundingIndices[gestureIndex][1], 0, gestureSignatureBoundingIndices[gestureIndex][1], height);
//}

/**
 * Draw the plot for the currently selected gesture signature.
 */
//void drawGesturePlot() {
//  
//  // Draw lines connecting all points
//  smooth();
//  strokeWeight(1);
//  for (int i = 0; i < gestureSamples.size(); i++) {
//    ArrayList<ArrayList<Integer>> singleGestureSample = gestureSamples.get(i);
//    if (singleGestureSample.size() > 0) {
//      stroke(255,0,0); drawPlot(singleGestureSample.get(0), 0, height / 2, width, height, 0, 1000);
//      stroke(0,255,0); drawPlot(singleGestureSample.get(1), 0, height / 2, width, height, 0, 1000);
//      stroke(0,0,255); drawPlot(singleGestureSample.get(2), 0, height / 2, width, height, 0, 1000);
//    }
//  }
//}

void drawSignatureData() {
  // Draw ending bound of gesture signature
  fill(0); 
  textFont(boundaryLabelFont); 
  textAlign(RIGHT);
  text("" + gestureName[gestureIndex] + "\nsize " + maximumSampleSize[gestureIndex] + "\nexamples " + gestureSamples.size(), width - 8, 30);
  
  text("" + "mode " + keyMode, width - 8, height - 30);
} 

/**
 * Draw the plot for the latest accelerometer data.
 */
void drawLiveGesturePlot() {

  // Draw lines connecting all points
  smooth();
  strokeWeight(1);

  int scaledBoundLeft = int((float(gestureSignatureOffset[gestureIndex]) / float(maximumSampleSize[gestureIndex])) * width);
  int scaledBoundRight = int((float(gestureSignatureOffset[gestureIndex] + gestureSignatureSize[gestureIndex]) / float(maximumSampleSize[gestureIndex])) * width);
  //int signatureSize = (gestureSignatureOffset[gestureIndex] + gestureSignatureSize[gestureIndex]) - gestureSignatureOffset[gestureIndex];

  // int sampleSize = averageSample.get(0).size();
  //  int scaledOffsetStart = int((float(gestureSIgnatureOffset[gestureIndex]) / float(maximumSampleSize)) * width);
  //int scaledWidth = int((float(liveGestureSize + gestureSignatureOffset[gestureIndex]) / float(maximumSampleSize)) * width);
  //int scaledWidth = int((float(liveGestureSize) / float(averageSample.get(0).size())) * width);
  if (liveGestureSample.size() > 0) {
    // TODO: Check here with an "if" whether the index should be drawn (if it's in the boundary range)
    stroke(255, 0, 0); 
    drawPlot(liveGestureSample.get(0), gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex], scaledBoundLeft, height / 2, scaledBoundRight - scaledBoundLeft, height, 0, 1000);
    stroke(0, 255, 0); 
    drawPlot(liveGestureSample.get(1), gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex], scaledBoundLeft, height / 2, scaledBoundRight - scaledBoundLeft, height, 0, 1000);
    stroke(0, 0, 255); 
    drawPlot(liveGestureSample.get(2), gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex], scaledBoundLeft, height / 2, scaledBoundRight - scaledBoundLeft, height, 0, 1000);
    
    if (dimensionCount > 3) {
      stroke(255, 0, 0); 
      drawPlot(liveGestureSample.get(3), gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex], scaledBoundLeft, height / 2, scaledBoundRight - scaledBoundLeft, height, 0, 1000);
      stroke(0, 255, 0); 
      drawPlot(liveGestureSample.get(4), gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex], scaledBoundLeft, height / 2, scaledBoundRight - scaledBoundLeft, height, 0, 1000);
      stroke(0, 0, 255); 
      drawPlot(liveGestureSample.get(5), gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex], scaledBoundLeft, height / 2, scaledBoundRight - scaledBoundLeft, height, 0, 1000);
    }
  }

  // Draw beginning bound of gesture signature
  stroke(128, 128, 128);
  line(scaledBoundLeft, 0, scaledBoundLeft, height);
  fill(0); 
  textFont(boundaryLabelFont); 
  textAlign(LEFT);
  text("least recent\noffset " + gestureSignatureOffset[gestureIndex], scaledBoundLeft + 8, 30);

  // Draw ending bound of gesture signature
  stroke(128, 128, 128);
  line(scaledBoundRight, 0, scaledBoundRight, height);
  fill(0); 
  textFont(boundaryLabelFont); 
  textAlign(RIGHT);
  text("now\nsize " + gestureSignatureSize[gestureIndex], scaledBoundRight - 8, 30);
}

void drawGesturePlotBoundaries() {

  ArrayList<ArrayList<Integer>> gestureSampleUpperBounds = new ArrayList<ArrayList<Integer>>();
  ArrayList<ArrayList<Integer>> gestureSampleLowerBounds = new ArrayList<ArrayList<Integer>>();
  ArrayList<ArrayList<Integer>> gestureSampleAverageSum = new ArrayList<ArrayList<Integer>>();
  ArrayList<ArrayList<Integer>> gestureSampleAverageCount = new ArrayList<ArrayList<Integer>>();

  while (gestureSampleUpperBounds.size() < dimensionCount) {
    gestureSampleUpperBounds.add(new ArrayList<Integer>());
  }

  while (gestureSampleLowerBounds.size() < dimensionCount) {
    gestureSampleLowerBounds.add(new ArrayList<Integer>());
  }

  while (gestureSampleAverageSum.size() < dimensionCount) {
    gestureSampleAverageSum.add(new ArrayList<Integer>());
  }

  while (gestureSampleAverageCount.size() < dimensionCount) {
    gestureSampleAverageCount.add(new ArrayList<Integer>());
  }

  // TODO: Move axisVisible and axisColor elsewhere... for efficiency's sake!
  boolean axisVisible[] = {
    showAxisX, showAxisY, showAxisZ,
    showAxisGyroX, showAxisGyroY, showAxisGyroZ
  };
  int axisColor[][] = {
    { 255, 0, 0, 20 }, // i.e., Accelerometer X, Y, Z
    { 0, 255, 0, 20 }, 
    { 0, 0, 255, 20 }, 
    { 255, 0, 0, 20 }, // i.e., Gyro X, Y, Z
    { 0, 255, 0, 20 }, 
    { 0, 0, 255, 20 }
  };

  // Draw lines connecting all points
  smooth();
  strokeWeight(1);
  int sampleCount = 0;
  for (int i = 0; i < gestureSamples.size(); i++) {

    ArrayList<ArrayList<Integer>> singleGestureSample = gestureSamples.get(i);

    if (singleGestureSample.size() > 0) {
      sampleCount++;

      for (int axis = 0; axis < dimensionCount; axis++) {

        // Draw gesture accelerometer data for current axis

        //stroke(255,0,0, 20); drawPlot(singleGestureSample.get(0), 0, height / 2, width, height, 0, 1000);
        if (axisVisible[axis]) {
          stroke(axisColor[axis][0], axisColor[axis][1], axisColor[axis][2], axisColor[axis][3]); 
          drawPlot(singleGestureSample.get(axis), 0, height / 2, width, height, 0, 1000);
        }

        // Update list sizes
        while (gestureSampleUpperBounds.get (axis).size() < singleGestureSample.get(axis).size()) {
          gestureSampleUpperBounds.get(axis).add(Integer.MIN_VALUE);
          gestureSampleLowerBounds.get(axis).add(Integer.MAX_VALUE);
          gestureSampleAverageSum.get(axis).add(0);
          gestureSampleAverageCount.get(axis).add(0);
        }

        // println(singleGestureSample.get(axis).size());

        // Update all upper bounds
        for (int j = 0; j < singleGestureSample.get(axis).size(); j++) {

          if (singleGestureSample.get(axis).get(j) > gestureSampleUpperBounds.get(axis).get(j)) { // Check if the current value is greater than the currently-stored upper bound
            gestureSampleUpperBounds.get(axis).set(j, singleGestureSample.get(axis).get(j)); // Update value
          }

          // Update lower bound
          if (singleGestureSample.get(axis).get(j) < gestureSampleLowerBounds.get(axis).get(j)) { // Check if the current value is greater than the currently-stored upper bound
            gestureSampleLowerBounds.get(axis).set(j, singleGestureSample.get(axis).get(j)); // Update value
          }

          // Update average
          int cumulativeGestureSample = singleGestureSample.get(axis).get(j) + gestureSampleAverageSum.get(axis).get(j);
          gestureSampleAverageSum.get(axis).set(j, cumulativeGestureSample); // Update value
          // Update average count
          int gestureSampleCount = gestureSampleAverageCount.get(axis).get(j) + 1;
          gestureSampleAverageCount.get(axis).set(j, gestureSampleCount); // Update value
        }
      }
    }
  }

  // Compute average of accelerometer x-axis, y-axis, and z-axis data
  for (int axis = 0; axis < 3; axis++) {
    // Compute average of accelerometer data for current axis
    for (int j = 0; j < gestureSampleAverageSum.get(axis).size(); j++) {
      //int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(0).get(j)) / float(sampleCount));
      int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(axis).get(j)) / float(gestureSampleAverageCount.get(axis).get(j)));
      gestureSampleAverageSum.get(axis).set(j, cumulativeGestureSample); // Update value
    }
  }

  // Plot sample data
  for (int axis = 0; axis < 3; axis++) {
    if (axisVisible[axis]) {
      stroke(axisColor[axis][0], axisColor[axis][1], axisColor[axis][2], 85); 
      drawPlot(gestureSampleUpperBounds.get(axis), 0, height / 2, width, height, 0, 1000);
      stroke(axisColor[axis][0], axisColor[axis][1], axisColor[axis][2], 85); 
      drawPlot(gestureSampleLowerBounds.get(axis), 0, height / 2, width, height, 0, 1000);
      stroke(axisColor[axis][0], axisColor[axis][1], axisColor[axis][2], 180); 
      drawPlot(gestureSampleAverageSum.get(axis), 0, height / 2, width, height, 0, 1000);
      //drawPlotNodes(12, gestureSampleAverageSum.get(axis), 0, height / 2, width, height, 0, 1000);
    }
  }
}

void drawPlot(ArrayList<Integer> data, int dataOffset, int dataSize, int originX, int originY, int plotWidth, int plotHeight, int plotRangeFloor, int plotRangeCeiling) {

  //  int dataRange = data.size() - 1;
  //  int min = min(data.size() - 1, dataOffset); // Move -1 to the end of the expression?
  //  for (int i = data.size() - 1; i > min; i--) {
  //    line(
  //      map(i - 1, min, dataRange, originX, originX + plotWidth), // x1
  //      map(data.get(i - 1), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight), // y1
  //      map(i, min, dataRange, originX, originX + plotWidth), // x2
  //      map(data.get(i), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight) // y2
  //    );
  //  }

  // Plot data
  int dataRange = min(data.size(), dataSize) - 1;
  for (int i = 0; i < dataRange; i++) {
    line(
    map(i, 0, dataRange, originX, originX + plotWidth), // x1
    map(data.get(i), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight), // y1
    map(i+1, 0, dataRange, originX, originX + plotWidth), // x2
    map(data.get(i+1), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight) // y2
    );
  }
}

void drawPlot(ArrayList<Integer> data, int originX, int originY, int plotWidth, int plotHeight, int plotRangeFloor, int plotRangeCeiling) {

  // Plot data
  for (int i = 0; i < data.size() - 1; i++) {
    line(
    map(i, 0, data.size() - 1, originX, originX + plotWidth), 
    map(data.get(i), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight), 
    map(i+1, 0, data.size() - 1, originX, originX + plotWidth), 
    map(data.get(i+1), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight)
      );
  }
}

void drawPlotNodes(int divisions, ArrayList<Integer> data, int originX, int originY, int plotWidth, int plotHeight, int plotRangeFloor, int plotRangeCeiling) {

  // maximumSampleSize
  int currentDivision = 0;
  int lastDivisionValue = 0;

  for (int i = 0; i < data.size(); i++) {

    //if (i == ((i + 1) * (data.size() / divisions))) {
    if (i == floor((currentDivision + 1) * (data.size() / (divisions - 1)))) {

      // Draw circle over key moment used for gesture classification (recognition)
      fill(255, 255, 255, 0);
      ellipse(
      map(i, 0, maximumSampleSize[gestureIndex], originX, originX + plotWidth), 
      map(data.get(i), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight), 
      20, 
      20
        );

      currentDivision++;
    }
  }
}

void drawPlot(int[] data, int originX, int originY, int plotWidth, int plotHeight, int plotRangeFloor, int plotRangeCeiling) {
  // Draw lines connecting all points
  for (int i = 0; i < data.length-1; i++) {
    // stroke(255,0,0);
    // strokeWeight(1);
    line(
    map(i, 0, data.length, originX, originX + plotWidth), 
    map(data[i], plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight), 
    map(i+1, 0, data.length, originX, originX + plotWidth), 
    map(data[i+1], plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight)
      );
  }
}

void drawPlot(float[] data, int originX, int originY, int plotWidth, int plotHeight, float plotRangeFloor, float plotRangeCeiling) {
  // Draw lines connecting all points
  for (int i = 0; i < data.length-1; i++) {
    // stroke(255,0,0);
    // strokeWeight(1);
    line(
    map(i, 0, data.length, originX, originX + plotWidth), 
    map(data[i], plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight), 
    map(i+1, 0, data.length, originX, originX + plotWidth), 
    map(data[i+1], plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight)
      );
  }
}

void drawValidationStatistics() {
  
  // Compute averages
  ArrayList<Float> gestureClassificationScore = new ArrayList<Float>();
  
  // Get most-recent live gesture data
  getGestureSamples();
  for (int i = 0; i < getGestureCount(); i++) {
    if (i != gestureIndex) {
      gestureClassificationScore.add(0.0);
      continue; // Only compute accuracy for current gesture (for now)
    }
    
    ArrayList<ArrayList<ArrayList<Integer>>> gestureExamples = getGestureSamples(i);
    
    // Add counter to compute classification score
    int correctClassificationCount = 0;
    
    for (int j = 0; j < gestureExamples.size(); j++) {
      ArrayList<ArrayList<Integer>> gestureExample = gestureExamples.get(j);
      
      int classifiedIndex = classifyGesture(gestureExample);
      // int classifiedIndex = classifyGestureFromTransitions(gestureExample);
      
      // Compute classification score
      if (classifiedIndex == i) {
        correctClassificationCount = correctClassificationCount + 1;
      }
    }
    
    // Add gesture classification percentage correct
    gestureClassificationScore.add(((float) correctClassificationCount) / (float) gestureExamples.size());
    
    //println("Accuracy " + i + ": " + gestureClassificationScore.get(gestureClassificationScore.size() - 1));
  }
  
  int scaledBoundLeft = int((float(gestureSignatureOffset[gestureIndex]) / float(maximumSampleSize[gestureIndex])) * width);
  int scaledBoundRight = int((float(gestureSignatureOffset[gestureIndex] + gestureSignatureSize[gestureIndex]) / float(maximumSampleSize[gestureIndex])) * width);
  
  // Draw beginning bound of gesture signature
  fill(0); 
  textFont(boundaryLabelFont); 
  textAlign(LEFT);
  text("\n\n\naccuracy " + gestureClassificationScore.get(gestureIndex), scaledBoundLeft + 8, 30);
}

void drawOrientationVisualization() {
  
  int scaledBoundLeft = int((float(gestureSignatureOffset[gestureIndex]) / float(maximumSampleSize[gestureIndex])) * width);
  int scaledBoundRight = int((float(gestureSignatureOffset[gestureIndex] + gestureSignatureSize[gestureIndex]) / float(maximumSampleSize[gestureIndex])) * width);
  
  int scaledCenter = int((float(scaledBoundRight) + float(scaledBoundLeft)) / 2.0);
  
  pushMatrix(); 
  
  //translate(width/2, height/2, -30);
 //translate(scaledCenter, height - 100, -30); 
 translate((width / 2) - 300, (height / 4) - 70);
  
  // Rotate
  rotateX(pitch); // rotateX(((float)pitch)*-PI/180.0); 
  rotateY(yaw); // rotateY(((float)yaw)*-PI/180.0); 
  rotateZ(roll); // rotateZ(((float)roll)*-PI/180.0);  
  
  // Draw X, Y, and Z axes
  strokeWeight(1);
  stroke(255, 0, 0); line(-100, 0, 0, 100, 0, 0); // X axis
  stroke(0, 255, 0); line(0, -100, 0, 0, 100, 0); // Y axis 
  stroke(0, 0, 255); line(0, 0, -100, 0, 0, 100); // Z axis
  
  // Draw X, Y, and Z acceleration vectors
  strokeWeight(4);
  stroke(255, 0, 0); line(-50, 0, 0, 50, 0, 0); // X axis
  stroke(0, 255, 0); line(0, -50, 0, 0, 50, 0); // Y axis 
  stroke(0, 0, 255); line(0, 0, -50, 0, 0, 50); // Z axis
  
  scale(30);
  
  beginShape(QUADS);
  
  stroke(0, 0, 0);
  strokeWeight(0);
  
  fill(0, 255, 0); vertex(-1,  1,  1);
  fill(0, 255, 0); vertex( 1,  1,  1);
  fill(0, 255, 0); vertex( 1, -1,  1);
  fill(0, 255, 0); vertex(-1, -1,  1);
  
  fill(0, 255, 255); vertex( 1,  1,  1);
  fill(0, 255, 255); vertex( 1,  1, -1);
  fill(0, 255, 255); vertex( 1, -1, -1);
  fill(0, 255, 255); vertex( 1, -1,  1);
  
  fill(255, 0, 255); vertex( 1,  1, -1);
  fill(255, 0, 255); vertex(-1,  1, -1);
  fill(255, 0, 255); vertex(-1, -1, -1);
  fill(255, 0, 255); vertex( 1, -1, -1);
  
  fill(255, 255, 0); vertex(-1,  1, -1);
  fill(255, 255, 0); vertex(-1,  1,  1);
  fill(255, 255, 0); vertex(-1, -1,  1);
  fill(255, 255, 0); vertex(-1, -1, -1);
  
  fill(255, 0, 0); vertex(-1,  1, -1);
  fill(255, 0, 0); vertex( 1,  1, -1);
  fill(255, 0, 0); vertex( 1,  1,  1);
  fill(255, 0, 0); vertex(-1,  1,  1);
  
  fill(0, 0, 255); vertex(-1, -1, -1);
  fill(0, 0, 255); vertex( 1, -1, -1);
  fill(0, 0, 255); vertex( 1, -1,  1);
  fill(0, 0, 255); vertex(-1, -1,  1);
  
  endShape();
  
  popMatrix();
}
