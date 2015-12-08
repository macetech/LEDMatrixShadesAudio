// Graphical pattern functions for glasses

// Global variables
boolean effectInit = false;       // flag for whether an effect has been run previously
unsigned int effectDelay = 0;

// Draw a sine wave on the bit array
// Speeds up and slows down in a cycle
void sines() {

  if (!effectInit) {
    switchDrawType(0, 0);
    effectInit = true;
    effectDelay = 5;
  }

  static float incrval = 0;
  static float incr = 0.3;
  static byte incdir = 1;

  for (int i = 0; i < 24; i++) {
    GlassesBits[i][0] = 3 << (int)(sin(i / 2.0 + incrval) * 3.5 + 3.5);
  }
  writeBitFrame(0, 0);

  incrval += incr;
  if (incdir == 1 ) {
    incr += 0.001;
  }
  else {
    incr -= 0.001;
  }

  if (incr > 0.5) incdir = 0;
  if (incr < 0.1) incdir = 1;

}


// Draw a circular sine plasma

void Plasma() {

  if (!effectInit) {
    switchDrawType(0, 1);
    effectInit = true;
    effectDelay = 1;
  }
  static int plasoffset = 0;
  static float offset  = 0;
  static float plasIncr = -1;
  static float plasVector = 0;

  for (int x = 0; x < 24; x++) {
    for (int y = 0; y < 8; y++) {
      byte brightness = qsine(sqrt((x - 11.5) * (x - 11.5) + (y - 3.5) * (y - 3.5)) * 60 + plasoffset);

      GlassesPWM[x][y] = pgm_read_byte(&cie[brightness]);
    }
  }

  writePWMFrame(0);
  plasoffset += 15;
  if (plasoffset > 359) plasoffset -= 359;

}

#define fadeIncrement 0.9
void fadeAllPWM() {
  for (int x = 0; x < 24; x++) {
    for (int y = 0; y < 8; y++) {
      GlassesPWM[x][y] *= fadeIncrement;
    }
  }
}


// Message scrolling functions
// Initialize / load message string
byte currentCharColumn = 0;
int currentMessageChar = 0;
void initMessage(byte message) {
  currentCharColumn = 0;
  currentMessageChar = 0;
  selectFlashString(message);
  loadCharBuffer(loadStringChar(message, currentMessageChar));
}

// Draw message scrolling across the two arrays
// SCROLL1X is normal scrolling
// SCROLL2X is page-flipping scroll that simulates double horizontal resolution using persistence of vision
#define SCROLL1X 0
#define SCROLL2X 1
void scrollMessage(byte message, byte mode) {

  if (!effectInit) {
    switchDrawType(0, 0);
    initMessage(message);
    effectInit = true;
    if (mode != SCROLL2X) {
      effectDelay = 30;
    } else {
      effectDelay = 10;
    }
  }

  if ((currentCharColumn % 2 == 0) || mode != SCROLL2X) {
    scrollBits(1, 0);
  } else {
    scrollBits(1, 1);
  }

  if ((currentCharColumn % 2 == 1) || mode != SCROLL2X) {
    GlassesBits[23][0] = charBuffer[currentCharColumn];
    writeBitFrame(0, 0);
  } else {
    GlassesBits[23][1] = charBuffer[currentCharColumn];
    writeBitFrame(0, 1);
  }

  currentCharColumn++;
  if (currentCharColumn > 7) {
    currentCharColumn = 0;
    currentMessageChar++;
    char nextChar = loadStringChar(message, currentMessageChar);
    if (nextChar == 0) {
      currentMessageChar = 0;
      nextChar = loadStringChar(message, currentMessageChar);
    }
    loadCharBuffer(nextChar);
  }

}

void sideRain(byte dir) {

  if (!effectInit) {
    switchDrawType(0, 0);
    effectInit = true;
    effectDelay = 20;
  }

  byte tempRain = 0;

  tempRain = (1 << random(0, 8)) | (1 << random(0, 8));

  if (dir == 0) {
    scrollBits(0, 0);
    GlassesBits[0][0] = tempRain;
  } else {
    scrollBits(1, 0);
    GlassesBits[23][0] = tempRain;
  }

  writeBitFrame(0, 0);

}

void rain() {

  if (!effectInit) {
    switchDrawType(0, 0);
    effectInit = true;
    effectDelay = 20;
  }

  for (int i = 0; i < 24; i++) {
    GlassesBits[i][0] <<= 1;
  }

  GlassesBits[random(0, 24)][0] |= 1;
  GlassesBits[random(0, 24)][0] |= 1;

  writeBitFrame(0, 0);

}

typedef struct Stars {
  float xIncr;
  float yIncr;
  float xPos;
  float yPos;
};

Stars stars[10];

void starField() {

  if (!effectInit) {
    switchDrawType(0, 1);
    effectInit = true;
    effectDelay = 1;
  }


  fadeAllPWM();
  for (int i = 0; i < 10; i++) {
    if (abs(stars[i].xIncr) < 0.02 || abs(stars[i].yIncr) < 0.02) {
      stars[i].xPos = 11.5;
      stars[i].yPos = 3.5;
      stars[i].xIncr = random(0, 200) / 100.0 - 1.0;
      stars[i].yIncr = random(0, 200) / 200.0 - 0.5;
    }

    stars[i].xPos += stars[i].xIncr;
    stars[i].yPos += stars[i].yIncr;

    int xPos = (int)stars[i].xPos;
    int yPos = (int)stars[i].yPos;
    if (xPos < 0 || xPos > 23 || yPos < 0 || yPos > 7) {
      stars[i].xIncr = 0;
      stars[i].yIncr = 0;
    } else {
      GlassesPWM[xPos][yPos] = 255;
    }
  }
  writePWMFrame(0);

}



void fullOn() {

  if (!effectInit) {
    switchDrawType(0, 1);
    effectInit = true;
    effectDelay = 50;
  }

  static byte blinkAction = 0;

  if (blinkAction == 0) {
    blinkAction = 255;
  } else {
    blinkAction = 0;
  }

  for (int x = 0; x < 24; x++) {
    for (int y = 0; y < 8; y++) {
      GlassesPWM[x][y] = blinkAction;
    }
  }

  writePWMFrame(0);

}


void slantBars() {

  if (!effectInit) {
    switchDrawType(0, 1);
    effectInit = true;
    effectDelay = 3;
  }

  static int slantPos = 23;

  for (int x = 0; x < 24; x++) {
    for (int y = 0; y < 8; y++) {
      GlassesPWM[x][y] = pgm_read_byte(&cie[(((x + y + (int)slantPos) % 8) * 32)]);
    }
  }

  slantPos--;
  if (slantPos < 0) slantPos = 23;

  writePWMFrame(0);

}

void sparkles() {

  if (!effectInit) {
    switchDrawType(0, 1);
    effectInit = true;
    effectDelay = 1;
  }

  fadeAllPWM();
  for (int i = 0; i < 5; i++) GlassesPWM[random(0, 24)][random(0, 8)] = 255;
  writePWMFrame(0);
}

void rider() {

  if (!effectInit) {
    switchDrawType(0, 1);
    effectInit = true;
    effectDelay = 5;
  }

  static int riderPos = 0;

  fadeAllPWM();

  int tpos;
  if (riderPos < 8) {
    tpos = riderPos;
  } else if (riderPos < 12) {
    tpos = -1;
  } else if (riderPos < 20) {
    tpos = 19 - riderPos;
  } else if (riderPos <= 40) {
    tpos = -1;
  } else if (riderPos > 40) {
    riderPos = 0;
  }

  for (int x = tpos * 3; x < (tpos * 3 + 3); x++) {
    for (int y = 0; y < 8; y++) {
      GlassesPWM[x][y] = pgm_read_byte(&cie[255 * (tpos != -1)]);
    }
  }

  riderPos++;
  writePWMFrame(0);

}


// Simply grab a character from the font and put it in the 8x8 section of both sides of the glasses
void displayChar(int character) {

  if (!effectInit) {
    switchDrawType(0, 0);
    effectInit = true;
    effectDelay = 5;
  }

  loadCharBuffer(character);

  for (int i = 0; i < 8; i++) {
    GlassesBits[i + 1][0] = charBuffer[i];
    GlassesBits[i + 15][0] = charBuffer[i];
  }

  writeBitFrame(0, 0);

}




void fire() {

  if (!effectInit) {
    switchDrawType(0, 1);
    effectInit = true;
    effectDelay = 2;
  }

  static byte lineBuffer[24] = {0};

  byte x;
  for (x = 0; x < 24; x++) {
    GlassesPWM[x][8] = (random(0, 4) == 1) * 255;
  }


  for (int y = 1; y < 9 ; y++) {
    for (x = 0; x < 24; x++) lineBuffer[x] = GlassesPWM[x][y];
    for (x = 0; x < 24; x++) {
      int tempBright = (lineBuffer[(x - 1) % 24] + lineBuffer[(x + 1) % 24] + lineBuffer[x] + GlassesPWM[x][(y + 1) % 9]) / 3.7 - 10;
      if (tempBright < 0) tempBright = 0;
      if (tempBright > 255) tempBright = 255;
      GlassesPWM[x][y - 1] = tempBright;
    }
  }

  writePWMFrame(0);

}

// Awww!
void beatingHearts() {

  if (!effectInit) {
    switchDrawType(0, 0);
    effectInit = true;
    effectDelay = 50;
  }

  static byte currentHeartFrame = 0;


  if (currentHeartFrame < 3) {
    loadGraphicsFrame(currentHeartFrame);
  } else {
    loadGraphicsFrame(5 - currentHeartFrame);
  }

  currentHeartFrame++;
  if (currentHeartFrame > 5) currentHeartFrame = 0;

  writeBitFrame(0, 0);

}

byte eqLevels[12] = {0};
int eqDecay = 0;
int eqRandomizerDelay = 0;
int eqRandomizerCap = 0;
#define eqDecaySpeed 85
#define eqMinInterval 100
#define eqMaxInterval 400

void fakeEQ() {

  // start new pattern
  if (!effectInit) {
    switchDrawType(0, 0);
    effectInit = true;
    eqRandomizerCap = random(0, eqMaxInterval - eqMinInterval) + eqMinInterval;
    effectDelay = 1;
  }

  // decay the eq array at a set interval
  eqDecay++;
  if (eqDecay > eqDecaySpeed) {
    eqDecay = 0;
    for (byte i = 0; i < 12; i++) {
      if (eqLevels[i] > 0) eqLevels[i]--;
    }
  }

  // splash random bars at a semi-random interval
  eqRandomizerDelay++;
  if (eqRandomizerDelay >= eqRandomizerCap) {
    eqRandomizerDelay = 0;
    eqRandomizerCap = random(0, eqMaxInterval - eqMinInterval) + eqMinInterval;
    for (byte i = 0; i < 12; i++) {
      byte eqNewLevel = random(0, 9);
      if (eqLevels[i] < eqNewLevel) eqLevels[i] = eqNewLevel;
    }
  }

  // render the bars if something visible has happened
  if (eqDecay == 0 || eqRandomizerDelay == 0) {
    for (byte i = 0; i < 12; i++) {
      GlassesBits[i * 2][0] = 0xFF << (8 - eqLevels[i]);
      GlassesBits[i * 2 + 1][0] = 0xFF << (8 - eqLevels[i]);
    }
    writeBitFrame(0, 0);
  }
}


byte eqMapping[10] = {0, 0, 1, 2, 3, 3, 4, 5, 6, 6};

void realEQ() {

  // start new pattern
  if (!effectInit) {
    switchDrawType(0, 0);
    effectInit = true;
    effectDelay = 1;
  }

  byte tempSpec = 0;
  byte tempPeak = 0;

  for (byte i = 0; i < 10; i++) {
    tempSpec = spectrumDecay[eqMapping[i]] / 50;
    tempPeak = spectrumPeaks[eqMapping[i]] / 50;
    if (tempSpec > 7) tempSpec = 7;
    if (tempPeak > 7) tempPeak = 7;
    //GlassesBits[i][0] = (0xFF << (7 - tempSpec)) | (0x01 << (7 - tempPeak));
    //GlassesBits[23-i][0] = (0xFF << (7 - tempSpec)) | (0x01 << (7 - tempPeak));
    GlassesBits[i][0] = (0xFF << (7 - tempSpec));
    GlassesBits[23 - i][0] = (0xFF << (7 - tempSpec));
  }
  writeBitFrame(0, 0);
}



void audioRain() {

  if (!effectInit) {
    switchDrawType(0, 0);
    effectInit = true;
    effectDelay = 15;
  }



  byte tempRain = 0;

  tempRain = ((spectrumDecay[1] + spectrumValue[2] + spectrumValue[3]) / 3.0) / 50;
  if (tempRain > 7) tempRain = 7;

  scrollBits(1, 0);
  GlassesBits[23][0] =  0x03 << (7 - tempRain);


  writeBitFrame(0, 0);


}

void bigVU() {

  // start new pattern
  if (!effectInit) {
    switchDrawType(0, 0);
    effectInit = true;
    effectDelay = 1;
  }

  int tempSpec = ((spectrumDecay[0] + spectrumValue[1] + spectrumValue[2]) / 3.0) / 50;
  if (tempSpec > 7) tempSpec = 7;


  for (byte i = 0; i < 24; i++) {
    GlassesBits[i][0] = 0xFF << (7 - tempSpec);
  }
  writeBitFrame(0, 0);
}


void audioHearts() {

  // start new pattern
  if (!effectInit) {
    switchDrawType(0, 0);
    effectInit = true;
    effectDelay = 1;
  }

  int tempSpec = ((spectrumDecay[0] + spectrumDecay[1] + spectrumDecay[2] + spectrumValue[3]) / 4.0) / 100;
  if (tempSpec > 3) tempSpec = 3;

  if (tempSpec == 0) {
    fillBitFrame(0, 0);
  } else {
    loadGraphicsFrame(tempSpec - 1);
  }
  writeBitFrame(0, 0);

}

void fillAudioPWM() {

  if (!effectInit) {
    switchDrawType(0, 1);
    effectInit = true;
    effectDelay = 1;
  }

  int tempSpec = ((spectrumDecay[0] + spectrumValue[1] + spectrumValue[2]) / 3.0) / 2;
  if (tempSpec > 255) tempSpec = 255;

  fillPWMFrame(0, getCIE(tempSpec));
  writePWMFrame(0);


}




