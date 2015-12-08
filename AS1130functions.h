// Mostly generic functions for AS1130 use

// I2C addresses for AS1130 chips
#define glassesRight        0x37
#define glassesLeft         0x30

// Entry code for changing memory registers
#define registerSelect             0xFD

// Memory Registers
#define memoryONOFFSTART           0x01
#define memoryBLINKPWMSTART        0x40
#define memoryDOTCORRECTION        0x80
#define memoryCONTROLREGISTERS     0xC0

// Control Registers
#define controlPICTURE             0x00
#define controlMOVIE               0x01
#define controlMOVIEMODE           0x02
#define controlFRAMETIME           0x03
#define controlDISPLAYOPTION       0x04
#define controlCURRENTSOURCE       0x05
#define controlAS1130CONFIG        0x06
#define controlINTERRUPTMASK       0x07
#define controlINTERRUPTFRAMEDEF   0x08
#define controlSHUTDOWNOPENSHORT   0x09
#define controlI2CINTERFACEMON     0x0A
#define controlCLKSYNC             0x0B
#define controlINTERRUPTSTATUS     0x0E
#define controlAS1130STATUS        0x0F
#define controlOPENLEDBEGIN        0x20

// Send a value to a device's register
void writeRegister(byte addr, byte reg, byte value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// Select control memory area for subsequent writes
void selectControlMemory(byte address) {
  writeRegister(address, registerSelect, memoryCONTROLREGISTERS);
}

// Select memory frame to display
void setFrame(byte address, byte frame, byte enable) {
  selectControlMemory(address);
  writeRegister(address, controlPICTURE, ((enable > 0) << 6) | (frame & 0b11111));
}

// Set movie display options
void setMovie(byte address, byte startFrame, byte frameCount, byte loopFrame, byte enable) {
  selectControlMemory(address);
  writeRegister(address, controlMOVIE, ((enable > 0) << 6) | (startFrame & 0b11111));
  writeRegister(address, controlMOVIEMODE, (1 << 7) | ((loopFrame > 0) << 6) | (frameCount & 0b11111));
}

// Set movie frame speed and scrolling options
void setMovieOptions(byte address, byte fading, byte scrollDir, byte enableScroll, byte frameDelay) {
  selectControlMemory(address);
  writeRegister(address, controlFRAMETIME, ((fading > 0) << 7) | ((scrollDir > 0) << 6) | ((enableScroll > 0) << 4) | (frameDelay & 0b1111));
}

// Configure movie looping options
void setMovieLooping(byte address, byte loops) {
  selectControlMemory(address);
  writeRegister(address, controlDISPLAYOPTION, ((loops & 0b111) << 5) | 0b1011);
}

// Set brightness level for device
void setBrightness(byte address, byte brightness) {
  selectControlMemory(address);
  writeRegister(address,controlCURRENTSOURCE, brightness);
}

// Set configuration options
#define AS1130_low_vdd_rst           7
#define AS1130_low_vdd_stat          6
#define AS1130_led_error_correction  5
#define AS1130_dot_corr              4
#define AS1130_common_addr           3
void setConfigs(byte address, byte options, byte memConfig) {
  selectControlMemory(address);
  writeRegister(address, controlAS1130CONFIG, options | (memConfig & 0b111));
}

// Configure interrupt mask
#define AS1130_selected_pic    7
#define AS1130_watchdog        6
#define AS1130_por             5
#define AS1130_overtemp        4
#define AS1130_low_vdd         3
#define AS1130_open_err        2
#define AS1130_short_err       1
#define AS1130_movie_fin       0
void setInterruptMask(byte address, byte options) {
  selectControlMemory(address);
  writeRegister(address, controlINTERRUPTMASK, options);
}

// Select movie frame to generate interrupt
void setInterruptFrame(byte address, byte frame) {
  selectControlMemory(address);
  writeRegister(address, controlINTERRUPTFRAMEDEF, (frame & 0b11111));
}

// Configure test/shutdown register
#define AS1130_test_all    4
#define AS1130_auto_test   3
#define AS1130_manual_test 2
#define AS1130_init        1
#define AS1130_shdn        0
void setShutdownTest(byte address, byte options) {
  selectControlMemory(address);
  writeRegister(address, controlSHUTDOWNOPENSHORT, (options & 0b11111));
}

// Configure I2C watchdog
void setI2CWatchdog(byte address, byte timeout, byte enable) {
  selectControlMemory(address);
  writeRegister(address, controlI2CINTERFACEMON, ((timeout & 0b11111) << 1) | (enable > 0));
}

// Configure clock sync
#define AS1130_clock_speed_1MHz    0b00
#define AS1130_clock_speed_500kHz  0b01
#define AS1130_clock_speed_125kHz  0b10
#define AS1130_clock_speed_32kHz   0b11
#define AS1130_sync_OUT            0b10
#define AS1130_sync_IN             0b01
void setClockSync(byte address, byte clockSpeed, byte syncDir) {
  selectControlMemory(address);
  writeRegister(address, controlCLKSYNC, ((clockSpeed & 0b11) << 2) | (syncDir & 0b11));
}
