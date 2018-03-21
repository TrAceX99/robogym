#include <Tone.h>

static const uint8_t tonePin = 4;
uint8_t tempoToPlay  = 1;

Tone speaker[2];

void setup() 
{
  speaker[1].begin(4);
  speaker[2].begin(5);
}

void loop() 
{
  playMelody(Melody0, Melody0_Length, tempoToPlay); 
}



// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
// End of the normally edited bits of the sketch, the rest of this file is 
// the data for the melodies, and the supporting functions, carry on down
// by all means, but if you just want to play the melody, you don't need to
// go any further.
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------




#include <avr/pgmspace.h>


// Melody Data
// ---------------------------------------------------------------------------- 
// The melody is stored as an array of 16 bit unsigned integers (uint16_t)  
// where each integer encodes the note, octave and duration
//  
//   [duration][octave][note]  - ddddddddoooonnnn
//
//   note (nnnn) is 4 bits which are the note to play
//    C = 0, C# = 1, D = 2, D# = 3, E = 4, F = 5,
//    F# = 6, G = 7, G# = 8, A = 9, A# = 10, B = 11,
//    SILENT = 15
//
//   octave (oooo) is 4 bits which indicate the octave to play that note in,
//   maximum octave we can handle is 8, minimum is 0
//    octave 0 = 0 .. octave 8 = 8
//    
//   duration (dddddddd) is the duration of the note to play in units of tempo
//    hold note for 10 tempos = 10
//
// You can use the following web page to create the Melody data for you:
//    http://sparks.gogo.co.nz/midi_tone.html
//
// You can also select a human readable/editable output format on that page 
// instead of this short form version.


// The melodies must be named Melody0 .. Melody{NUMBER_OF_MELODIES-1}, and 
// each must also have a MelodyX_Length defined.
#define NUMBER_OF_MELODIES 1

// Track 0 - 8-Bit Square
static const uint16_t Melody0[] PROGMEM = { 
        0b1010100001000111, 0b0001100011111111, 0b1010100001001011, 0b0001100011111111, 
        0b1010100001010000, 0b0001100011111111, 0b1111111101010010, 0b1111111101010010, 
        0b1110101001010010, 0b1101100011111111, 0b1010100001000111, 0b0001100011111111, 
        0b1010100001001011, 0b0001100011111111, 0b1010100001010000, 0b0001100011111111, 
        0b1111111101010010, 0b1111111101010010, 0b1110101001010010, 0b1101100011111111, 
        0b1010100001000111, 0b0001100011111111, 0b1010100001001011, 0b0001100011111111, 
        0b1010100001010000, 0b0001100011111111, 0b1111111101010010, 0b0110100101010010, 
        0b0001100011111111, 0b1111111101001011, 0b0110100101001011, 0b0001100011111111, 
        0b1111111101000111, 0b0110100101000111, 0b0001100011111111, 0b1111111101001011, 
        0b0110100101001011, 0b0001100011111111, 0b1111111101001001, 0b1111111101001001, 
        0b1110101001001001, 0b1101100011111111, 0b1010100001001011, 0b0001100011111111, 
        0b1010100001001011, 0b0001100011111111, 0b1010100001001001, 0b0001100011111111, 
        0b1111111101000111, 0b1111111101000111, 0b0010101001000111, 0b0001100011111111, 
        0b1010100001000111, 0b0001100011111111, 0b1111111101001011, 0b0110100101001011, 
        0b0001100011111111, 0b1111111101010010, 0b0110100101010010, 0b0001100011111111, 
        0b1010100001010010, 0b0001100011111111, 0b1111111101010000, 0b1111111101010000, 
        0b0010101001010000, 0b1111111111111111, 0b1001100111111111, 0b1010100001001011, 
        0b0001100011111111, 0b1010100001010000, 0b0001100011111111, 0b1111111101010010, 
        0b0110100101010010, 0b0001100011111111, 0b1111111101001011, 0b0110100101001011, 
        0b0001100011111111, 0b1111111101000111, 0b0110100101000111, 0b0001100011111111, 
        0b1111111101001001, 0b0110100101001001, 0b0001100011111111, 0b1111111101000111, 
        0b1111111101000111, 0b1110101001000111, 
  };
static const uint16_t Melody0_Length    = sizeof( Melody0 ) / sizeof(uint16_t);





/** Play a melody.
 *
 * Provide the MelodyX and MelodyX_Length as well as a tempo for the melody to play it on 
 * tonePin (defined above).
 *
 */
 
void playMelody_Data(const uint16_t MelodyData[], const uint16_t MelodyLength, const uint8_t tempo)
{
  // 8th Octave Frequencies C8 to B8, lower octaves are calculated from this
  static const uint16_t Freq8[] PROGMEM = { 4186 , 4435 , 4699  , 4978 , 5274 , 5588 , 5920 , 6272 , 6645 , 7040 , 7459 , 7902 };

  for(uint16_t  x = 0; x < MelodyLength; x++)
  {
    uint16_t data = pgm_read_word((uint16_t *)&MelodyData[x]);
    if((data & 0xF) == 0xF) 
    {
      noTone(tonePin);
    }
    else
    {
      uint16_t Freq = pgm_read_word(&Freq8[data&0xF]) / ( 1 << (8-(data>>4 & 0xF)) );
      tone(tonePin, Freq);    

    }    
    
    int16_t Duration = data>>8;
    while(Duration--) delay(tempo);
  }
}



/** Play a melody by number, with a given tempo. 
 *
 *  This looks like a big clunky function, but the optimiser will 
 *  "inline" it and this will mean that provided the N you provide
 *  is a constant value, the compiler doesn't need to include the 
 *  data of any of the non-used melodies, and can jump straight into
 *  the appropriate playMelody_Data call.
 *
 */
 
inline static void playMelody(uint8_t melodyNumber, uint8_t tempo)
{
  switch(melodyNumber)
  {
  #if NUMBER_OF_MELODIES > 0
    case 0: { playMelody_Data(Melody0, Melody0_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 1
    case 1: { playMelody_Data(Melody1, Melody1_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 2
    case 2: { playMelody_Data(Melody2, Melody2_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 3
    case 3: { playMelody_Data(Melody3, Melody3_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 4
    case 4: { playMelody_Data(Melody4, Melody4_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 5
    case 5: { playMelody_Data(Melody5, Melody5_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 6
    case 6: { playMelody_Data(Melody6, Melody6_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 7
    case 7: { playMelody_Data(Melody7, Melody7_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 8
    case 8: { playMelody_Data(Melody8, Melody8_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 9
    case 9: { playMelody_Data(Melody9, Melody9_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 10
    case 10: { playMelody_Data(Melody10, Melody10_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 11
    case 11: { playMelody_Data(Melody11, Melody11_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 12
    case 12: { playMelody_Data(Melody12, Melody12_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 13
    case 13: { playMelody_Data(Melody13, Melody13_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 14
    case 14: { playMelody_Data(Melody14, Melody14_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 15
    case 15: { playMelody_Data(Melody15, Melody15_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 16
    case 16: { playMelody_Data(Melody16, Melody16_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 17
    case 17: { playMelody_Data(Melody17, Melody17_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 18
    case 18: { playMelody_Data(Melody18, Melody18_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 19
    case 19: { playMelody_Data(Melody19, Melody19_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 20
    case 20: { playMelody_Data(Melody20, Melody20_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 21
    case 21: { playMelody_Data(Melody21, Melody21_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 22
    case 22: { playMelody_Data(Melody22, Melody22_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 23
    case 23: { playMelody_Data(Melody23, Melody23_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 24
    case 24: { playMelody_Data(Melody24, Melody24_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 25
    case 25: { playMelody_Data(Melody25, Melody25_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 26
    case 26: { playMelody_Data(Melody26, Melody26_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 27
    case 27: { playMelody_Data(Melody27, Melody27_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 28
    case 28: { playMelody_Data(Melody28, Melody28_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 29
    case 29: { playMelody_Data(Melody29, Melody29_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 30
    case 30: { playMelody_Data(Melody30, Melody30_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 31
    case 31: { playMelody_Data(Melody31, Melody31_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 32
    case 32: { playMelody_Data(Melody32, Melody32_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 33
    case 33: { playMelody_Data(Melody33, Melody33_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 34
    case 34: { playMelody_Data(Melody34, Melody34_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 35
    case 35: { playMelody_Data(Melody35, Melody35_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 36
    case 36: { playMelody_Data(Melody36, Melody36_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 37
    case 37: { playMelody_Data(Melody37, Melody37_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 38
    case 38: { playMelody_Data(Melody38, Melody38_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 39
    case 39: { playMelody_Data(Melody39, Melody39_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 40
    case 40: { playMelody_Data(Melody40, Melody40_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 41
    case 41: { playMelody_Data(Melody41, Melody41_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 42
    case 42: { playMelody_Data(Melody42, Melody42_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 43
    case 43: { playMelody_Data(Melody43, Melody43_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 44
    case 44: { playMelody_Data(Melody44, Melody44_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 45
    case 45: { playMelody_Data(Melody45, Melody45_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 46
    case 46: { playMelody_Data(Melody46, Melody46_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 47
    case 47: { playMelody_Data(Melody47, Melody47_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 48
    case 48: { playMelody_Data(Melody48, Melody48_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 49
    case 49: { playMelody_Data(Melody49, Melody49_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 50
    case 50: { playMelody_Data(Melody50, Melody50_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 51
    case 51: { playMelody_Data(Melody51, Melody51_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 52
    case 52: { playMelody_Data(Melody52, Melody52_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 53
    case 53: { playMelody_Data(Melody53, Melody53_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 54
    case 54: { playMelody_Data(Melody54, Melody54_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 55
    case 55: { playMelody_Data(Melody55, Melody55_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 56
    case 56: { playMelody_Data(Melody56, Melody56_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 57
    case 57: { playMelody_Data(Melody57, Melody57_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 58
    case 58: { playMelody_Data(Melody58, Melody58_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 59
    case 59: { playMelody_Data(Melody59, Melody59_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 60
    case 60: { playMelody_Data(Melody60, Melody60_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 61
    case 61: { playMelody_Data(Melody61, Melody61_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 62
    case 62: { playMelody_Data(Melody62, Melody62_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 63
    case 63: { playMelody_Data(Melody63, Melody63_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 64
    case 64: { playMelody_Data(Melody64, Melody64_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 65
    case 65: { playMelody_Data(Melody65, Melody65_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 66
    case 66: { playMelody_Data(Melody66, Melody66_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 67
    case 67: { playMelody_Data(Melody67, Melody67_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 68
    case 68: { playMelody_Data(Melody68, Melody68_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 69
    case 69: { playMelody_Data(Melody69, Melody69_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 70
    case 70: { playMelody_Data(Melody70, Melody70_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 71
    case 71: { playMelody_Data(Melody71, Melody71_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 72
    case 72: { playMelody_Data(Melody72, Melody72_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 73
    case 73: { playMelody_Data(Melody73, Melody73_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 74
    case 74: { playMelody_Data(Melody74, Melody74_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 75
    case 75: { playMelody_Data(Melody75, Melody75_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 76
    case 76: { playMelody_Data(Melody76, Melody76_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 77
    case 77: { playMelody_Data(Melody77, Melody77_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 78
    case 78: { playMelody_Data(Melody78, Melody78_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 79
    case 79: { playMelody_Data(Melody79, Melody79_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 80
    case 80: { playMelody_Data(Melody80, Melody80_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 81
    case 81: { playMelody_Data(Melody81, Melody81_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 82
    case 82: { playMelody_Data(Melody82, Melody82_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 83
    case 83: { playMelody_Data(Melody83, Melody83_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 84
    case 84: { playMelody_Data(Melody84, Melody84_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 85
    case 85: { playMelody_Data(Melody85, Melody85_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 86
    case 86: { playMelody_Data(Melody86, Melody86_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 87
    case 87: { playMelody_Data(Melody87, Melody87_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 88
    case 88: { playMelody_Data(Melody88, Melody88_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 89
    case 89: { playMelody_Data(Melody89, Melody89_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 90
    case 90: { playMelody_Data(Melody90, Melody90_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 91
    case 91: { playMelody_Data(Melody91, Melody91_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 92
    case 92: { playMelody_Data(Melody92, Melody92_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 93
    case 93: { playMelody_Data(Melody93, Melody93_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 94
    case 94: { playMelody_Data(Melody94, Melody94_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 95
    case 95: { playMelody_Data(Melody95, Melody95_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 96
    case 96: { playMelody_Data(Melody96, Melody96_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 97
    case 97: { playMelody_Data(Melody97, Melody97_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 98
    case 98: { playMelody_Data(Melody98, Melody98_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 99
    case 99: { playMelody_Data(Melody99, Melody99_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 100
    case 100: { playMelody_Data(Melody100, Melody100_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 101
    case 101: { playMelody_Data(Melody101, Melody101_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 102
    case 102: { playMelody_Data(Melody102, Melody102_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 103
    case 103: { playMelody_Data(Melody103, Melody103_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 104
    case 104: { playMelody_Data(Melody104, Melody104_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 105
    case 105: { playMelody_Data(Melody105, Melody105_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 106
    case 106: { playMelody_Data(Melody106, Melody106_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 107
    case 107: { playMelody_Data(Melody107, Melody107_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 108
    case 108: { playMelody_Data(Melody108, Melody108_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 109
    case 109: { playMelody_Data(Melody109, Melody109_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 110
    case 110: { playMelody_Data(Melody110, Melody110_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 111
    case 111: { playMelody_Data(Melody111, Melody111_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 112
    case 112: { playMelody_Data(Melody112, Melody112_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 113
    case 113: { playMelody_Data(Melody113, Melody113_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 114
    case 114: { playMelody_Data(Melody114, Melody114_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 115
    case 115: { playMelody_Data(Melody115, Melody115_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 116
    case 116: { playMelody_Data(Melody116, Melody116_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 117
    case 117: { playMelody_Data(Melody117, Melody117_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 118
    case 118: { playMelody_Data(Melody118, Melody118_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 119
    case 119: { playMelody_Data(Melody119, Melody119_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 120
    case 120: { playMelody_Data(Melody120, Melody120_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 121
    case 121: { playMelody_Data(Melody121, Melody121_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 122
    case 122: { playMelody_Data(Melody122, Melody122_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 123
    case 123: { playMelody_Data(Melody123, Melody123_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 124
    case 124: { playMelody_Data(Melody124, Melody124_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 125
    case 125: { playMelody_Data(Melody125, Melody125_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 126
    case 126: { playMelody_Data(Melody126, Melody126_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 127
    case 127: { playMelody_Data(Melody127, Melody127_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 128
    case 128: { playMelody_Data(Melody128, Melody128_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 129
    case 129: { playMelody_Data(Melody129, Melody129_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 130
    case 130: { playMelody_Data(Melody130, Melody130_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 131
    case 131: { playMelody_Data(Melody131, Melody131_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 132
    case 132: { playMelody_Data(Melody132, Melody132_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 133
    case 133: { playMelody_Data(Melody133, Melody133_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 134
    case 134: { playMelody_Data(Melody134, Melody134_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 135
    case 135: { playMelody_Data(Melody135, Melody135_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 136
    case 136: { playMelody_Data(Melody136, Melody136_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 137
    case 137: { playMelody_Data(Melody137, Melody137_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 138
    case 138: { playMelody_Data(Melody138, Melody138_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 139
    case 139: { playMelody_Data(Melody139, Melody139_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 140
    case 140: { playMelody_Data(Melody140, Melody140_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 141
    case 141: { playMelody_Data(Melody141, Melody141_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 142
    case 142: { playMelody_Data(Melody142, Melody142_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 143
    case 143: { playMelody_Data(Melody143, Melody143_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 144
    case 144: { playMelody_Data(Melody144, Melody144_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 145
    case 145: { playMelody_Data(Melody145, Melody145_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 146
    case 146: { playMelody_Data(Melody146, Melody146_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 147
    case 147: { playMelody_Data(Melody147, Melody147_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 148
    case 148: { playMelody_Data(Melody148, Melody148_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 149
    case 149: { playMelody_Data(Melody149, Melody149_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 150
    case 150: { playMelody_Data(Melody150, Melody150_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 151
    case 151: { playMelody_Data(Melody151, Melody151_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 152
    case 152: { playMelody_Data(Melody152, Melody152_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 153
    case 153: { playMelody_Data(Melody153, Melody153_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 154
    case 154: { playMelody_Data(Melody154, Melody154_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 155
    case 155: { playMelody_Data(Melody155, Melody155_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 156
    case 156: { playMelody_Data(Melody156, Melody156_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 157
    case 157: { playMelody_Data(Melody157, Melody157_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 158
    case 158: { playMelody_Data(Melody158, Melody158_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 159
    case 159: { playMelody_Data(Melody159, Melody159_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 160
    case 160: { playMelody_Data(Melody160, Melody160_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 161
    case 161: { playMelody_Data(Melody161, Melody161_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 162
    case 162: { playMelody_Data(Melody162, Melody162_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 163
    case 163: { playMelody_Data(Melody163, Melody163_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 164
    case 164: { playMelody_Data(Melody164, Melody164_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 165
    case 165: { playMelody_Data(Melody165, Melody165_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 166
    case 166: { playMelody_Data(Melody166, Melody166_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 167
    case 167: { playMelody_Data(Melody167, Melody167_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 168
    case 168: { playMelody_Data(Melody168, Melody168_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 169
    case 169: { playMelody_Data(Melody169, Melody169_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 170
    case 170: { playMelody_Data(Melody170, Melody170_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 171
    case 171: { playMelody_Data(Melody171, Melody171_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 172
    case 172: { playMelody_Data(Melody172, Melody172_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 173
    case 173: { playMelody_Data(Melody173, Melody173_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 174
    case 174: { playMelody_Data(Melody174, Melody174_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 175
    case 175: { playMelody_Data(Melody175, Melody175_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 176
    case 176: { playMelody_Data(Melody176, Melody176_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 177
    case 177: { playMelody_Data(Melody177, Melody177_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 178
    case 178: { playMelody_Data(Melody178, Melody178_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 179
    case 179: { playMelody_Data(Melody179, Melody179_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 180
    case 180: { playMelody_Data(Melody180, Melody180_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 181
    case 181: { playMelody_Data(Melody181, Melody181_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 182
    case 182: { playMelody_Data(Melody182, Melody182_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 183
    case 183: { playMelody_Data(Melody183, Melody183_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 184
    case 184: { playMelody_Data(Melody184, Melody184_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 185
    case 185: { playMelody_Data(Melody185, Melody185_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 186
    case 186: { playMelody_Data(Melody186, Melody186_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 187
    case 187: { playMelody_Data(Melody187, Melody187_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 188
    case 188: { playMelody_Data(Melody188, Melody188_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 189
    case 189: { playMelody_Data(Melody189, Melody189_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 190
    case 190: { playMelody_Data(Melody190, Melody190_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 191
    case 191: { playMelody_Data(Melody191, Melody191_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 192
    case 192: { playMelody_Data(Melody192, Melody192_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 193
    case 193: { playMelody_Data(Melody193, Melody193_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 194
    case 194: { playMelody_Data(Melody194, Melody194_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 195
    case 195: { playMelody_Data(Melody195, Melody195_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 196
    case 196: { playMelody_Data(Melody196, Melody196_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 197
    case 197: { playMelody_Data(Melody197, Melody197_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 198
    case 198: { playMelody_Data(Melody198, Melody198_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 199
    case 199: { playMelody_Data(Melody199, Melody199_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 200
    case 200: { playMelody_Data(Melody200, Melody200_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 201
    case 201: { playMelody_Data(Melody201, Melody201_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 202
    case 202: { playMelody_Data(Melody202, Melody202_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 203
    case 203: { playMelody_Data(Melody203, Melody203_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 204
    case 204: { playMelody_Data(Melody204, Melody204_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 205
    case 205: { playMelody_Data(Melody205, Melody205_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 206
    case 206: { playMelody_Data(Melody206, Melody206_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 207
    case 207: { playMelody_Data(Melody207, Melody207_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 208
    case 208: { playMelody_Data(Melody208, Melody208_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 209
    case 209: { playMelody_Data(Melody209, Melody209_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 210
    case 210: { playMelody_Data(Melody210, Melody210_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 211
    case 211: { playMelody_Data(Melody211, Melody211_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 212
    case 212: { playMelody_Data(Melody212, Melody212_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 213
    case 213: { playMelody_Data(Melody213, Melody213_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 214
    case 214: { playMelody_Data(Melody214, Melody214_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 215
    case 215: { playMelody_Data(Melody215, Melody215_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 216
    case 216: { playMelody_Data(Melody216, Melody216_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 217
    case 217: { playMelody_Data(Melody217, Melody217_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 218
    case 218: { playMelody_Data(Melody218, Melody218_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 219
    case 219: { playMelody_Data(Melody219, Melody219_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 220
    case 220: { playMelody_Data(Melody220, Melody220_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 221
    case 221: { playMelody_Data(Melody221, Melody221_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 222
    case 222: { playMelody_Data(Melody222, Melody222_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 223
    case 223: { playMelody_Data(Melody223, Melody223_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 224
    case 224: { playMelody_Data(Melody224, Melody224_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 225
    case 225: { playMelody_Data(Melody225, Melody225_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 226
    case 226: { playMelody_Data(Melody226, Melody226_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 227
    case 227: { playMelody_Data(Melody227, Melody227_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 228
    case 228: { playMelody_Data(Melody228, Melody228_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 229
    case 229: { playMelody_Data(Melody229, Melody229_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 230
    case 230: { playMelody_Data(Melody230, Melody230_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 231
    case 231: { playMelody_Data(Melody231, Melody231_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 232
    case 232: { playMelody_Data(Melody232, Melody232_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 233
    case 233: { playMelody_Data(Melody233, Melody233_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 234
    case 234: { playMelody_Data(Melody234, Melody234_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 235
    case 235: { playMelody_Data(Melody235, Melody235_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 236
    case 236: { playMelody_Data(Melody236, Melody236_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 237
    case 237: { playMelody_Data(Melody237, Melody237_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 238
    case 238: { playMelody_Data(Melody238, Melody238_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 239
    case 239: { playMelody_Data(Melody239, Melody239_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 240
    case 240: { playMelody_Data(Melody240, Melody240_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 241
    case 241: { playMelody_Data(Melody241, Melody241_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 242
    case 242: { playMelody_Data(Melody242, Melody242_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 243
    case 243: { playMelody_Data(Melody243, Melody243_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 244
    case 244: { playMelody_Data(Melody244, Melody244_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 245
    case 245: { playMelody_Data(Melody245, Melody245_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 246
    case 246: { playMelody_Data(Melody246, Melody246_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 247
    case 247: { playMelody_Data(Melody247, Melody247_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 248
    case 248: { playMelody_Data(Melody248, Melody248_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 249
    case 249: { playMelody_Data(Melody249, Melody249_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 250
    case 250: { playMelody_Data(Melody250, Melody250_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 251
    case 251: { playMelody_Data(Melody251, Melody251_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 252
    case 252: { playMelody_Data(Melody252, Melody252_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 253
    case 253: { playMelody_Data(Melody253, Melody253_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 254
    case 254: { playMelody_Data(Melody254, Melody254_Length, tempo); return; }
  #endif
  #if NUMBER_OF_MELODIES > 255
    case 255: { playMelody_Data(Melody255, Melody255_Length, tempo); return; }
  #endif

  }
}



