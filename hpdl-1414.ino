/* This is some simple code to write a scrolling message onto the HPDL-1414 device.
 * 
 * HPDL device information:
 *  Data sheet: https://datasheetspdf.com/pdf-file/1252628/Agilent/HPDL-1414/1
 *  
 *  Pin   Pin Name          Macro
 *  1     Data 5            DI_5
 *  2     Data 4            DI_4
 *  3     Active Low Write  N_WR
 *  4     Digit select 1    DS_1
 *  5     Digit select 0    DS_0
 *  6     VDD (supply)      ----
 *  
 *  7     Ground            ----
 *  8     Data 0            DI_0
 *  9     Data 1            DI_1
 *  10    Data 2            DI_2
 *  11    Data 3            DI_3
 *  12    Data 6            DI_6
 *  
 *  Data[6:0] take the least-significant 7 bits of an ascii character.
 *  Note that the supported range of characters is 0x20 to 0x5F
 *    (i.e. make sure you use capital letters in your strings)
 */

//////////
// Device to arduino pin maps
//////////

// DS_x are the digit select pins 
#define DS_1 5
#define DS_0 4
// The active-low write pin
#define N_WR 6
// DI_x are the data pins
#define DI_0 13 
#define DI_1 12 
#define DI_2 11 
#define DI_3 10 
#define DI_4 7
#define DI_5 9 
#define DI_6 8

// How long should each frame last in the scrolling effect
#define FRAME_LEN_MS 100

// How many characters are on the device
#define DISPLAY_NO_CHARS 4
// DS_NO is the number of digit select pins on the device
#define DS_NO 2
// DI_NO is the number of data pins on the device
#define DI_NO 7


unsigned int dataInPins[DI_NO] = {
  DI_0,
  DI_1,
  DI_2,
  DI_3,
  DI_4,
  DI_5,
  DI_6
};

unsigned int addrPins[DS_NO] = {
  DS_0,
  DS_1
};

// setAllOutputs sets the pin with each number specified in the `pins` array to an output
void setAllOutputs(unsigned int* pins, unsigned int no) {
  for (int i = 0; i < no; i++, pins++) {
    pinMode(*pins, OUTPUT);
  }
}

// writeToPins writes a bit of the input number to each of the input pins in order
// starting from the least significant bit.  Note that the `pins` array must be at least noBits
// in length
void writeToPins(unsigned int* pins, byte val, unsigned int noBits) {
  for (int i = 0; i < noBits; i++, pins++) {
    digitalWrite(*pins, (val >> i) & 0x01);
  }
}


// writePos will write the character to the specific position on the display
void writePos(byte digit, byte character) {
  writeToPins(addrPins, digit, DS_NO);
  writeToPins(dataInPins, character, DI_NO);
  digitalWrite(N_WR, LOW);
  delay(5);
  digitalWrite(N_WR, HIGH);
  delay(5);
}

// writeDisplay will write the set of characters to the display.  Note that the 
// input buffer must be at least DISPLAY_NO_CHARS in length
void writeDisplay(char *chars) {
  for (int i = 0; i < DISPLAY_NO_CHARS; i ++) {
    writePos(DISPLAY_NO_CHARS - 1 - i, chars[i]);
  }
}

// blankBuffer will set the input buffer to the 'blank' character.  Note that the
// input buffer must be at least DISPLAY_NO_CHARS in length
void blankBuffer(char * buf) {
  for (int i = 0; i < DISPLAY_NO_CHARS; i++) {
    buf[i] = ' ';
  }
}

// shiftBuffer moves all characters in the buffer one place forwards (i.e. towards zero).
// The zero'th character in the original string is dropped, and the (DISPLAY_NO_CHARS)th 
// character is replaced with `replacement`.
inline void shiftBuffer(char * buf, char replacement) {
  for (int i = 0; i <= DISPLAY_NO_CHARS - 1; i++) {
    buf[i] = buf[i + 1];
  }
  buf[DISPLAY_NO_CHARS-1] = replacement;
}

// scrollMessageIntro will display an effect of the text in message scrolling onto the display
// while the existing display content is scrolled off.  buf must be at least DISPLAY_NO_CHARS
// in length, and *message may be any length but must be terminated with a null character.
char* scrollMessageIntro(char * buf, char *message)  {
  for (int i = 0; i < DISPLAY_NO_CHARS; i++, message++) {
    if (!*message) {
      break;
    }
    shiftBuffer(buf, *message);
    writeDisplay(buf);
    delay(FRAME_LEN_MS);
  }
  return message;
}

// scrollMessageMid will display an effect of the text in the message scrolling across the display.
// The function will continue to scroll text until it reaches a null character. buf must be at least 
// DISPLAY_NO_CHARS in length, and *message may be any length but must be terminated with a null character.
void scrollMessageMid(char * buf, char *message) {
  while (*message) {
    shiftBuffer(buf, *message);
    message++;
    writeDisplay(buf);
    delay(FRAME_LEN_MS);
  }
}

// scrollMessageOut will display an effect of the text in the buffer scrolling off the display
// while being replaced with a space character.  buf must be at least DISPLAY_NO_CHARS in length.
void scrollMessageOut(char * buf) {
  for (int i = 0; i < DISPLAY_NO_CHARS; i++) {
    shiftBuffer(buf, ' ');
    writeDisplay(buf);
    delay(FRAME_LEN_MS);
  }
}

// scrollMessage will display an effect of the text in message scrolling onto, across and then off the display.
// Note that message must be terminated by a null character.
void scrollMessage(char *message) {
  char buf[DISPLAY_NO_CHARS];
  blankBuffer(buf);
  message = scrollMessageIntro(buf, message);
  scrollMessageMid(buf, message);
  scrollMessageOut(buf);
}

void setup() {
  setAllOutputs(addrPins, DS_NO);
  setAllOutputs(dataInPins, DI_NO);

  pinMode(N_WR, OUTPUT);
  digitalWrite(N_WR, HIGH);
}

void loop() {
  scrollMessage("HELLO WORLD!");
}
