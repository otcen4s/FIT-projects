#include "notes.h"
#include <string.h>

/******************************************************************* GLOBAL VARIABLES ************************************************************/
/*************************************************************************************************************************************************/

// number of pin where the speaker is connected
int speakerPin = 8;

// notes for merry christmas melody
int merryChristmasMelody[] = {
  NOTE_C5,NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_D5, NOTE_D5, 
  NOTE_D5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_C5, 
  NOTE_C5, NOTE_A5, NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_D5, 
  NOTE_C5, NOTE_C5, NOTE_D5, NOTE_G5, NOTE_E5,

  NOTE_F5, NOTE_C5, NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_D5, 
  NOTE_D5, NOTE_D5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, 
  NOTE_C5, NOTE_C5, NOTE_A5, NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5, 
  NOTE_D5, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_G5, NOTE_E5, NOTE_F5, REST,
};

// notes duration for merry christmas melody
int merryChristmasNotesDuration[] = {4,4,8,8,8,8,4,4,4,4,8,8,8,8,4,4,4,4,8,8,8,8,4,4,8,8,4,4,4,   
              
                                     2,4,4,8,8,8,8,4,4,4,4,8,8,8,8,4,4,4,4,8,8,8,8,4,4,8,8,4,4,4,2,4};


// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!

// notes for silent night melody
int silentNightMelody[] = {
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_E4,
  NOTE_D5, NOTE_D5, NOTE_B4, NOTE_C5, NOTE_C5, NOTE_G4,

  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_G4,
  NOTE_E4, NOTE_A4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_A4, 
  NOTE_G4, NOTE_E4, 
  
  NOTE_D5, NOTE_D5, NOTE_F5, NOTE_D5, NOTE_B4, NOTE_C5, NOTE_E5, NOTE_C5,
  NOTE_G4, NOTE_E4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_C4,
};

// notes duration for silent night melody
int silentNightNotesDuration[] = {-4,8,4,-2,-4,8,4,-2,2,4,-2,2,4,-2,
   
                                  2,4,-4,8,4,-4,8,4,-2,2,4,-4,8,4,-4,8,4,-2,   
                                  
                                  2,4,-4,8,4,-2,-2,4,4,4,-4,8,4,-2};

// notes for star wars melody
int starWarsMelody[] = { 
  NOTE_AS4, NOTE_AS4, NOTE_AS4,NOTE_F5, NOTE_C6, NOTE_AS5, NOTE_A5, NOTE_G5, 
  NOTE_F6, NOTE_C6, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6, NOTE_AS5, 
  NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_F5, NOTE_C6, 
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,  
  
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6, NOTE_AS5, NOTE_A5, NOTE_AS5, 
  NOTE_G5, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, 
  NOTE_F5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C5, 
  NOTE_C5, NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
  
  NOTE_C6, NOTE_G5, NOTE_G5, REST, NOTE_C5, NOTE_D5, NOTE_D5, NOTE_AS5, 
  NOTE_A5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, 
  NOTE_E5,NOTE_C6, NOTE_C6, NOTE_F6, NOTE_DS6, NOTE_CS6, NOTE_C6, NOTE_AS5, 
  NOTE_GS5, NOTE_G5, NOTE_F5, NOTE_C6
};

// notes duration for star wars melody
int starWarsNotesDuration[] = {8,8,8,2,2,8,8,8,2,4,8,8,8,2,4,8,8,8,2,8,8,8,2,2,8,8,8,2,4,   

                               8,8,8,2,4,8,8,8,2,-8,16,-4,8,8,8,8,8,8,8,8,4,8,4,-8,16,-4,8,8,8,8,8,   
                               
                               -8,16,2,8,8,-4,8,8,8,8,8,8,8,8,4,8,4,-8,16,4,8,4,8,4,8,4,8,1};




int tempo = 140; // this value defines how fast the melody will be played
int wholenote = (60000 * 4) / tempo; // this calculates the duration of a whole note in ms
int noteDuration = 0;

String termtext; // for incoming serial data

/*************************************************************************************************************************************************/


/***************************************************************** HELP FUNCTIONS ****************************************************************/
// plays the merry christmas melody
void playMerryChristmas(){
  // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
  // there are two values per note (pitch and duration), so for each note there are four bytes
  int notes = sizeof(merryChristmasMelody) / sizeof(merryChristmasMelody[0]);
  
  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int i = 0; i < notes; i++) {

    if (merryChristmasNotesDuration[i] > 0) {
      // regular note
      noteDuration = (wholenote) / merryChristmasNotesDuration[i];
    } 
    else if (merryChristmasNotesDuration[i] < 0) {
      // dotted note -> it has negative duration
      noteDuration = (wholenote) / abs(merryChristmasNotesDuration[i]);
      noteDuration *= 1.5; // increases the duration in half for dotted note
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(speakerPin, merryChristmasMelody[i], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(speakerPin);
  }
}

// plays the silent night melody
void playSilentNight(){
  // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
  // there are two values per note (pitch and duration), so for each note there are four bytes
  int notes = sizeof(silentNightMelody) / sizeof(silentNightMelody[0]);
  
  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int i = 0; i < notes; i++) {

    if (silentNightNotesDuration[i] > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / silentNightNotesDuration[i];
    } 
    else if (silentNightNotesDuration[i] < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(silentNightNotesDuration[i]);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(speakerPin, silentNightMelody[i], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(speakerPin);
  }
}

// plays the star wars melody
void playStarWars(){
  // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
  // there are two values per note (pitch and duration), so for each note there are four bytes
  int notes = sizeof(starWarsMelody) / sizeof(silentNightMelody[0]);
  
  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int i = 0; i < notes; i++) {

    if (starWarsNotesDuration[i] > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / starWarsNotesDuration[i];
    } 
    else if (starWarsNotesDuration[i] < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(starWarsNotesDuration[i]);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(speakerPin, starWarsMelody[i], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(speakerPin);
  }
}

// play chosen melody
void playMelody(String melodyName){
  if(melodyName == "merry christmas"){
    playMerryChristmas();
  }
  else if(melodyName == "silent night"){
    playSilentNight();
  }
   else if(melodyName == "star wars"){
    playStarWars();
  }
}

/*************************************************************************************************************************************************/


/***************************************************************** MAIN FUNCTIONS ****************************************************************/


/** Function starts after program is uploaded to Arduino kit*/
void setup() {
  // Terminal input
  Serial.begin(9600);
  delay(25);
  Serial.print("Enter keywords.");
}

/** Function loops the inside code infinite times and is actively waiting for terminal inputs */
void loop() {
  while(Serial.available()) {
    // reading terminal string
    termtext = Serial.readString();
    // EOL trim for better comparission
    termtext.trim();
    
    if (termtext.substring(0) == "merry christmas"){
      Serial.println("PLAYING MERRY CHRISTMAS :-) .");
      delay(1000);
      playMelody(termtext.substring(0));
    }
    else if(termtext.substring(0) == "silent night"){
      Serial.println("PLAYING SILENT NIGHT :-) .");
      delay(1000);
      playMelody(termtext.substring(0));  
    }
    else if(termtext.substring(0) == "star wars"){
      Serial.println("PLAYING STAR WARS ... MAY FORCE BE WITH YOU :-) .");
      delay(1000);
      playMelody(termtext.substring(0));  
    }
    else if(termtext.substring(0) == "help"){
      Serial.println("You typed help option, here are some hints. \nType these string literals into terminal to play songs:\n1. 'merry christmas\n2. 'silent night'\n3. 'star wars'");
    }
    else{
      Serial.println("I am sorry, but I don't know this song neither option :-( .Please try 'help' option.");
    }
  }
}

/******************************************************************************************************************************x*******************/
