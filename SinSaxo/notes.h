#define NOTE_A0   27.50
#define NOTE_As0  29.14
#define MOTE_B0   30.87
#define NOTE_C1   32.70
#define NOTE_Cs1  34.65
#define NOTE_D1   36.71
#define NOTE_Ds1  38.89
#define NOTE_E1   41.20
#define NOTE_F1   43.65
#define NOTE_Fs1  46.25
#define NOTE_G1   49.00
#define NOTE_Gs1  51.91
#define NOTE_A1   55.00
#define NOTE_As1  58.27
#define NOTE_B1   61.74
#define NOTE_C2   65.41
#define NOTE_Cs2  69.30
#define NOTE_D2   73.41
#define NOTE_Ds2  77.78
#define NOTE_E2   82.41
#define NOTE_F2   87.31
#define NOTE_Fs2  92.50
#define NOTE_G2   98.00
#define NOTE_Gs2 103.82
#define NOTE_A2  110.00
#define NOTE_As2 116.54
#define NOTE_B2  123.47
#define NOTE_C3  130.81
#define NOTE_Cs3 138.59
#define NOTE_D3  146.83
#define NOTE_Ds3 155.56
#define NOTE_E3  164.81
#define NOTE_F3  174.61
#define NOTE_Fs3 185.00
#define NOTE_G3  196.00
#define NOTE_Gs3 207.65
#define NOTE_A3  220.00
#define NOTE_As3 233.08
#define NOTE_B3  246.94
#define NOTE_C4  261.63
#define NOTE_Cs4 277.18
#define NOTE_D4  293.66
#define NOTE_Ds4 311.13
#define NOTE_E4  329.63
#define NOTE_F4  349.23
#define NOTE_Fs4 369.99
#define NOTE_G4  392.00
#define NOTE_Gs4 415.30
#define NOTE_A4  440.00
#define NOTE_As4 466.16
#define NOTE_B4  493.88

// The equation for note to frequency is:
float Freq(int noteValue) {
  return 440.0f * exp2f((float)(noteValue - 69) * 0.0833333f);
}
// note:  a value between 0 and 127



const uint32_t holes_Map_Codification[] = {
0xFFFFFF,0x7FFFFF,0x3FFFFF,0x1FFFFF,0x2FFFFF,0x27FFFF,0x2BFFFF,0x29FFFF,0x28FFFF,0x297FFF,0x293FFF,0x291FFF,
0x292FFF,0x2927FF,0x292BFF,0x2929FF,0x292F7F,0x29293F,0x2FFFFE,0x27FFFE,0x2BFFFE,0x29FFFE,0x297FFE,0x28FFFE,
0x293FFE,0x291FFE,0x292FFE,0x292BFE,0x2927FE,0x2929FE,0x292F7E,0x29293E,0x29291E,0x29290E,0x292906,0x292902,
0x292900,0x29F3FE,0x29F9FE,0x2FFF7E,0x2BFB3E,0x29281E,0x29290E,0x292906,0x292902,0x2D6B04
};


const int relative_note_Map[] = {
-15,-14,-13,-12,-11,-10,-9,-8,-7,-7,-6,-5,-4,-3,-3,-2,-1,0,1,2,3,4,5,5,6,7,8,9,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26  
};

char charNotes[128][10] = {
  "C-1","C#-1","D-1","D#-1/Eb-1","E-1","F-1","F#-1","G-1","G#-1","A-1",
  "A#-2/Bb-2","B-1","C0","C#0","D0","D#0/Eb0","E0","F0","F#0","G0","G#0",
  "A0","A#0/Bb0","B0","C1","C#1","D1","D#1/Eb1","E1","F1","F#1","G1","G#1",
  "A1","A#1/Bb1","B1","C2","C#2","D2","D#2/Eb2","E2","F2","F#2","G2","G#2",
  "A2","A#2/Bb2","B2","C3","C#3","D3","D#3/Eb3","E3","F3","F#3","G3","G#3",
  "A3","A#3/Bb3","B3","C4","C#4","D4","D#4/Eb4","E4","F4","F#4","G4","G#4",
  "A4","A#4/Bb4","B4","C5","C#5","D5","D#5/Eb6","E5","F5","F#5","G5","G#5",
  "A5","A#5/Bb5","B5","C6","C#6","D6","D#6/Eb6","E6","F6","F#6","G6","G#6",
  "A6","A#6/Bb6","B6","C7","C#7","D7","D#7/Eb7","E7","F7","F#7","G7","G#7",
  "A7","A#7/Bb7","B7","C8","C#8","D8","D#8/Eb8","E8","F8","F#8","G8","G#8",
  "A8","A#8/Bb8","B8","C8","C#9","D9","D#9/Eb9","E9","F9","F#9","G9"
};

const char* MidiToCharNote(int noteValue) {
  return *(charNotes + noteValue);
};


// according to http://www.guitar-chords.org.uk/
// and http://www.8notes.com/guitar_chord_chart/c.asp
// Chords 
              // open =  NOTE_E2  NOTE_A2  NOTE_D3  NOTE_G3  NOTE_B3  NOTE_E4
const float Cmajor[6] = {      0, NOTE_C3, NOTE_E3, NOTE_G3, NOTE_C4, NOTE_E4};  // C - E - G
const float Dmajor[6] = {      0,       0, NOTE_D3, NOTE_A3, NOTE_D4, NOTE_Fs4}; // D - F# - A
const float Emajor[6] = {NOTE_E2, NOTE_B2, NOTE_E3, NOTE_Gs3,NOTE_B3, NOTE_E4};  // E - G# - B
const float Fmajor[6] = {      0, NOTE_A2, NOTE_F3, NOTE_A3, NOTE_C4, NOTE_F4};  // F - A - C
const float Gmajor[6] = {NOTE_G2, NOTE_B2, NOTE_D3, NOTE_G3, NOTE_B3, NOTE_E4};  // G - B - D
const float Amajor[6] = {      0, NOTE_A2, NOTE_E3, NOTE_A3, NOTE_Cs4,NOTE_E4};  // A - C# - E
const float Bmajor[6] = {      0, NOTE_B2, NOTE_Fs3,NOTE_B3, NOTE_Ds4,NOTE_Fs4}; // B - D# - F#
const float Cminor[6] = {      0, NOTE_C3, NOTE_G3, NOTE_C4, NOTE_Ds4,NOTE_G4};  // C - D# - G
const float Dminor[6] = {      0,       0, NOTE_D3, NOTE_A3, NOTE_D4, NOTE_F4};  // D - F - A
const float Eminor[6] = {NOTE_E2, NOTE_B2, NOTE_E3, NOTE_G3, NOTE_B3, NOTE_E4};  // E - G - B
const float Fminor[6] = {NOTE_F2, NOTE_C3, NOTE_F3, NOTE_Gs3,NOTE_C4, NOTE_F4};  // F - G# - C
const float Gminor[6] = {NOTE_G2, NOTE_D3, NOTE_G3, NOTE_As3,NOTE_D3, NOTE_G4};  // G - A# - D
const float Aminor[6] = {      0, NOTE_A2, NOTE_E3, NOTE_A3, NOTE_C4, NOTE_E4};  // A - C - E
const float Bminor[6] = {      0, NOTE_B2, NOTE_Fs3,NOTE_B3, NOTE_D4, NOTE_Fs4}; // B - D - F#
