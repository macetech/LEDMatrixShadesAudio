// Interface with MSGEQ7 chip for audio analysis

#define AUDIODELAY 7

// Pin definitions
#define ANALOGPIN 0
#define STROBEPIN 5
#define RESETPIN A2

// Smooth/average settings
#define SPECTRUMSMOOTH 0.08
#define PEAKDECAY 0.01
#define NOISEFLOOR 60

// AGC settings
#define AGCSMOOTH 0.005
#define GAINUPPERLIMIT 10.0
#define GAINLOWERLIMIT 0.1

// Global variables
int spectrumValue[7];         // holds raw adc values
float spectrumDecay[7] = {0}; // holds time-averaged values
float spectrumPeaks[7] = {0}; // holds peak values

void doAnalogs() {
  // Static variables
  static float beatAvg = 0.0;
  static float gainAGC = 0.0;
  static PROGMEM const float spectrumFactors[7] = {1.0, 1.0, 1.0, 1.0, 1.25, 1.5, 1.75};

  // reset MSQEQ7 to first frequency bin
  digitalWrite(RESETPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(RESETPIN, LOW);

  // store sum of values for AGC
  int analogsum = 0;

  // cycle through each MSGEQ7 bin and read the analog values
  for (int i = 0; i < 7; i++) {

    // set up the MSGEQ7
    digitalWrite(STROBEPIN, LOW);
    delayMicroseconds(40); // to allow the output to settle

    // read the analog value
    spectrumValue[i] = analogRead(ANALOGPIN);
    digitalWrite(STROBEPIN, HIGH);

    // noise floor filter
    if (spectrumValue[i] < NOISEFLOOR) {
      spectrumValue[i] = 0;
    } else {
      spectrumValue[i] -= NOISEFLOOR;
    }

    // apply correction factor per frequency bin
    spectrumValue[i] *= pgm_read_float_near(spectrumFactors + i);

    // prepare average for AGC
    analogsum += spectrumValue[i];

    // apply current gain value
    spectrumValue[i] *= gainAGC;

    // process time-averaged values
    spectrumDecay[i] = (1.0 - SPECTRUMSMOOTH) * spectrumDecay[i] + SPECTRUMSMOOTH * spectrumValue[i];

    // process peak values
    if (spectrumPeaks[i] < spectrumDecay[i]) spectrumPeaks[i] = spectrumDecay[i];
    spectrumPeaks[i] = spectrumPeaks[i] * (1.0 - PEAKDECAY);
  }

  // Calculate audio levels for automatic gain
  beatAvg = (1.0 - AGCSMOOTH) * beatAvg + AGCSMOOTH * (analogsum / 7.0);

  // Calculate gain adjustment factor
  gainAGC = 250.0 / beatAvg;
  if (gainAGC > GAINUPPERLIMIT) gainAGC = GAINUPPERLIMIT;
  if (gainAGC < GAINLOWERLIMIT) gainAGC = GAINLOWERLIMIT;

}
