 #ifndef TeensyPlayer_H
#define TeensyPlayer_H

#include <Bounce.h>
#include <Audio.h>

class TeensyPlayer {
  private:
    int startStopEnable = 0;
    int rangeIndex;
    int rangeInit = 0;
    int rangeStart = 0;
    int rangeSCindex;  // static variable are like global variable but "private" to this function
    int rangeSCinit = 0;
    unsigned int lastSCmillis;
    int rangeSCStart = 0;
    int readings[20];
    int readIndex = 0;
    int total = 0;
    int average = 0;
    int smoothInit = 0;
    const int numReadings = 20;
    int UpDownRangeIndex;
    int UpDownRangePlayEnable;

  public:
    static int totalPlaying;

    void playAfileInAnArray(int index, const char* arrayName[], int maxFilenameIndex, Bounce* theButton, AudioPlaySdWav* SdWaveplayer );

    void playAFileWhileButtonDown(int index, const char* arrayName[], int maxFilenameIndex, Bounce* theButton, AudioPlaySdWav* SdWaveplayer);

    void startStopPlayAfileInAnArray(int index, const char* arrayName[], int maxFilenameIndex, Bounce * buttonName, AudioPlaySdWav * SdWaveplayer);

    void playRangeOfFilesInAnArray(int startIndex, int endIndex, const char* arrayName[], int maxFilenameIndex, Bounce * buttonName, AudioPlaySdWav * SdWaveplayer);
    
    void playRangeControlledByUpDownButtons(int startIndex, int endIndex, const char* arrayName[], int maxFilenameIndex, Bounce *downButtonName, Bounce *upButtonName, AudioPlaySdWav *SdWaveplayer);

    void playRangeStartStopWithSpeedControl(int startIndex, int endIndex, const char* arrayName[], int maxFilenameIndex, Bounce *buttonName,
        AudioPlaySdWav * SdWaveplayer, int speedMS);

    void playRangeOfFilesWithSpeedControl(int startIndex, int endIndex, const char* arrayName[], int maxFilenameIndex, Bounce *buttonName,
                                           AudioPlaySdWav *SdWaveplayer, int speedMS);

};

#endif
