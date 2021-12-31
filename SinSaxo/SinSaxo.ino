/* Saxo Synth v0.1
 * This requieres Teensy 4.0 and correct Pin conections as schematics
 * any change made in Teensy Pins connection must be reflected in 
 * #define labels
*/


#include <Bounce2.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "notes.h"
#include "flute_samples.h"
#include "clarinet_samples.h"
#include "saxosoprano_samples.h"
#include "trumpet_samples.h"



#define TUNE_BASE 37 //The base MIDI note value the instrument is tuned to (C#1=37 for an Alto Sax)
#define MIDI_CHANNEL 1 //The MIDI chaneel we will use

//define Specific combination for Control COdes
#define INCREASE_OCTAVE_CODE 0x39293B
#define DECREASE_OCTAVE_CODE 0x39292F
#define INCREASE_AFFINATION_CODE 0x29293B
#define DECREASE_AFFINATION_CODE 0x29292F
#define CHANGE_INSTRUMENT_CODE 0x29283F
#define CHANGE_MODE_CODE 0x292937

//Definition of diferente states of the SinSaxo
#define SILENCE 0
#define TRANSIT 1
#define SOUND 2

#define DEBOUNCE_TIME 15


//Specific thresholds
#define NOTE_OFF_THRESHOLD 130   //pressure value for stop sounding
#define ACTIVATE_THRESHOLD 180   //pressure value for start sounding
#define SOUND_THRESHOLD 160      //pressure value for initial sounding
#define TRANSIT_TIME_THRESHOLD 10   //time needed in miliseconds to play sound
#define AFTER_TOUCH_INTERVAL 50 //time needed for sending another aftertouch


//Specific THRESHOLD for input analog values to circuits
#define BAD_FUNCION 550
#define BOTH_CIRCUITS_CLOSED 620
#define LOW_RESISTANCE_CLOSED 700
#define HIGH_RESISTANCE_CLOSED 850
#define NO_CIRCUIT_CLOSED 1024


//Definition of used digital Pins
#define dPIN_1 0
#define dPIN_2 1
#define dPIN_3 5
#define dPIN_4 6
#define dPIN_5 7
#define dPIN_6 8
#define dPIN_7 9
#define dPIN_8 10
#define dPIN_9 11
#define dPIN_10 12
#define dPIN_11 18
#define dPIN_12 19
#define dPIN_13 20
#define dPIN_14 23


//Conected Saxo Holes to defined digital Pins
#define Hole_dPIN_1 13
#define Hole_dPIN_2 12
#define Hole_dPIN_3 11
#define Hole_dPIN_4 10
#define Hole_dPIN_5 9 
#define Hole_dPIN_6 8
#define Hole_dPIN_7 7
#define Hole_dPIN_8 6
#define Hole_dPIN_9 5
#define Hole_dPIN_10 4
#define Hole_dPIN_11 19
#define Hole_dPIN_12 18
#define Hole_dPIN_13 17
#define Hole_dPIN_14 14

//Specific Pin where hardware led is connected to (dont allow normal pull-up)
//Teensy 4.0 has this led connected to pin 13
#define lPIN 13  
#define Hole_lPIN 20

//Definition of analog Pins used (each analog pin allow 2 hole signals)
#define aPIN_1 14
#define aPIN_2 15
#define aPIN_3 16
#define aPIN_4 17
#define aPIN_5 22

//Conected Holes to defined analog Pins (H- High Resistence L-Low Resistence)
#define Hole_aPIN_H1 3
#define Hole_aPIN_L1 24  //Neck octave hole
#define Hole_aPIN_H2 2
#define Hole_aPIN_L2 21
#define Hole_aPIN_H3 1
#define Hole_aPIN_L3 22   
#define Hole_aPIN_H4 25  //Body octave hole
#define Hole_aPIN_L4 23
#define Hole_aPIN_H5 15
#define Hole_aPIN_L5 16

//Specific Pin where Pressure Sensort is connected TO
#define SENSOR_PIN 21


// GUItool: begin automatically generated code
AudioSynthWavetable      wavetable;     //xy=126.00000762939453,195.0000057220459
AudioMixer4              mezclador;     //xy=317.00000762939453,239.0000228881836
AudioOutputI2S2          i2s;           //xy=561.0000953674316,244.0000286102295
AudioConnection          patchCord1(wavetable, 0, mezclador, 0);
AudioConnection          patchCord5(mezclador, 0, i2s, 0);
AudioConnection          patchCord6(mezclador, 0, i2s, 1);
// GUItool: end automatically generated code


//Defined Constants values
const int digitalPINS[] = {dPIN_1,dPIN_2,dPIN_3,dPIN_4,dPIN_5,dPIN_6,dPIN_7,dPIN_8,dPIN_9,dPIN_10,dPIN_11,dPIN_12,dPIN_13,dPIN_14};
const int digitalLedPIN = lPIN;
const int analogPINS[]= {aPIN_1,aPIN_2,aPIN_3,aPIN_4,aPIN_5};
const int digitalInputs[] = {Hole_dPIN_1,Hole_dPIN_2,Hole_dPIN_3,Hole_dPIN_4,Hole_dPIN_5,Hole_dPIN_6,Hole_dPIN_7,Hole_dPIN_8,Hole_dPIN_9,Hole_dPIN_10,Hole_dPIN_11,Hole_dPIN_12,Hole_dPIN_13,Hole_dPIN_14};
const int digitalLedInput = Hole_lPIN;
const int analogInputs[] ={Hole_aPIN_H1,Hole_aPIN_H2,Hole_aPIN_H3,Hole_aPIN_H4,Hole_aPIN_H5,Hole_aPIN_L1,Hole_aPIN_L2,Hole_aPIN_L3,Hole_aPIN_L4,Hole_aPIN_L5};

//Defined initial values
int state = SILENCE;
boolean MIDImode = false;
int baseTune = TUNE_BASE;
int instrument = 0;
unsigned int currentNote= TUNE_BASE;  
int intervalTime;
int initial_time_transit;
int initial_pressure_value ;

//Define all digital/analog entries as Bounce
Bounce digitalEntries[14] = Bounce();
Bounce analogEntries[5] = Bounce();



/* Test_state_hole(int hole) 
 * checks if a specific hole is open(false) or close(true)
 * argument hole is a int number from 0-23 that represent a hole
 * in the SinSaxo as specified in the schematics
 */
boolean Test_state_hole(int hole){

    if ((hole > 25) || (hole < 0)) return false;
    
    //First we test digital pins
    for (int i=0; i<14; ++i) {
      if (digitalInputs[i]== hole) {
          digitalEntries[i].update();
          if (digitalRead(digitalPINS[i])== LOW) {
            return true;
          } else {
            return false;
          }
      }
    }

    //them we test digital led pin
   if (digitalLedInput == hole) {
     if (digitalRead(digitalLedPIN)== LOW) {
            return true;
          } else {
            return false;
          }
   }

    //then we test anlog pins with high resistence
     for ( int i=0; i <5; ++i) {
       if (analogInputs[i]== hole) {
         int analogValue = analogRead(analogPINS[i]);
         if  ( ( (analogValue < HIGH_RESISTANCE_CLOSED) && (analogValue > LOW_RESISTANCE_CLOSED) ) || ( analogValue < BOTH_CIRCUITS_CLOSED)) {
            return true;
         }else {
            return false;
          }
       }
     }

    //finaly we test analog pins with low resistence
     for ( int i=5; i <10; ++i) {
        if (analogInputs[i]== hole) {
        int analogValue = analogRead(analogPINS[i-5]);
        if  (analogValue < LOW_RESISTANCE_CLOSED ) {
           return true;
         }else {
          return false;
          }
        }
     }

    return true; 
}




/* Init_state_holes()
 * Sends to serial Print the state of the holes
 * this is used to detect any wrong conection during the test fase
*/
void Init_state_holes(){
  
    Serial.println("Holes state: ");
    Serial.println("321098765432109876543210");
    unsigned int note_code=0;
    //controls the position bit in note_code to be changed    
    for (int hole=23; hole>0; --hole) {
      if (Test_state_hole(hole)) {
        Serial.print("1");
        note_code |= 1 << hole;
      } else {
        Serial.print("0");
        note_code &= ~(1 << hole); //clear bit in bitPos
      }
      
    }
        
    //Finally we check neck-octave and body-octave
    boolean pin24 = Test_state_hole(24);
    boolean pin25 = Test_state_hole(25);
 
    if (pin24 && pin25) {
      //if both of them are closed then octave is closed
       Serial.print("1");
        note_code |= 1 << 0;  //set  bit in bitPos
    } else {
        Serial.print("0");
        note_code &= ~(1 << 0); //clear bit in bitPos
        
    }
  Serial.print(" :");
  Serial.printf("0x%X\n",note_code); 
  
}

void print_bin(unsigned int integer) {

  int k;
  for (int c = 23; c >= 0; c--)
  {
    k = integer >> c;

    if (k & 1)
      Serial.printf("1");
    else
      Serial.printf("0");
  }
}

/*  Compute_holes()
 *  function to compute the state of all holes and return 
 *  a unsigned int number thar represents the actual saxo state
 *  in SinSaxo hole format
 */

unsigned int Compute_holes(){
   static int loopsBetweenCalls;
   unsigned int note_code = currentNote;
   //compute digital PINS
   for (int i=0; i < 14; ++i){
        //digitalEntries[i].update(); 
        int bitPos = digitalInputs[i] ;
        if (digitalRead(digitalPINS[i])==LOW) {
          //the hole at position pos is closed (bit equal to 1)
          note_code |= 1 << bitPos;
          } else {
          note_code &= ~(1 << bitPos);
            }
   }
   
  //compute digital led PIN
   if (digitalRead(digitalLedPIN)==LOW){
    note_code |= 1 << digitalLedInput;
   }else {
          note_code &= ~(1 << digitalLedInput);
            }
   
 
  note_code |= 1 << 0;  //octave hole is compute initialy as close
  
  //compute analog PINS
  for (int i=0; i < 5; ++i) {
      int analogValue = analogRead(analogPINS[i]);
      int posH = analogInputs[i];
      int posL = analogInputs[i+5];
      if (posH>23) posH = 0;
      if (posL>23) posL = 0;
       if (analogValue < BAD_FUNCION) {
          //BAD FUNCTION
       } else if ( analogValue < BOTH_CIRCUITS_CLOSED) {
          if (posH>0) { note_code |= 1 << posH;}  //only change if is not octave hole
          if (posL>0) { note_code |= 1 << posL;}  //only change if is not octave hole
        } else if  ( analogValue < LOW_RESISTANCE_CLOSED )  {
          note_code &= ~(1 << posH);
          if (posL >0) {note_code |= 1 << posL;}    //only change if is not octave hole
          } else if  ( analogValue < HIGH_RESISTANCE_CLOSED ) {
            if (posH >0) {note_code |= 1 << posH;}  //only change if is not octave hole
            note_code &= ~(1 << posL);
            } else if  ( analogValue <  NO_CIRCUIT_CLOSED ) {
              note_code &= ~(1 << posH);
              note_code &= ~(1 << posL);
              } else {
                //BAD FUNCTION
                }
        }
   loopsBetweenCalls++;
   if (loopsBetweenCalls >200) {
    print_bin(note_code);
    Serial.print(" :");
    Serial.printf("0x%X\n",note_code);
    loopsBetweenCalls=0; 
   }
  return note_code;
  }

/* decode_control()
 * specific hole combinations are control parameters
 * this function manage those control commands
 * NO return any value, just performs an action
 */
void Decode_control(int note_code) {
  
  if (note_code == CHANGE_MODE_CODE) {
    MIDImode = not(MIDImode);
    Serial.print("Mode Changed: MIDI ");
    if (MIDImode) Serial.println("On"); else Serial.println("Off");
    delay(800);
  } else if (note_code == CHANGE_INSTRUMENT_CODE && not(MIDImode)){
      if (instrument == 0) {
        wavetable.setInstrument(flute);
        Serial.println("Instrument selected: flute");
        instrument +=1;
        delay(500);
      } else if (instrument == 1) {
        wavetable.setInstrument(clarinet);
        Serial.println("Instrument selected: clarinet");
        instrument +=1 ;
        delay(500);
      } else if (instrument == 2) {
        wavetable.setInstrument(saxosoprano);
        Serial.println("Instrument selected: saxo soprano");
        instrument +=1;
        delay(500);
      } else {
         wavetable.setInstrument(trumpet);
         Serial.println("Instrument selected: trumpet");
        instrument = 0;
        delay(500);
      }
  } else if (note_code == INCREASE_OCTAVE_CODE && baseTune < 89) {
    baseTune += 12;
    Serial.print("Tune increased by 1 Octave new base-tone: ");
    Serial.println(MidiToCharNote(baseTune));
    delay(500);
  } else if (note_code == DECREASE_OCTAVE_CODE && baseTune > 26) {
    baseTune -= 12;
    Serial.print("Tune decreased by 1 Octave new base-tone: ");
    Serial.println(MidiToCharNote(baseTune));
    delay(500);
  } else if (note_code == INCREASE_AFFINATION_CODE && baseTune < 101) {
    baseTune++;
    Serial.print("Tune increased by 1 tone new base-tone: ");
    Serial.println(MidiToCharNote(baseTune));
    delay(500);
  } else if (note_code == DECREASE_AFFINATION_CODE && baseTune > 15) {
    baseTune--;
    Serial.print("Tune decreased by 1 tone new base-tone: ");
    Serial.println(MidiToCharNote(baseTune));
    delay(500);
  }
 
}

/* DecodeNote(int note_code)
 * function to decode the note played from the unsigned int note_code variable
 * the note_code is in SinSaxo format (each bit of this unsignet integer represents the state of a hole in Sax)
 * returns note in the int MIDI format (0-127 value)
 */
int Decode_note(unsigned int note_code) {

  for(int i=0; i< sizeof(holes_Map_Codification)/sizeof(uint32_t); i++) {
    if (note_code==holes_Map_Codification[i]) {
      return  baseTune +  relative_note_Map[i]; 
    }
  }
  return currentNote; //if note_code not correspond to any note sounds last correct note
}


void setup() { 
  Serial.begin(115200);
  Serial.println("Inicio");
  
  for(int i=0; i < 14; ++i){
    digitalEntries[i]= Bounce (digitalPINS[i], DEBOUNCE_TIME);
    pinMode(digitalPINS[i], INPUT_PULLUP);
  }

    pinMode(digitalLedPIN, INPUT_PULLUP);

  for(int i=0; i < 5; ++i){
    analogEntries[i]= Bounce (analogPINS[i], DEBOUNCE_TIME);
    pinMode(analogPINS[i], INPUT_PULLUP);
  }

  
  Init_state_holes();
  AudioMemory(20);
  mezclador.gain(0, 0.5);  //Actual Volume 
  wavetable.setInstrument(trumpet);
  Serial.println("start in SILENCE");
  unsigned int noteCode = Compute_holes();
  Decode_control(noteCode);
}


void loop() {
  delay(15);
  int sensorValue = analogRead(SENSOR_PIN); 
  unsigned int noteCode = Compute_holes();
  
  //Specific action depends on State
  if (state == SOUND) {
    int notePlayed = Decode_note(noteCode);
    if (sensorValue < NOTE_OFF_THRESHOLD) {      
      if (MIDImode) {
        usbMIDI.sendNoteOff(currentNote, 100, MIDI_CHANNEL);  
      } else {
         wavetable.stop();
        }
      Serial.print("From SOUND to SILENCE: ");
      Serial.println(sensorValue);
      state = SILENCE;
      } else if (currentNote != notePlayed) {
          if (MIDImode) {
            usbMIDI.sendNoteOff(currentNote, 100, MIDI_CHANNEL);
            int velocity = map(sensorValue, SOUND_THRESHOLD, 1023, 0, 127);
            usbMIDI.sendNoteOn(notePlayed, velocity, MIDI_CHANNEL);
          } else {
           wavetable.playNote(notePlayed);
          }
        currentNote = notePlayed;
        Serial.print("Current Note: ");
        Serial.println(MidiToCharNote(currentNote));
        
      } else if (millis() - intervalTime > AFTER_TOUCH_INTERVAL) {
        intervalTime = millis();
        if (MIDImode) {
                 int afterTouchValue = map(sensorValue, ACTIVATE_THRESHOLD, 1023, 0, 127);
                 usbMIDI.sendAfterTouch(afterTouchValue, MIDI_CHANNEL);    
        } else {
              float tgain = map(sensorValue, ACTIVATE_THRESHOLD, 1023, 0.0, 1.0);
              wavetable.amplitude(tgain);
              
              // mezclador.gain(0,tgain)); //channel 1 = trompeta; 
          }
      }
  
    } else if (state == SILENCE) {
      if (sensorValue > ACTIVATE_THRESHOLD) {
        initial_time_transit = millis();
        initial_pressure_value = sensorValue;
        Serial.print("From SILENCE to TRANSIT: ");
        Serial.println(sensorValue);
        state = TRANSIT;  
      } else {
        Decode_control(noteCode); //Only performs control function check when Saxo is in silence state
      }
    } else {
      // state == TRANSIT
       if (sensorValue> SOUND_THRESHOLD) {
        if (millis( )-  initial_time_transit  > TRANSIT_TIME_THRESHOLD) {
          currentNote = Decode_note(noteCode);
          if (MIDImode) {
              int velocity = map(initial_pressure_value, SOUND_THRESHOLD, 1023, 0, 127);
              usbMIDI.sendNoteOn(currentNote, velocity, MIDI_CHANNEL);
            } else {
              wavetable.playNote(currentNote);
            }
           Serial.print("From TRANSIT to SOUND: ");
           Serial.println(sensorValue);
           Serial.print("Current Note: ");
           Serial.println(MidiToCharNote(currentNote));
           state = SOUND;
          }
        } else {
        state = SILENCE;
        Serial.print("From TRANSIT to SILENCE: ");
        Serial.println(sensorValue);
      }
    }


}
