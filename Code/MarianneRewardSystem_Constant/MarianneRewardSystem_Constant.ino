// Trinket code

#define MEANHISTORY 50

// digital output states
#define PULSE_OFF 0
#define PULSE_ON 1
#define PULSE_WAIT 2

// analog input states
#define INACTIVE 0
#define ACTIVE 1

// pulse params in ms
#define ON_TIME 10
#define WAIT_TIME 90

// input magnification
//#define INPUT_SCALE 3.0
#define INPUT_SCALE 4.0

// input and output pins
int threshPin = A3;  // pin #3
int lickInput = A2;  // pin #4!
int outPin = 0;

// gather the vars for each lick channel into a structure
typedef struct lickChannel {
  int lickInput;
  int outPin;
  int threshPin;
  int inputVal = 0;
  int pulseRunning = 0;
  int pulseState = PULSE_OFF;
  long nextTransitionTime = 0;
} LickChannel;

LickChannel chan;
long channelMeanTotal;
int channelMeanCount;
int channelMean;

long threshMeanTotal;
int threshMeanCount;
int threshMean;

void setup() {

  //Serial.begin(9600);
  //  analogReference(INTERNAL);
  // assign analog input pins
  chan.lickInput = lickInput;
  pinMode(chan.lickInput, INPUT);
  chan.threshPin = threshPin;
  pinMode(chan.threshPin, INPUT);

  // assign digital output pins
  chan.outPin = outPin;
  pinMode(chan.outPin, OUTPUT);

  // initalize mean arrays
  channelMeanTotal = 0;
  channelMeanCount = 0;
  channelMean = 0;
  threshMeanTotal = 0;
  threshMeanCount = 0;
  threshMean = 0;
}

void loop() {

  // check for analog input over threshold
  if (inputAboveThreshold()) {
    chan.inputVal = ACTIVE;
    chan.pulseState = PULSE_ON;
  } else {
    chan.inputVal = INACTIVE;
    chan.pulseState = PULSE_OFF;
  }

  // tend to the TTL output pulses
  setPulseValue();
}


//////////////////////////////////////////////
int inputAboveThreshold() {
  // read the pins
  int inputVal = analogRead(chan.lickInput);
  inputVal = analogRead(chan.lickInput);
  inputVal = (int)((double)inputVal * INPUT_SCALE);

  int threshVal = analogRead(chan.threshPin);
  threshVal = analogRead(chan.threshPin);


  return (inputVal > threshVal);

  int mean = updateMean(inputVal);
  inputVal = inputVal - mean;
  inputVal = inputVal < 0 ? 0 : inputVal;


  int thresh = updateThresh(threshVal);
  thresh = threshVal - threshMean;
  thresh = thresh < 0 ? 0 : thresh;

  // subtract off the mean from the input and scale it
  inputVal = (int)((double)inputVal * INPUT_SCALE);

  return (inputVal > thresh);
}

//////////////////////////////////////////////
int setPulseValue() {
  if (chan.inputVal == ACTIVE) {
    digitalWrite(chan.outPin, HIGH);
  } else {
    digitalWrite(chan.outPin, LOW);
  }
}

//////////////////////////////////////////////
int updateMean(int value) {
  if (channelMeanCount + 1 >= MEANHISTORY) {
    int newCount = MEANHISTORY / 2;
    int newTotal = channelMeanTotal / 2;
    channelMeanCount = newCount;
    channelMeanTotal = newTotal;
  }

  channelMeanCount++;
  channelMeanTotal += value;

  channelMean = channelMeanTotal / channelMeanCount;

  return channelMean;
}

//////////////////////////////////////////////
int updateThresh(int value) {

  if (chan.inputVal == INACTIVE) {
    if (threshMeanCount + 1 >= MEANHISTORY) {
      int newCount = MEANHISTORY / 2;
      int newTotal = threshMeanTotal / 2;
      threshMeanCount = newCount;
      threshMeanTotal = newTotal;
    }

    threshMeanCount++;
    threshMeanTotal += value;
  }
  threshMean = threshMeanTotal / threshMeanCount;

  return threshMean;
}
