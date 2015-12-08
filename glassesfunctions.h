// Glasses-specific functions, may not work with other applications

typedef void (*functionList)(); // definition for list of effect function pointers
extern const byte numEffects;

// Buffers for graphics generation
byte GlassesBits[24][2] = {{0}};    // 24 column x 8 row bit arrays (on/off frame)
byte GlassesPWM[24][9] = {};      // 24 column x 8 row byte array (PWM frame)


// Read and smooth light sensor input
#define brightnessSmoothFactor 0.99
float smoothedBrightness = 0;
void readBrightness() {
  smoothedBrightness = smoothedBrightness * brightnessSmoothFactor + analogRead(3) * (1.0 - brightnessSmoothFactor);
}

// Send contents of bit frame to correct AS1130 registers
// Rough, needs to be streamlined
void writeBitFrame(byte frame, byte bitbuffer) {
  
  Wire.beginTransmission(0x37);
  Wire.write(0xfd);
  Wire.write(frame + memoryONOFFSTART);
  Wire.endTransmission();

  byte tempBits = 0;

  Wire.beginTransmission(0x37);
  Wire.write(0x00);
  for (byte i = 0; i < 12; i++) {
    tempBits = GlassesBits[i][bitbuffer];
    Wire.write(tempBits << 2);
    Wire.write(tempBits >> 6); 
  }
  Wire.endTransmission();

  Wire.beginTransmission(0x30);
  Wire.write(0xfd);
  Wire.write(frame + memoryONOFFSTART);
  Wire.endTransmission();

  Wire.beginTransmission(0x30);
  Wire.write(0x00);
  for (byte i = 0; i < 12; i++) {
    tempBits = GlassesBits[i+12][bitbuffer];
    Wire.write(tempBits << 2);
    Wire.write(tempBits >> 6); 
  }
  Wire.endTransmission();

}

// Send contents of PWM frame to correct AS1130 registers
// Rough, needs to be streamlined
void writePWMFrame(byte frame) {

  Wire.beginTransmission(0x37);
  Wire.write(0xfd);
  Wire.write(frame + memoryBLINKPWMSTART);
  Wire.endTransmission();

  for (int x = 0; x < 12; x++) {
    Wire.beginTransmission(0x37);
    Wire.write(26+x*11);
    for (int y = 0; y < 8; y++) {
      Wire.write(GlassesPWM[x][y]);
    }
    Wire.endTransmission();
  }

  Wire.beginTransmission(0x30);
  Wire.write(0xfd);
  Wire.write(frame + memoryBLINKPWMSTART);
  Wire.endTransmission();

  for (int x = 0; x < 12; x++) {
    Wire.beginTransmission(0x30);
    Wire.write(26+x*11);
    for (int y = 0; y < 8; y++) {
      Wire.write(GlassesPWM[x+12][y]);
    }
    Wire.endTransmission();
  }

}

// Send contents of bit frame to blink AS1130 registers
// Rough, needs to be streamlined
// Mostly used to clear blink frames, since blink is useless
void writeBlinkFrame(byte frame, byte bitbuffer) {

  Wire.beginTransmission(0x37);
  Wire.write(0xfd);
  Wire.write(frame + memoryBLINKPWMSTART);
  Wire.endTransmission();

  Wire.beginTransmission(0x37);
  Wire.write(0x00);
  for (byte i = 0; i < 12; i++) {
    Wire.write(GlassesBits[i][bitbuffer] << 2);
    Wire.write(GlassesBits[i][bitbuffer] >> 6); 
  }
  Wire.endTransmission();

  Wire.beginTransmission(0x30);
  Wire.write(0xfd);
  Wire.write(frame + memoryBLINKPWMSTART);
  Wire.endTransmission();

  Wire.beginTransmission(0x30);
  Wire.write(0x00);
  for (byte i = 0; i < 12; i++) {
    Wire.write(GlassesBits[i+12][bitbuffer] << 2);
    Wire.write(GlassesBits[i+12][bitbuffer] >> 6); 
  }
  Wire.endTransmission();

}

// Set PWM frame all to one value
// Usually used to clear or fill frame
// Bit and PWM frames interact, if you want to do bit graphics you must fill PWM frame
void fillPWMFrame(byte frame, byte value) {
  for (int x = 0; x < 24; x++) {
    for (int y = 0; y < 8; y++) {
      GlassesPWM[x][y] = value;
    }
  }
}

// Set PWM frame all to one value
// Usually used to clear or fill frame
// Bit and PWM frames interact, if you want to do PWM graphics you must fill bit on/off frame
void fillBitFrame(byte frame, byte value) {
  for (int x = 0; x < 24; x++) {
    GlassesBits[x][0] = 255*(value > 0);
    GlassesBits[x][1] = 255*(value > 0);
  }
}

// Fill blink frame with known value
// Usually zero because no one actually wants to use blink
void fillBlinkFrame(byte frame, byte value) {
  for (int x = 0; x < 24; x++) {
    GlassesBits[x][0] = 255*(value > 0);
    GlassesBits[x][1] = 255*(value > 0);
  }
  writeBlinkFrame(frame, 0);

}

// Configure AS1130 chips to ideal startup settings
void glassesInit() {

  byte configOptions = 0;

  // reset chip again  
  configOptions = (0 << AS1130_test_all) |
    (0 << AS1130_auto_test) |
    (0 << AS1130_manual_test) |
    (0 << AS1130_init) |
    (0 << AS1130_shdn);

  setShutdownTest(glassesRight, configOptions);
  setShutdownTest(glassesLeft, configOptions);

  delay(5);

  configOptions = (1 << AS1130_test_all) |
    (1 << AS1130_auto_test) |
    (0 << AS1130_manual_test) |
    (1 << AS1130_init) |
    (1 << AS1130_shdn);

  setShutdownTest(glassesRight, configOptions);
  setShutdownTest(glassesLeft, configOptions);    

  delay(5);

  configOptions = (0 << AS1130_low_vdd_rst) |
                  (0 << AS1130_low_vdd_stat) | 
                  (1 << AS1130_led_error_correction) |
                  (0 << AS1130_dot_corr) |
                  (0 << AS1130_common_addr);

  setConfigs(glassesRight, configOptions, 1);
  setConfigs(glassesLeft, configOptions, 1);
  
  setMovie(glassesRight, 0, 0, 0, 0);
  setMovie(glassesLeft, 0, 0, 0, 0);

  setMovieOptions(glassesRight, 0, 0, 0, 0);
  setMovieOptions(glassesLeft, 0, 0, 0, 0);

  setMovieLooping(glassesRight, 1);
  setMovieLooping(glassesLeft, 1);

  setBrightness(glassesRight, startbrightness);
  setBrightness(glassesLeft, startbrightness);

  configOptions = (0 << AS1130_selected_pic) |
    (0 << AS1130_watchdog) |
    (0 << AS1130_por) |
    (0 << AS1130_overtemp) |
    (0 << AS1130_low_vdd) |
    (0 << AS1130_open_err) |
    (0 << AS1130_short_err) |
    (0 << AS1130_movie_fin);

  setInterruptMask(glassesRight, configOptions);
  setInterruptMask(glassesLeft, configOptions);

  setInterruptFrame(glassesRight, 0);
  setInterruptFrame(glassesLeft, 0);

  setI2CWatchdog(glassesRight, 64, 1);
  setI2CWatchdog(glassesLeft, 64, 1);

  setClockSync(glassesRight, AS1130_clock_speed_1MHz, AS1130_sync_IN);
  setClockSync(glassesLeft, AS1130_clock_speed_1MHz, AS1130_sync_OUT);

  fillPWMFrame(0, 255);
  writePWMFrame(0);
  fillBitFrame(0, 0);
  writeBitFrame(0, 0);
  fillBlinkFrame(0, 0);
  writeBlinkFrame(0, 0);

  setFrame(glassesRight, 0, 1);
  setFrame(glassesLeft, 0, 1); 

}

// When switching from on/off to PWM or vice versa
// Need to blank or fill respectively to correct the masks
void switchDrawType(byte frame, byte enablePWM) {
  if (enablePWM == 0) {
    fillBitFrame(frame,0); writeBitFrame(frame, 0);
    fillPWMFrame(frame,255); writePWMFrame(frame);
  } else if (enablePWM == 1) {
    fillPWMFrame(frame,0); writePWMFrame(frame);
    fillBitFrame(frame,1); writeBitFrame(frame, 0);
  }
}

// Copy contents of PWM array one LED over
// Possible to do in hardware at some future date
void scrollPWM(byte dir) {

  if (dir == 0) {

    for (int i = 1; i < 24; i++) {
      for (int j = 0; j < 8; j++) {
        GlassesPWM[24 - i][j] = GlassesPWM[24-i-1][j];
      }
    }  
  } 
  else if (dir == 1) {
    for (int i = 1; i < 24; i++) {
      for (int j = 0; j < 8; j++) {
        GlassesPWM[i-1][j] = GlassesPWM[i][j];
      }  
    }
  }
}

// Copy contents of bit array one LED over
// Possible to do in hardware at some future date
void scrollBits(byte dir, byte bitbuffer) {

  if (dir == 0) {

    for (int i = 1; i < 24; i++) {
        GlassesBits[24 - i][bitbuffer] = GlassesBits[24-i-1][bitbuffer];
    }  
  } 
  else if (dir == 1) {
    for (int i = 1; i < 24; i++) {
        GlassesBits[i-1][bitbuffer] = GlassesBits[i][bitbuffer];
    }
  }
}



// Fetch font character bitmap from flash
byte charBuffer[8] = {0};
void loadCharBuffer(byte character) {
  for (int i = 0; i< 8; i++) {
    charBuffer[i] = pgm_read_byte(Font[character]+i);
  }
}

// Determine flash address of text string
unsigned int currentStringAddress = 0;
void selectFlashString(byte string) {
  currentStringAddress = pgm_read_word(&stringArray[string]);
}

// Fetch a character value from a text string in flash
char loadStringChar(byte string, int character) {
  return (char) pgm_read_byte(currentStringAddress + character);
}

byte getCIE (byte value) {
  return pgm_read_byte(&cie[value]);
}


// Cycle through several brightness settings
byte brightness = startbrightness;
void cycleBrightness() {

  if (brightness > 204) {
    brightness = 51;
  } else {
      brightness += 51;
  }
  
  byte newBrightness = getCIE(brightness);
  setBrightness(glassesRight, newBrightness);
  setBrightness(glassesLeft, newBrightness);
}


byte qsine(int angle) {

  int cangle = abs(angle)%359;
  
  if (cangle < 90) {
    return pgm_read_byte(&qsinetable[cangle]);
  } else if (cangle < 180) {
    return 255-pgm_read_byte(&qsinetable[179-cangle]);
  } else if (cangle < 270) {
    return 255-pgm_read_byte(&qsinetable[cangle-180]);
  } else {
    return pgm_read_byte(&qsinetable[359-cangle]);
  }

}

// Anti-aliased line algorithm
// Adapted from Michael Abrash http://www.phatcode.net/res/224/files/html/ch42/42-02.html
void wuLine(int X0, int Y0, int X1, int Y1) {

  uint16_t IntensityShift, ErrorAdj, ErrorAcc;
  uint16_t ErrorAccTemp, Weighting, WeightingComplementMask;
  int DeltaX, DeltaY, Temp, XDir;
  
  // make sure line runs from top to bottom
  if (Y0 > Y1) {
    Temp = Y0; Y0 = Y1; Y1 = Temp;
    Temp = X0; X0 = X1; X1 = Temp;
  }
  
  // first pixel
  GlassesPWM[X0][Y0] = 255;
  
  if ((DeltaX = X1 - X0) >= 0) {
    XDir = 1;
  } else {
    XDir = -1;
    DeltaX = -DeltaX;
  }
  
  if ((DeltaY = Y1 - Y0) == 0) {
    // horizontal line
    while (DeltaX-- != 0) {
      X0 += XDir;
      GlassesPWM[X0][Y0] = 255;
    }
    return;
  }
  
  if (DeltaX == 0) {
    // vertical line
    do {
      Y0++;
      GlassesPWM[X0][Y0] = 255;
    } while (--DeltaY != 0);
    return;
  }
  
  if (DeltaX == DeltaY) {
    // diagonal line
    do {
      X0 += XDir;
      Y0++;
      GlassesPWM[X0][Y0] = 255;
    } while (--DeltaY != 0);
    return;
  }
  
  // need an anti-aliased line
  ErrorAcc = 0;
  IntensityShift = 16 - 8;
  WeightingComplementMask = 256 - 1;
  
  if (DeltaY > DeltaX) {
    // y-major line
    ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;
    while (--DeltaY) {
      ErrorAccTemp = ErrorAcc;
      ErrorAcc += ErrorAdj;
      if (ErrorAcc <= ErrorAccTemp) {
        X0 += XDir;
      }
      Y0++;
      Weighting = ErrorAcc >> IntensityShift;
      GlassesPWM[X0][Y0] = getCIE(255 - Weighting);
      GlassesPWM[X0][Y0 + 1] = getCIE(255 - (Weighting ^ WeightingComplementMask));
    }
    GlassesPWM[X1][Y1] = 255;
    return;
  }
  
  ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;
  while (--DeltaX) {
    ErrorAccTemp = ErrorAcc;
    ErrorAcc += ErrorAdj;
    if (ErrorAcc <= ErrorAccTemp) {
      Y0++;
    }
    X0 += XDir;
    Weighting = ErrorAcc >> IntensityShift;
    GlassesPWM[X0][Y0] = getCIE(255 - Weighting);
    GlassesPWM[X0][Y0 + 1] = getCIE(255 - (Weighting ^ WeightingComplementMask));
  }
  GlassesPWM[X1][Y1] = 255;
}

void loadGraphicsFrame(int frame) {

  for (int x = 0; x < 24; x++) {
    GlassesBits[x][0] = pgm_read_byte(Graphics[frame]+x);
  }
 
}

  /*
  readBrightness();
  brightact++;
  if (brightact > 10) {
    brightact = 0;
    //Serial.println(smoothedBrightness);
    
    int autoBright = smoothedBrightness;
    if (autoBright < 50) autoBright = 50;
    if (autoBright > 255) autoBright = 255;
    setBrightness(glassesRight, autoBright);
    setBrightness(glassesLeft, autoBright);
    
  }
  */
