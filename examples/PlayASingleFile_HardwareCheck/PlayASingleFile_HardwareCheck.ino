/* Wav File Player Title Name Array And Button Tutorial
    Plays files from an array of file names
    The Teensy Audio Shield uses pins 6,7,9,11,12,13,14,18,19,22,23
    This means that you may use pins 0, 1, 2, 3, 4, 5, 8, 16, 17, 20, 21, A10, A11.
    Additionally, there are lots more pins on the bottom side of the Teensy board
    but you'll have to solder (stranded!) wires on them and bring them out to the breadboard.

    For the Teensy & Teensy Audio Shield WAV player example
    Hardware: Two 10k (104) pots, outside leads connected to ground and +3.3V
    Wipers connected to pins 16 & 17

    3 pushbuttons connected to pins 0, 1, 2
    ****   Important - switches wired with ONLY one wire to ground ****
    ****   The switch connects input to ground when pushed         ****
    ****   NO PULLDOWN RESISTORS SHOULD BE ADDED!                  ****

    Teensy Audio Shield, Teensy 3.2 or similar
    Plays a single file for hardware check.
*/

// make sure these libraries are only included once!
#include <Audio.h>
#include <Wire.h>
#include <SerialFlash.h>
#include <Bounce.h>
#include <SPI.h>
#include <SD.h>
#include "TeensyPlayer.h"

/*** Debugging - uncomment line to print debugging information ***/
// #define ACCELEROMETER  // uncomment this to use the accelerometer
 #define DEBUG_BUTTONS  // uncomment to print button debugging
 #define DEBUG_POTS     // uncomment to print pot debugging
// #define DEBUG_SMOOTH   // uncomment to debug smooth function

#ifdef ACCELEROMETER

#include <Adafruit_LIS3DH.h> // uncomment for accelerometer
#include <Adafruit_Sensor.h>
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

const char* fileNameArray[] =
{ "0.wav", "1.wav", "2.wav", "3.wav", "4.wav", "5.wav", // break lines like this
  "6.wav", "7.wav", "8.wav", "9.wav", "10.wav" // use your own titles!
};

// This is an example of a two-dimensional array
// The first dimension is the letters (chars) in the title,
// The second dimmension (leftmost) is the strings of the title, in which we are most interested.

int maxFilenameIndex = (sizeof(fileNameArray)  / sizeof(fileNameArray[0]));
// sizeof() is a utility that measures the size of data structures - this allows
// adding / deleting titles without having to change anything else - nothing breaks!
// try cutting and pasting some titles - titles can be used more than once - to prove this works

TeensyPlayer player1;

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
  
  delay(2000); // allow user to read above information in Serial Monitor
}


void loop() {

  readSensors(); // all the pots, switch and accelerometer readings are in readSensors()

  /* This is just intended as a hardware check. The following code
      plays a single file over and over again.
  */

  if (!playSdWav1.isPlaying()) {
    playSdWav1.play("1.wav");    // make sure this filename is on your SD card
    delay(3); // short delay seems to be necessary or it skips files
  }
}

void readSensors() {
  volumePotVal = analogRead(volumePotPin);  // comment out if you don't have volume control
  controlPotVal = analogRead(controlPotPin);  // comment out if you don't have control pot

#ifdef DEBUG_POTS
  Serial.print(" pot values ");
  Serial.print(volumePotVal);
  Serial.print("\t");
  Serial.println(controlPotVal);
  delay(3);
#endif

#ifdef ACCELEROMETER
  lis.read();      // get X Y and Z data at once
  Then print out the raw accelerometer data
  Serial.print("X:  "); Serial.print(lis.x); Serial.print("  \tY:  ");
  Serial.print(lis.y);  Serial.print("  \tZ:  "); Serial.println(lis.z);
#endif


  button1->update(); // the arrow operator defreferences the button1 pointers
  button2->update(); // (replaces the "dot" operator in Serial.print() etc.)
  button3->update();
  /*  debug switches */

#ifdef DEBUG_BUTTONS
  Serial.print(button1->read());
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
    Serial.print("input " );
    Serial.print(input);
    Serial.print("  total " );
    Serial.print(total);
    Serial.print("  average = ");
    Serial.println(average);
    delay(3);        // delay in between reads for stability
#endif
    return average;
  }
  else{
  	return input; // this should only happen once
  }
}
