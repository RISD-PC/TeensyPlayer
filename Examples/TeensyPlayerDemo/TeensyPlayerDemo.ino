/* Wav File Player Title Name Array And Button Tutorial
    Plays files from an array of file names
    The Teensy Audio Shield uses pins 6,7,9,11,12,13,14,18,19,22,23
    This means that you may use pins 0, 1, 2, 3, 4, 5, 8, 16, 17, 20, 21, A10, A11.
    Additionally, there are lots more pins on the bottom side of the Teensy board
    but you'll have to solder (stranded!) wires on them and bring them out to the breadboard.
    
    For the Teensy & Teensy Audio Shield WAV player example
    Hardware: 
    Teensy Audio Shield, Teensy 3.2 or similar
    
    Two 10k (104) pots, outside leads connected to ground and +3.3V
    Wipers connected to pins 16 & 17
    
    3 pushbuttons connected to pins 0, 1, 2
    ****   Important - switches wired with ONLY one wire to ground ****
    ****   The switch connects input to ground when pushed         ****
    ****   NO PULLDOWN RESISTORS SHOULD BE ADDED!                  ****
 
    Plays a single file for hardware check. 
*/

// make sure these libraries are only included once!
#include <Audio.h>
#include <Wire.h>
#include <SerialFlash.h>
#include <Bounce.h>"
#include <SPI.h>
#include <SD.h>
#include "TeensyPlayer.h"
// #include <Adafruit_LIS3DH.h> // uncomment for accelerometer
// #include <Adafruit_Sensor.h>


// define ACCELEROMETER  // uncomment this to use the accelerometer

#ifdef ACCELEROMETER
#define LIS3DH_CS 10
Adafruit_LIS3DH lis = Adafruit_LIS3DH();
#endif

// GUItool: begin automatically generated code

AudioPlaySdWav           playSdWav1;     //xy=89,39.00000762939453
AudioPlaySdWav           playSdWav2;     //xy=90,82.00000762939453
AudioPlaySdWav           playSdWav3;     //xy=86,146
AudioPlaySdWav           playSdWav4;     //xy=86,146
AudioMixer4              mixer2;         //xy=275,143
AudioPlaySdRaw           playSdRaw1;     //xy=261.1666717529297,4109.16667175293
AudioMixer4              mixer1;         //xy=281,64
AudioPlayMemory          playMem1;       //xy=305.1666717529297,4228.16667175293
AudioMixer4              mixer3;         //xy=424,91.00000762939453
AudioOutputI2S           i2s1;           //xy=590,75AudioConnection          patchCord1(playSdWav3, 0, reverb1, 0);
AudioConnection          patchCord2(playSdWav3, 0, mixer2, 0);
AudioConnection          patchCord4(playSdWav3, 1, mixer2, 1);
AudioConnection          patchCord15(playSdWav4, 0, mixer2, 2);
AudioConnection          patchCord16(playSdWav4, 1, mixer2, 3);
AudioConnection          patchCord5(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord6(playSdWav1, 1, mixer1, 1);
AudioConnection          patchCord7(playSdWav2, 0, mixer1, 2);
AudioConnection          patchCord8(playSdWav2, 1, mixer1, 3);
AudioConnection          patchCord11(mixer2, 0, mixer3, 1);
AudioConnection          patchCord12(mixer1, 0, mixer3, 0);
AudioConnection          patchCord13(mixer3, 0, i2s1, 0);
AudioConnection          patchCord14(mixer3, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=427,172
// GUItool: end automatically generated code

// Use these with the audio adaptor board
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

const int volumePotPin = 15;  // change to the pin you are using for volume control pot
const int controlPotPin = 16; // for more pots use pins 17, 20, 21, A10, A11
const int buttonPin1 = 0;     // next to the GND pin on the Teensy
const int buttonPin2 = 1;
const int buttonPin3 = 2;

// I used the button class to do button debouncing and management
Bounce butt1 = Bounce(buttonPin1, 10 );  // create instances of the bounce class
Bounce butt2 = Bounce(buttonPin2, 10 );
Bounce butt3 = Bounce(buttonPin3, 10 );

Bounce *button1;   // create some pointers to the instances - makes coding easier / more clear
Bounce *button2;
Bounce *button3;

int lastVolumePotVal;
int fileIndex;
unsigned int lastPlayMillis;  //use a long for Arduino Uno, Mega etc
int volumePotVal;
int controlPotVal;
int rangeStart = 0, rangeSCStart = 0, startStopEnable = 0, playArrayStart = 0;
int accel, lastAccel, track;

// file names are 8.3 format - eight characters only, '.' , "wav" extension

char* fileNameArray[] =
{ "one.wav", "Click.wav", "Click.wav", "Whaan.wav", "SshSsh.wav", // break lines like this
  "Whaan.wav", "Click.wav", "Click.wav", "SshSsh.wav", "Click.wav"  // use your own titles!
};

// This is an example of a two-dimensional array
// The first dimension is the letters (chars) in the title,
// The second dimmension (leftmost) is the strings of the title, in which we are most interested.

int maxFilenameIndex = (sizeof(fileNameArray)  / sizeof(fileNameArray[0]));
// sizeof() is a utility that measures the size of data structures - this allows
// adding / deleting titles without having to change anything else - nothing breaks!
// try cutting and pasting some titles - titles can be used more than once - to prove this works

// another array - to show you how you might organize files
// if you do use more than one array - make sure you use the appropriate
// "maxFilenameIndex" for the array in your functions - some of the functions
// have "maxFilenameIndex" hardwired in - and you may need to chang that
char* fileNameArray2[] = {"WhenThe.wav", "reminder.wav", "SshSsh.wav", "CompAre.wav", "Whoop.wav", "Click.wav", "Click.wav", "SshSsh.wav"};
int maxFilenameIndex2 = (sizeof(fileNameArray2)  / sizeof(fileNameArray2[0]));

TeensyPlayer player1;
TeensyPlayer player2;
TeensyPlayer player3;
TeensyPlayer player4;  
TeensyPlayer player5;
TeensyPlayer player6;


void setup() {
  Serial.begin(9600);
  Serial.println("LIS3DH test!");
  delay(1000);
#ifdef ACCELEROMETER
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1);
  }
#endif

#ifdef ACCELEROMETER
  Serial.println("LIS3DH found!");
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
#endif

  button1 = &butt1;   // make pointers point to Button class instances
  button2 = &butt2;
  button3 = &butt3;


  pinMode(buttonPin1, INPUT_PULLUP); // for switches wired with ONLY one wire to ground
  pinMode(buttonPin2, INPUT_PULLUP); // switch connects to Ground when pressed
  pinMode(buttonPin3, INPUT_PULLUP); // NO PULLDOWN RESISTORS!

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(40);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.6);

  // set up mixer volumes 
  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);
  mixer1.gain(2, 0.5);
  mixer1.gain(3, 0.5);
  mixer2.gain(0, 0.5);
  mixer2.gain(1, 0.5);
  mixer2.gain(2, 0.5);
  mixer2.gain(3, 0.5);
  mixer3.gain(0, 0.5);
  mixer3.gain(1, 0.5);
  mixer3.gain(2, 0.5);
  mixer3.gain(3, 0.5);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);

  if (!(SD.begin(SDCARD_CS_PIN))) {
    // no SD card found, stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      Serial.println("Check to see that you are not using pins 6,7,9,11,12,13,14,18,19,22,23");
      Serial.println("Check the soldering on all Teensy and Audio Shield pins");
      delay(2000);
    }
  }

  Serial.print("maxFilenameIndex = ");
  Serial.print(maxFilenameIndex);
  Serial.println("This is the number of titles in my fileNameArray");

  Serial.print("maxFilenameIndex2 = ");
  Serial.print(maxFilenameIndex2);
  Serial.println("TNumber of titles in fileNameArray2");

  readSensors();
  delay(2000); // to read information above
}


void loop() {
  //  /****** uncomment the lines below for a quick check of your hardware  *******/
  //	 if (!playSdWav1.isPlaying()) {
  //      playSdWav1.play("one.wav");    // make sure this filename is on your SD card
  //      delay(3); // short delay seems to be necessary or it skips files
  //    }
  //   return;

  readSensors(); // all the pots, switch and accelerometer readings are in readSensors()

  player1.startStopPlayAfileInAnArray(0, fileNameArray, maxFilenameIndex, button1, &playSdWav1);
  player2.playAFileWhileButtonDown(2, fileNameArray, maxFilenameIndex, button2, &playSdWav2);
  //  player3.startStopPlayAfileInAnArray(1, fileNameArray, maxFilenameIndex, button3, &playSdWav3);
  //  player4.startStopPlayAfileInAnArray(0, fileNameArray2, maxFilenameIndex2, button3, &playSdWav4); uses second array

  return;   // put code to test above the return - nothing below return will play or is active
  // comments and code below this return are just for explanation of the functions ("methods" is the correct name)
  // don't remove the return above or strange things are going to happen

  // void playAfileInAnArray(fileIndex (number to play in array), arrayName, maxFilenameIndex, buttonName, &playSdWav)
  // enter fileIndex, arrayName, maxFileIndex, buttonName, SDplayerName you are using
  // don't forget the proceeding ampersands
  // playAfileInAnArray(indexToPlayInArray, fileNameArray, maxFilenameIndex, button1, &playSdWav1);
  // Example: playAfileInAnArray(0, fileNameArray, maxFilenameIndex, button1, &playSdWav1);
  
  // plays a file while the button is down, stops on button up
  // playAfileWhileButtonDown(fileIndex, arrayName, maxFilenameIndex, &buttonName, &playSdWav)
  // Example: playAFileWhileButtonDown(2, fileNameArray, maxFilenameIndex, button2, &playSdWav2);

  // void playRangeOfFilesInAnArray(arrayName, maxFilenameIndex, &buttonName, &playSdWav, startFileIndex, endFileIndex)
  // Example: playRangeOfFilesInAnArray(0, 3, fileNameArray, maxFilenameIndex, button3, &playSdWav3);

  // when button is pressed, it plays one file contstantly until button is pushed again
  // void startStopPlayAfileInAnArray(fileIndex, arrayName, maxFilenameIndex, ButtonName, SdWaveplayer){
  // Example: startStopPlayAfileInAnArray(3, fileNameArray2, maxFilenameIndex2,  button1, &playSdWav1);

  // plays a range of titles in an array. Speed is controlled by the parameter passed in speedMS
  // play toggles on and off with press of buttonName
  // playRangeStartStopWithSpeedControl(arrayName, maxFilenameIndex, &buttonName, &SdWaveplayer, speedMS, startIndex, endIndex)
  // Example lines below: 
  // unsigned int speedMS = (controlPotVal * 2) + 10;  // 10 mS to 2 sec    // copy this line too - for testing above
  // playRangeStartStopWithSpeedControl(1, 4, fileNameArray2, maxFilenameIndex2, button3, &playSdWav3, speedMS);

  // playRangeControlledByUpDownButtons
  // Up-down buttons. Plays one file at a time and stops.
  // One button moves forward through files, the other retreats.
  // interrupts playing file with new one
  // playRangeControlledByUpDownButtons(int startIndex, int endIndex, arrayName, maxFilenameIndex, &downButtonName, &upButtonName, &SdWaveplayer) {
  // Example: playRangeControlledByUpDownButtons( 0, 2, fileNameArray, maxFilenameIndex, button2, button3, &playSdWav2);
  

  // plays an index in a range govened by a sensor. The function is not triggered to play a new file
  // until the sensor input changes. "track" shoud be a value between startIndex and endIndex (inclusive).
  // you must uncomment "ifdef ACCELEROMETER" at the top of the sketch to use this function
  // you must also have an accelerometer hooked up
  //    accel = lis.x;
  //    playRangeControlledByAccelerometer(fileNameArray, maxFilenameIndex, &playSdWav3, accel, 0, 4);
}  // loop end

void readSensors() {
  volumePotVal = analogRead(volumePotPin);  // comment out if you don't have volume control
  controlPotVal = analogRead(controlPotPin);  // comment out if you don't have control pot

  // debug pots
  //  Serial.print(" pot values ");
  //  Serial.print(volumePotVal);
  //  Serial.print("\t");
  //  Serial.println(controlPotVal);
  // delay(3);

#ifdef ACCELEROMETER
  lis.read();      // get X Y and Z data at once
  // Then print out the raw accelerometer data
  // Serial.print("X:  "); Serial.print(lis.x); Serial.print("  \tY:  ");
  // Serial.print(lis.y);  Serial.print("  \tZ:  "); Serial.println(lis.z);
#endif


  button1->update(); // the arrow operator defreferences the button1 pointers
  button2->update();
  button3->update();
  /*  debug switches */
  Serial.println(button1->read());
  // Serial.print("\t");
  //  Serial.print(button2.read());
  //  Serial.print("\t");
  //  Serial.println(button3.read());
  // delay(3);  // use this with debugging then comment out again for faster sketch execution
}

int smoothed(int input) {
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
    // send it to the computer as ASCII digits
    //    Serial.print("input " );
    //    Serial.print(input);
    //    Serial.print("  total " );
    //    Serial.print(total);
    //    Serial.print("  average = ");
    //    Serial.println(average);
    // delay(1);        // delay in between reads for stability

    return average;
  }
}






