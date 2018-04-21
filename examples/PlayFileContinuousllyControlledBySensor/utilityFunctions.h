float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void readSensors() {
  static unsigned int lastMillis = millis(); // for Teensy - use a long data type for Arduino board

  volumePotVal = analogRead(volumePotPin);  // comment out if you don't have volume control
  controlPotVal = analogRead(controlPotPin);  // comment out if you don't have control pot

#ifdef DEBUG_POTS
  Serial.print(" pot values ");
  Serial.print(volumePotVal);
  Serial.print("\t");
  Serial.println(controlPotVal);
  delay(3);
#endif

  // the volume level doesn' like to be set too often
  // Using millis() here to time out a reasonable interval
  if (millis() - lastMillis > 5) { // every 5 milliseconds
    float volumeLvl = map((float)volumePotVal, 0.0, 1023.0, 0.0, 1.0);
    sgtl5000_1.volume(volumeLvl);
    lastMillis = millis();
  }


#ifdef ACCELEROMETER
  lis.read();      // get X Y and Z data at once
  Then print out the raw accelerometer data
  Serial.print("X:  "); Serial.print(lis.x); Serial.print("  \tY:  ");
  Serial.print(lis.y);  Serial.print("  \tZ:  "); Serial.println(lis.z);
#endif


  button1->update(); // the arrow operator defreferences the button1 pointers
  button2->update();
  button3->update();
  /*  debug switches */

#ifdef DEBUG_BUTTONS
  Serial.println(button1->read());
  Serial.print("\t");
  Serial.print(button2->read());
  Serial.print("\t");
  Serial.println(button3->read());
  delay(3);  // use this with debugging then comment out again for faster sketch execution
#endif

}

int smooth(int input) {
  const int numReadings = 20;

  // Define the number of samples to keep track of. The higher the number, the
  // more the readings will be smoothed, but the slower the output will respond to
  // the input. Using a constant rather than a normal variable lets us use this
  // value to determine the size of the readings array.

  static int readings[20];               // the readings from the analog input
  static int readIndex = 0;              // the index of the current reading
  static int total = 0;                  // the running total
  int average = 0;                       // the average
  static int initialized = 0;

  if (!initialized) {  // make sure array is zeroed out
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings[thisReading] = 0;
      initialized = 1;
    }
  }

  // this code has one clever trick in it - normally to get an average of twenty elements,
  // we would add up all the elements, and then divide by twenty.
  // But because we know what all the elements are - we can keep track of a running total without
  // having to add up all the elements. Insted we just swap out the one value that changes each time.
  // This saves the computer from having to add up all the elements each time through the loop

  if (initialized) {
    total = (total - readings[readIndex]); // subtract the last reading:
    readings[readIndex] = input;             // input new data into the array
    total = total + readings[readIndex];     // add the reading to the total:
    readIndex = readIndex + 1;               // advance to the next position in the array:

    // if we're at the end of the array...
    if (readIndex >= numReadings) {
      // ...wrap around to the beginning:
      readIndex = 0;
    }

    // calculate the average:
    average = total / numReadings;

#ifdef DEBUG_SMOOTH
    send it to the computer as ASCII digits
    Serial.print("input " );      Serial.print(input);
    Serial.print("  total " );    Serial.print(total);
    Serial.print("  average = "); Serial.println(average);
    delay(3);        // delay in between reads for stability
#endif

    return average;
  }
  else {
    return input; // this should only happen once
  }
}


