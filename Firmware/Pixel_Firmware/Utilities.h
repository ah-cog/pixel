#ifndef UTILITIES_H
#define UTILITIES_H

String getValue (String data, char separator, int index) {
  
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
        found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring (strIndex[0], strIndex[1]) : "";
}

String getValueRest (String data, char separator, int index) {
  
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
        found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring (strIndex[0]) : "";
}

int getValueCount (String data, char separator) {
  
  int count = 0;
  int maxIndex = data.length() - 1;

  for (int i = 0; i < data.length (); i++) {
    if (data.charAt(i) == separator) {
      count++;
    }
  }
  
  if (count > 0) {
    count = count + 1;
  }
  
  if (count == 0 && data.length() > 0) {
    count = 1;
  }

  return count;
}

#endif
