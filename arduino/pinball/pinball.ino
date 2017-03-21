/*15ms max 1 per sec*/


const int SIGPINL = 0; // shift out
const int BUTTONPINL = 0; // shift in
const int SIGPINR = 1; // shift out
const int BUTTONPINR = 1; // shift in

int buttonStateL = 0;
int buttonStateR = 0;

const int SLING_IDLE = 0;
const int SLING_TRIGGERED_HIGH = 1;
const int SLING_TRIGGERED_LOW = 2;

const int PIN_LATCH_IN = 2;
const int PIN_CLOCK_IN = 3;
const int PIN_DATA_IN  = 4;
const int PIN_DISABLE_IN = 5;

const int PIN_LATCH_OUT = 6;
const int PIN_CLOCK_OUT = 7;
const int PIN_DATA_OUT  = 8;
const int PIN_DISABLE_OUT = 9;

const int numSlings = 4;
int slingTrigger[numSlings] = {2, 3, 4, 6}; // shift in
int slingFire[numSlings]    = {2, 3, 4, 6}; // shift out
int slingState[numSlings];
unsigned long lastTriggered[numSlings];

int score = 0;

void initSling(){
  for(int i = 0; i < numSlings; i++) {
    slingState[i] = SLING_IDLE;
    lastTriggered[i] = 0;
    //pinMode(slingTrigger[i], INPUT);
  }
}

/*       triggered
 * IDLE -----------> HIGH
 *   ^                 |
 *   |                 | highLength ms
 *   |   interval ms   V
 *   +--------------- LOW
 */
void slingShot(int n, int interval, int highLength) {
  unsigned long currentTime = millis();
  if(slingState[n] == SLING_IDLE && shiftRead(slingTrigger[n])) {
    slingState[n] = SLING_TRIGGERED_HIGH;
    if(n == 0) {
      // If pop bumper
      score += 10;
    }  else if(n == 3) {
      // If scoop
      score += 50;
    } else {
      // If sling shots
      score += 1;
    }
    lastTriggered[n] = currentTime;
    shiftWrite(slingFire[n], HIGH);
  } else if(slingState[n] == SLING_TRIGGERED_HIGH &&
           currentTime - lastTriggered[n] > highLength) {
    slingState[n] = SLING_TRIGGERED_LOW;
    shiftWrite(slingFire[n], LOW); 
  } else if(slingState[n] == SLING_TRIGGERED_LOW &&
            currentTime - lastTriggered[n] > interval) {
    slingState[n] = SLING_IDLE;
  }
}

const int PLUNGE_IDLE = 0;
const int PLUNGE_DRAINED = 1;
const int PLUNGE_TROUGH_HIGH = 2;
const int PLUNGE_TROUGH_LOW = 3;
const int PLUNGE_HIGH = 4;

int plungerState = PLUNGE_IDLE;
unsigned long plungerTimeStamp;

int plungeTrigger = 5; // shift in
int troughFire = 5; // shift out
int plungeFire = 7; // shift out

void plunger(int drainSettleTime, int troughHighTime, int moveToLaneTime, int plungeHighTime) {
  unsigned long currentTime = millis();
  if(plungerState == PLUNGE_IDLE && shiftRead(plungeTrigger)) {
    plungerState = PLUNGE_DRAINED;
    score = 0;
    plungerTimeStamp = currentTime;
  } else if(plungerState == PLUNGE_DRAINED &&
            currentTime - plungerTimeStamp > drainSettleTime) {
    plungerState = PLUNGE_TROUGH_HIGH;
    plungerTimeStamp = currentTime;
    shiftWrite(troughFire, HIGH);
  } else if(plungerState == PLUNGE_TROUGH_HIGH && 
            currentTime - plungerTimeStamp > troughHighTime) {
    plungerState = PLUNGE_TROUGH_LOW;
    plungerTimeStamp = currentTime;
    shiftWrite(troughFire, LOW);          
  } else if(plungerState == PLUNGE_TROUGH_LOW &&
            currentTime - plungerTimeStamp > moveToLaneTime) {
    plungerState = PLUNGE_HIGH;
    plungerTimeStamp = currentTime;
    shiftWrite(plungeFire, HIGH);
  } else if(plungerState == PLUNGE_HIGH &&
            currentTime - plungerTimeStamp > plungeHighTime) {
    plungerState = PLUNGE_IDLE;
    shiftWrite(plungeFire, LOW);          
  }
}


const int latchDisplay = 11;
const int clockDisplay = 10;
const int dataDisplay = 12;

void setupDisplay() {
  pinMode(latchDisplay, OUTPUT);
  pinMode(dataDisplay, OUTPUT);
  pinMode(clockDisplay, OUTPUT);
}

byte Tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
void updateDisplay(int score) {
    digitalWrite(latchDisplay, LOW);
    for(int i = 0; i < 8; i++) {
      shiftOut(dataDisplay, clockDisplay, MSBFIRST, Tab[score % 10]);
      score = score / 10;
    }
    digitalWrite(latchDisplay, HIGH);
}

byte shiftInput = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(PIN_LATCH_IN, OUTPUT);
  pinMode(PIN_CLOCK_IN, OUTPUT);
  pinMode(PIN_DATA_IN,  INPUT);
  pinMode(PIN_DISABLE_IN, OUTPUT); // unused currently

  pinMode(PIN_LATCH_OUT, OUTPUT);
  pinMode(PIN_CLOCK_OUT, OUTPUT);
  pinMode(PIN_DATA_OUT,  OUTPUT);
  pinMode(PIN_DISABLE_OUT, OUTPUT);
  
  initSling();
  
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(PIN_DISABLE_OUT, LOW); // avoid firing all outputs when arduino is turned on

  setupDisplay();

  Serial.begin(9600); 
}


// the loop function runs over and over again forever
void loop() {
  buttonStateL = shiftRead(BUTTONPINL);
  buttonStateR = shiftRead(BUTTONPINR);

  slingShot(0, 400, 20);
  slingShot(1, 400, 20);
  slingShot(2, 400, 20);
  slingShot(3, 400, 20);
  plunger(1000, 30, 2000, 50);

  updateDisplay(score);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonStateL == HIGH) {
    shiftWrite(SIGPINL, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else {
    shiftWrite(SIGPINL, LOW);    // turn the LED off by making the voltage LOW
  }

  if (buttonStateR == HIGH) {
    shiftWrite(SIGPINR, HIGH);
  } else {
    shiftWrite(SIGPINR, LOW);
  }

  //debug();
  bool ledLit = buttonStateL == HIGH || buttonStateR == HIGH || plungerState == PLUNGE_TROUGH_HIGH || plungerState == PLUNGE_HIGH;
  for(int i = 0; i < numSlings; i++)
    ledLit = ledLit || slingState[i] == SLING_TRIGGERED_HIGH;
  if (ledLit) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
  }
  shiftCommit();
  shiftReadSignals();
}


byte shiftOutput = 0;

void shiftWrite(int pin, int state) {
  bitWrite(shiftOutput, pin, state);
}
void shiftCommit(){
  digitalWrite(PIN_LATCH_OUT, LOW);
  shiftOut(PIN_DATA_OUT, PIN_CLOCK_OUT, MSBFIRST, shiftOutput);
  digitalWrite(PIN_LATCH_OUT, HIGH);
}

bool shiftRead(int pin) {
  return bitRead(shiftInput, pin);
}
void shiftReadSignals(){
  digitalWrite(PIN_LATCH_IN, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_LATCH_IN, HIGH);
  delayMicroseconds(5);
  digitalWrite(PIN_CLOCK_IN, HIGH);
  digitalWrite(PIN_DISABLE_IN, LOW);

  shiftInput = shiftIn(PIN_DATA_IN, PIN_CLOCK_IN, MSBFIRST);

  digitalWrite(PIN_DISABLE_IN, HIGH);
}

void debug(){
  if(shiftOutput != 0){
    Serial.println(shiftOutput,BIN);
  }
  if(shiftInput != 0){
    Serial.println(shiftInput,BIN);
  }
}
