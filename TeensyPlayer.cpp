#include "TeensyPlayer.h"
#include <Bounce.h>
#include <Audio.h>

void TeensyPlayer::playAfileByTitle(const char fileName[], Bounce* theButton, AudioPlaySdWav* SdWaveplayer) {
    // plays a wav file by passing in the title
    // will retrigger (start the file from beginning) if the button is pressed again before the file is done
    
    if (theButton->fallingEdge()) {   // will play once after the button is released - try it!
        // if (!SdWaveplayer->isPlaying()) {        // comment this back in if you don't want to interupt the file (retrigger)
        SdWaveplayer->play(fileName);
        delay(3); // short delay seems to be necessary or it skips files
        // }                                       // comment this back in if you don't want to interupt the file (retrigger)
    }
}

void TeensyPlayer::playAfileInAnArray(int index, const char* arrayName[], int maxFilenameIndex, Bounce* theButton, AudioPlaySdWav* SdWaveplayer) {
    // uses button 1, playSdWav2 - change (two places!) to suit your needs
    
    if (theButton->fallingEdge()) {   // will play once after the button is released - try it!
        // defensive programming - check params and make sure they are valid
        if (index >= maxFilenameIndex) {
            Serial.println("playAfileInAnArray(), index is invalid ");
            return;  // bail out
        }
        // if (!playSdWav2.isPlaying()) {          // comment this back in if you don't want to interupt the file (retrigger)
        SdWaveplayer->play(arrayName[index]);
        delay(3); // short delay seems to be necessary or it skips files
        // }                                       // comment this back in if you don't want to interupt the file (retrigger)
    }
}

void  TeensyPlayer::startStopPlayAfileInAnArray(int index, const char* arrayName[], int maxFilenameIndex, Bounce* theButton, AudioPlaySdWav* SdWaveplayer) {
    // pushbutton plays one file continuously, next pushbutton stops the file, etc.
    
    // start play on one press, stop play on another press (toggle)
    if (theButton->fallingEdge()) {
        
        // defensive programming - check params and make sure they are valid
        if (index >= maxFilenameIndex) {
            Serial.println("startStopPlayAfileInAnArray(), index is invalid, index > maxFilenameIndex!");
            return;  // bail out
        }
        startStopEnable = !startStopEnable;  // toggles between 0 and 1, " % 2 " is even / odd test
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
};

void TeensyPlayer::playAFileWhileButtonDown(int index, const char* arrayName[], int maxFilenameIndex, Bounce* theButton, AudioPlaySdWav* SdWaveplayer) {
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
};

void TeensyPlayer::playRangeOfFilesInAnArray(int startIndex, int endIndex, const char* arrayName[], int maxFilenameIndex, Bounce* buttonName,
                                             AudioPlaySdWav* SdWaveplayer) {
    
    if (buttonName -> fallingEdge()) {   // will play once after the button is pressed
        if (!rangeInit) { // if the first time, initalize the function
            
            // defensive programming - check file parameters and make sure they are valid
            if ((startIndex > endIndex) || (endIndex >= maxFilenameIndex)) {
                Serial.println(" playRangeOfFilesInAnArray() parameters startIndex > endIndex OR ");
                Serial.println(" endIndex >= maxFilenameIndex, illegal parameters! ");
                return;  // bail out
            }
            
        
            
            
            if (rangeStart) {
                if (!SdWaveplayer->isPlaying()) { // wait for last file to stop
                    //          Serial.print("index = ");
                    //          Serial.println(rangeIndex);
                    SdWaveplayer->play(arrayName[rangeIndex]);
                    delay(2); // short delay seems to be necessary or it skips files
                    
                    if (rangeIndex >= endIndex) { // all done, reset and bail out
                        rangeStart = 0;  // comment out this line to play forever (or until a power cycle) once button is pushed
                        rangeInit = 0;
                        return;
                    }
                    rangeIndex++;
                }
            }
            
            
            rangeIndex = startIndex;
            rangeInit = 1;  // this insures it only starts once per range played
        }
    }
};

// this function will interrupt files that are playing, when button pressed
void TeensyPlayer::playRangeControlledByUpDownButtons(int startIndex, int endIndex, const char* arrayName[], int maxFilenameIndex,
                                                      Bounce * downButton, Bounce * upButton, AudioPlaySdWav* SdWaveplayer) {
    
    if (upButton->fallingEdge()) { // up - this is governed by UpDown parameter passed in.
        
        // defensive programming - check params to make sure they are valid
        if ((startIndex > endIndex) || (endIndex >= maxFilenameIndex)) {
            Serial.println(" playRangeControlledByUp_DownButtons parameters are invalid ");
            Serial.println(" startIndex > endIndex OR endIndex >= maxFilenameIndex, parameters illegal! ");
            return;  // bail out
        }
        UpDownRangeIndex++;
        UpDownRangePlayEnable = 1;
    }
    else if (downButton->fallingEdge()) { // down
        UpDownRangeIndex--;
        UpDownRangePlayEnable = 1;
    }
    
    if (UpDownRangePlayEnable) {
        // UpDownRangeIndex = constrain(UpDownRangeIndex, startIndex, endIndex);  // stay at endpoint index when you get there
        //  rollover as an alternative below
        if (UpDownRangeIndex > endIndex) UpDownRangeIndex = startIndex;
        if (UpDownRangeIndex < startIndex) UpDownRangeIndex = endIndex;
        //    Serial.print("index = ");
        //    Serial.println(index);
        SdWaveplayer -> play(arrayName[UpDownRangeIndex]);  // note I used another SDcard player
        UpDownRangePlayEnable = 0;   // comment this line out for infinite play once button is pushed
    }
}

void TeensyPlayer::playRangeOfFilesWithSpeedControl(int startIndex, int endIndex, const char* arrayName[], int maxFilenameIndex, Bounce* buttonName,
                                                    AudioPlaySdWav* SdWaveplayer, int speedMS) {
    // plays through a range of files - but only once - until the next button press
    // The time each file can play is limited by the speedMS parameter passed in
    
    if (buttonName->fallingEdge()) {
        
        if (!rangeSCinit) { // if the first time, initalize the function
            
            // defensive programming - check params and make sure they are valid
            if ((startIndex > endIndex) || (endIndex >= maxFilenameIndex)) {
                Serial.println(" playRangeOfFilesWithSpeedControl parameters startIndex > endIndex OR ");
                Serial.println(" endIndex >= maxFilenameIndex, illegal parameters! ");
                return;  // bail out
            }
            
            rangeSCStart = 1;   // this will keep the function playing once it has started
            rangeSCindex = startIndex; // set the index to the starting file
            rangeSCinit = 1;    // initialize only once per range played
        }
    }
    
    if (millis() - lastSCmillis > speedMS) { // check the time and play next file
        //    Serial.print("index = ");
        //    Serial.println(index);
        SdWaveplayer->play(arrayName[rangeSCindex]);
        delay(2); // short delay seems to be necessary or it skips files
        
        if (rangeSCindex >= endIndex) { // all done, reset and bail out
            rangeSCStart = 0;      // comment this out for infinite play after first button push
            rangeSCinit = 0;
            return;
        }
        
        lastSCmillis = millis();
        rangeSCindex++;
    }
}

void TeensyPlayer::playRangeStartStopWithSpeedControl(int startIndex, int endIndex, const char* arrayName[], int maxFilenameIndex, Bounce * buttonName,
                                                      AudioPlaySdWav * SdWaveplayer, int speedMS) {
    
    // This function will start with a button press and stop with a button press. It plays a range of files
    // continuously until the button is pressed again. Length of play on each file is governed by the speedMS parameter passed in.

    // start stop play range with speed control. Will continuously play range after button press until the next button press
    if (buttonName->risingEdge()) {  // uses button 2 OK to change to another
        
        // defensive programming - check params and make sure they are valid
        if ((startIndex > endIndex) || (endIndex >= maxFilenameIndex)) {
            Serial.println(" playRangeOfFilesWithSpeedControl parameters startIndex > endIndex OR ");
            Serial.println(" endIndex >= maxFilenameIndex, illegal parameters! ");
            return;  // bail out
        }
        
        playRangeStartStop_rangeSCStart = !playRangeStartStop_rangeSCStart; // toggles between 0 and 1 - " % 2" is like an even / odd test
    }
    
    if (playRangeStartStop_rangeSCStart) {  // 1 is true and will play, 0 is not true
        
        if (!playRangeStartStop_initialized) { // if the first time, initalize the function
            playRangeStartStop_index = startIndex;
            playRangeStartStop_initialized = 1;  // this insures it only starts once per range played
        }
        
        if (millis() - playRangeStartStop_lastSCmillis > speedMS) { // wait for last file to time out with speedMS parameter
            if (playRangeStartStop_index == endIndex + 1) { // the timing is tricky - we need to test
                playRangeStartStop_initialized = 0;         // for endIndex + 1 so the last file gets time to play
                return;
            }
            
            //      Serial.print("index = ");
            //      Serial.println(index);
            SdWaveplayer->play(arrayName[playRangeStartStop_index]);
            delay(2);  // short delay seems to be necessary or it skips files
            playRangeStartStop_lastSCmillis = millis();
            playRangeStartStop_index++;
        }
    }
    else {  // stop immediately if the play button is toggled
        SdWaveplayer->stop();
    }
}

void TeensyPlayer::playFileOnceUntilIndexChanges(int index, const char* arrayName[], int maxFilenameIndex, AudioPlaySdWav *SdWaveplayer){
    
    // plays a file once until the function is called with a new index
    // will not repeat files if the sensor hasn't changed to a new position / input
    
    playFileOnce_index = index;
    
    // start stop play range with speed control. Will continuously play after button press until the next button press
    if (playFileOnce_index != playFileOnce_lastIndex  ) {  // compare to last time - to play file only once
        
        // defensive programming - check params and make sure they are valid
        if ((playFileOnce_index <  0) || (playFileOnce_index >= maxFilenameIndex)) {
            Serial.println(" playFileOnceUntilIndexChanges() index parameters illegal!");
            return;  // function bails out
        }
        
        playFileOnceStart = 1; // toggles between 0 and 1 - " % 2" is like an even / odd test
    }
    
    if (playFileOnceStart) {  // 1 is true and will play, 0 is not true will skip this
        if (SdWaveplayer->isPlaying()) SdWaveplayer->stop(); // stop file if playing
        
        //      Serial.print("index = ");
        //      Serial.println(playFileOnce_index);
        
        SdWaveplayer->play(arrayName[playFileOnce_index]);
        delay(2);  // short delay seems to be necessary or it skips files
        playFileOnce_lastIndex = playFileOnce_index;
        playFileOnceStart = 0;
    }
}








