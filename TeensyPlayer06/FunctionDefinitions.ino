void playAfileInAnArray(char* arrayName[], int maxFilenameIndex, Bounce* theButton, AudioPlaySdWav* SdWaveplayer, int index) {
  
  if (theButton->fallingEdge()) {   // will play once after the button is released - try it!
    // defensive programming - check params and make sure they are valid
    if (index >= maxFilenameIndex) {
      Serial.println("playAfileInAnArray(), index is invalid ");
      return;  // bail out
    }
    // if (!playSdWav2.isPlaying()) {          // comment this back in if you don't want to interupt the file (retrigger)
    playSdWav2.play(arrayName[index]);
    delay(3); // short delay seems to be necessary or it skips files
    // }                                       // comment this back in if you don't want to interupt the file (retrigger)
  }
}

void playAfileInAnArray2(char* arrayName[], int maxFilenameIndex, Bounce* theButton, AudioPlaySdWav* SdWaveplayer, int index) {
  // uses button 1, playSdWav2 - change (two places!) to suit your needs

  if (theButton->fallingEdge()) {   // will play once after the button is released - try it!
    // defensive programming - check params and make sure they are valid
    if (index >= maxFilenameIndex) {
      Serial.println("playAfileInAnArray(), index is invalid ");
      return;  // bail out
    }
    // if (!playSdWav2.isPlaying()) {          // comment this back in if you don't want to interupt the file (retrigger)
    playSdWav2.play(arrayName[index]);
    delay(3); // short delay seems to be necessary or it skips files
    // }                                       // comment this back in if you don't want to interupt the file (retrigger)
  }
}

void playAfileInAnArray3(char* arrayName[], int maxFilenameIndex, Bounce* theButton, AudioPlaySdWav* SdWaveplayer, int index) {
  // uses button 1, playSdWav2 - change (two places!) to suit your needs

  if (theButton->fallingEdge()) {   // will play once after the button is released - try it!
    // defensive programming - check params and make sure they are valid
    if (index >= maxFilenameIndex) {
      Serial.println("playAfileInAnArray(), index is invalid ");
      return;  // bail out
    }
    // if (!playSdWav2.isPlaying()) {          // comment this back in if you don't want to interupt the file (retrigger)
    playSdWav2.play(arrayName[index]);
    delay(3); // short delay seems to be necessary or it skips files
    // }                                       // comment this back in if you don't want to interupt the file (retrigger)
  }
}

void playAFileWhileButtonDown(char* arrayName[], int maxFilenameIndex, Bounce* theButton, AudioPlaySdWav* SdWaveplayer, int index) {
  // plays a file while the button is down - plays the whole file before repeating
  if (theButton->read() == 0) {   // will play once after the button is released - try it!

    // defensive programming - check params and make sure they are valid
    if (index >= maxFilenameIndex) {
      Serial.println("playAfileInAnArray(), index is invalid ");
      return;  // bail out
    }

    if (!SdWaveplayer->isPlaying()) {
      SdWaveplayer->play(arrayName[index]);
      delay(3); // short delay seems to be necessary or it skips files
    }
  }
  else {
    SdWaveplayer->stop();
  }
}


// void playAfileInAnArray(arrayName, maxFilenameIndex, &buttonName, &playSdWav, startFileIndex, endFileIndex)
void playRangeOfFilesInAnArray(char* arrayName[], int maxFilenameIndex, Bounce* buttonName, AudioPlaySdWav* SdWaveplayer, int startIndex, int endIndex) {
  // uses button3 and playSdWav3 change them if you wish - change playSdWav3 in two places

  static int index;  // static variable are like global variable but "private" to this function
  static int initialized = 0;
  static int rangeStart;

  if (buttonName -> fallingEdge()) {   // will play once after the button is pressed
    if (!initialized) { // if the first time, initalize the function

      // defensive programming - check file parameters and make sure they are valid
      if ((startIndex > endIndex) || (endIndex >= maxFilenameIndex)) {
        Serial.println(" playRangeOfFilesInAnArray() parameters startIndex > endIndex OR ");
        Serial.println(" endIndex >= maxFilenameIndex, illegal parameters! ");
        return;  // bail out
      }

      rangeStart = 1;   // this will keep the function playing once it has started - see above
      index = startIndex;
      initialized = 1;  // this insures it only starts once per range played
    }
  }

  if (rangeStart) {
    if (!SdWaveplayer->isPlaying()) { // wait for last file to stop
      Serial.print("index = ");
      Serial.println(index);
      SdWaveplayer->play(arrayName[index]);
      delay(2); // short delay seems to be necessary or it skips files

      if (index >= endIndex) { // all done, reset and bail out
        rangeStart = 0;  // comment out this line to play forever (or until a power cycle) once button is pushed
        initialized = 0;
        return;
      }
      index++;
    }
  }
}

void startStopPlayAfileInAnArray(char* arrayName[], int maxFilenameIndex, Bounce* buttonName, AudioPlaySdWav* SdWaveplayer, int index ) {
  // pushbutton plays one file continuously, next pushbutton stops the file, etc.
  static int startStopEnable;

  // start play on one press, stop play on another press (toggle)
  if (buttonName->fallingEdge()) {

    // defensive programming - check params and make sure they are valid
    if (index >= maxFilenameIndex) {
      Serial.println("startStopPlayAfileInAnArray(), index is invalid, index > maxFilenameIndex!");
      return;  // bail out
    }
    startStopEnable = ++startStopEnable % 2; // toggles between 0 and 1, " % 2 " is even / odd test
  }

  if (startStopEnable) {
    if (!SdWaveplayer->isPlaying()) {
      SdWaveplayer->play(arrayName[index]);
      delay(3); // short delay seems to be necessary or it skips files
    }
  }
  else {  // file should be stopped if the button has been pushed again to stop it.
    SdWaveplayer->stop();
  }
}

void startStopPlayAfileInAnArray2(char* arrayName[], int maxFilenameIndex, Bounce* buttonName, AudioPlaySdWav* SdWaveplayer, int index ) {
  // plays one file until pushbutton, next pushbutton starts the file again
  static int startStopEnable;

  // start play on one press, stop play on another press (toggle)
  if (buttonName->fallingEdge()) {

    // defensive programming - check params and make sure they are valid
    if (index >= maxFilenameIndex) {
      Serial.println("startStopPlayAfileInAnArray(), index is invalid, index > maxFilenameIndex!");
      return;  // bail out
    }
    startStopEnable = ++startStopEnable % 2; // toggles between 0 and 1
  }

  if (startStopEnable) {
    if (!SdWaveplayer->isPlaying()) {  // if the file has finished playing
      SdWaveplayer->play(arrayName[index]);  // play it again
      delay(3); // short delay seems to be necessary or it skips files
    }
  }
  else { // file should be stopped if the button has been pushed again to stop it.
    SdWaveplayer->stop();
  }
}

void startStopPlayAfileInAnArray3(char* arrayName[], int maxFilenameIndex, Bounce* buttonName, AudioPlaySdWav* SdWaveplayer, int index ) {
  // plays one file until pushbutton, next pushbutton starts the file again
  static int startStopEnable;

  // start play on one press, stop play on another press (toggle)
  if (buttonName->fallingEdge()) {

    // defensive programming - check params and make sure they are valid
    if (index >= maxFilenameIndex) {
      Serial.println("startStopPlayAfileInAnArray(), index is invalid, index > maxFilenameIndex!");
      return;  // bail out
    }
    startStopEnable = ++startStopEnable % 2; // toggles between 0 and 1
  }

  if (startStopEnable) {
    if (!SdWaveplayer->isPlaying()) {  // if the file has finished playing
      SdWaveplayer->play(arrayName[index]);  // play it again
      delay(3); // short delay seems to be necessary or it skips files
    }
  }
  else { // file should be stopped if the button has been pushed again to stop it.
    SdWaveplayer->stop();
  }
}


void playRangeOfFilesWithSpeedControl(char* arrayName[], int maxFilenameIndex, Bounce* buttonName,
                                      AudioPlaySdWav* SdWaveplayer, int speedMS,  int startIndex, int endIndex) {

  static int index;  // static variable are like global variable but "private" to this function
  static int initialized = 0;
  static unsigned int lastSCmillis;

  if (buttonName->fallingEdge()) {

    if (!initialized) { // if the first time, initalize the function

      // defensive programming - check params and make sure they are valid
      if ((startIndex > endIndex) || (endIndex >= maxFilenameIndex)) {
        Serial.println(" playRangeOfFilesWithSpeedControl parameters startIndex > endIndex OR ");
        Serial.println(" endIndex >= maxFilenameIndex, illegal parameters! ");
        return;  // bail out
      }

      rangeSCStart = 1;   // this will keep the function playing once it has started
      index = startIndex; // set the index to the starting file
      initialized = 1;    // initialize only once per range played
    }
  }

  if (millis() - lastSCmillis > speedMS) { // check the time and play next file
    //    Serial.print("index = ");
    //    Serial.println(index);
    SdWaveplayer->play(arrayName[index]);
    delay(2); // short delay seems to be necessary or it skips files

    if (index >= endIndex) { // all done, reset and bail out
      rangeSCStart = 0;      // comment this out for infinite play after first button push
      initialized = 0;
      return;
    }

    lastSCmillis = millis();
    index++;
  }
}


void playRangeStartStopWithSpeedControl( char* arrayName[], int maxFilenameIndex, Bounce * buttonName,
    AudioPlaySdWav * SdWaveplayer, int speedMS, int startIndex, int endIndex) {

  // This function will start with a button press and stop with a button press. It plays a a range of files
  // continuously until the button is pressed again. Speed is governed by the speedMS parameter passed in.

  static int index;  // static variable are like global variables but "private" to this function
  static int initialized = 0;
  static int rangeSCStart;
  static unsigned int lastSCmillis;

  // start stop play range with speed control. Will continuously play after button press until the next button press
  if (buttonName->risingEdge()) {  // uses button 2 OK to change to another

    // defensive programming - check params and make sure they are valid
    if ((startIndex > endIndex) || (endIndex >= maxFilenameIndex)) {
      Serial.println(" playRangeOfFilesWithSpeedControl parameters startIndex > endIndex OR ");
      Serial.println(" endIndex >= maxFilenameIndex, illegal parameters! ");
      return;  // bail out
    }

    rangeSCStart = ++rangeSCStart % 2; // toggles between 0 and 1 - " % 2" is like an even / odd test
  }

  if (rangeSCStart) {  // 1 is true and will play, 0 is not true

    if (!initialized) { // if the first time, initalize the function
      index = startIndex;
      initialized = 1;  // this insures it only starts once per range played
    }

    if (millis() - lastSCmillis > speedMS) { // wait for last file to stop

      if (index == endIndex + 1) { // the timing is tricky - we need to test
        initialized = 0;           // for endIndex + 1 so the last file gets time to play
        return;
      }

      //      Serial.print("index = ");
      //      Serial.println(index);
      SdWaveplayer->play(arrayName[index]);
      delay(2);  // short delay seems to be necessary or it skips files
      lastSCmillis = millis();
      index++;
    }
  }
  else {  // stop immediately if the play button is toggled
    SdWaveplayer->stop();
  }
}

// this function will interrupt files that are playing, when button pressed
void playRangeControlledByUpDownButtons( char* arrayName[], int maxFilenameIndex,
    Bounce * downButton, Bounce * upButton, AudioPlaySdWav* SdWaveplayer, int startIndex, int endIndex) {

  static int index = startIndex;
  static int playEnable = 0;

  if (upButton->fallingEdge()) { // up - this is governed by UpDown parameter passed in.

    // defensive programming - check params to make sure they are valid
    if ((startIndex > endIndex) || (endIndex >= maxFilenameIndex)) {
      Serial.println(" playRangeControlledByUp_DownButtons parameters are invalid ");
      Serial.println(" startIndex > endIndex OR endIndex >= maxFilenameIndex, parameters illegal! ");
      return;  // bail out
    }
    index++;
    playEnable = 1;
  }
  else if (downButton->fallingEdge()) { // down
    index--;
    playEnable = 1;
  }

  if (playEnable) {
    index = constrain(index, startIndex, endIndex);  // stay at endpoint index when you get there - might rollover as an alternative
    //    Serial.print("index = ");
    //    Serial.println(index);
    SdWaveplayer -> play(arrayName[index]);  // note I used another SDcard player
    playEnable = 0;   // comment this line out for infinite play once button is pushed
  }
}

// you must uncomment #define ACCELEROMETER at the top of the first tabe of teh sketch to use this function
#ifdef ACCELEROMETER
void playRangeControlledByAccelerometer( char* arrayName[], int maxFilenameIndex,
    AudioPlaySdWav* SdWaveplayer, int accelVal, int startIndex, int endIndex) {

  static int lastIndex;
  static int lastMillis;
  int index;

  // the accelerometer is really jittery - especially if shaken even mildly
  // some extensive signal processing has been used here.
  // first the sensor value is smoothed - using a smoothing function -
  // you can see how this works in an Arduino Examples->Analog->Smoothing sketch
  // I just turned it into a function for easy use.


  int  smoothedAccel = smoothed(accelVal);  // smoothing filter see the function definition
  /*
    if (abs(smoothedAccel - lastAccel) < 1000) {  // value must change at least 1000 before it registers
      smoothedAccel = lastAccel;
    }
    else {   // we have a valid move - update and play the file
  */
  //  Serial.print(" smoothedAccel ");
  //  Serial.print(smoothedAccel );
  //  Serial.print(" \t ");
  //  Serial.print(lastAccel );
  //  Serial.println(" index " );
  //  Serial.println(index);
  lastAccel = smoothedAccel;

  // Serial.println(digitalRead(buttonPin));
  index = map(smoothedAccel, -7300, 7300, startIndex, endIndex);   // lis.x is accelerometer data
  index = constrain(index, startIndex, endIndex);                  // prevent overrun values

  if (index != lastIndex) { // play each track only once
    if (millis() - lastMillis > 1000) {  // limit playback speed to limit jittery playback
      //                                   // change this to degree of responsiveness desired

      // if you want to play entire tracks  use the following line instead  of the above line
      // if (!SdWaveplayer ->isPlaying()){

      Serial.print("  index = ");
      Serial.println(index);
      SdWaveplayer -> play(arrayName[index]);  // note I used another SDcard player
      lastIndex = index;
      lastMillis = millis();
    }
  }
}
#endif

