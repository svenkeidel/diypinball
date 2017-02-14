/*15ms max 1 per sec*/


const int SIGPINL = 4;
const int BUTTONPINL = 2;
const int SIGPINR = 5;
const int BUTTONPINR = 3;

const int SLINGL = 8;
const int SLINGSWITCHL = 6;
const int SLINGR = 9;
const int SLINGSWITCHR = 7;

int buttonStateL = 0;
int buttonStateR = 0;
int slingSwitchStateL = 0;
int slingSwitchStateR = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(BUTTONPINL, INPUT);
  pinMode(SIGPINL, OUTPUT);
  pinMode(BUTTONPINR, INPUT);
  pinMode(SIGPINR, OUTPUT);
  
  pinMode(SLINGSWITCHL, INPUT);
  pinMode(SLINGL, OUTPUT);
  pinMode(SLINGSWITCHR, INPUT);
  pinMode(SLINGR, OUTPUT);
  
  pinMode(LED_BUILTIN, OUTPUT);
}


unsigned long interval = 1000;
unsigned long highLength = 20;
unsigned long startGrace = 2000;
unsigned long lastTriggerL = 0;
unsigned long lastTriggerR = 0;
bool enableLedL = false;
bool enableLedR = false;

// the loop function runs over and over again forever
void loop() {
  buttonStateL = digitalRead(BUTTONPINL);
  buttonStateR = digitalRead(BUTTONPINR);
  slingSwitchStateL = digitalRead(SLINGSWITCHL);
  slingSwitchStateR = digitalRead(SLINGSWITCHR);
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonStateL == HIGH) {
    digitalWrite(SIGPINL, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else {
    digitalWrite(SIGPINL, LOW);    // turn the LED off by making the voltage LOW
  }

  if (buttonStateR == HIGH) {
    digitalWrite(SIGPINR, HIGH);
  } else {
    digitalWrite(SIGPINR, LOW);
  }

  if (slingSwitchStateL == HIGH && (millis() - lastTriggerL) > interval) {
    lastTriggerL = millis();
  }
  if ((millis() - lastTriggerL) < highLength && millis() > startGrace) {
    digitalWrite(SLINGL, HIGH);
    enableLedL = true;
  }
  else{
    digitalWrite(SLINGL, LOW);
    enableLedL = false;
  }

  if (slingSwitchStateR == HIGH && (millis() - lastTriggerR) > interval) {
    lastTriggerR = millis();
  }
  if ((millis() - lastTriggerR) < highLength && millis() > startGrace) {
    digitalWrite(SLINGR, HIGH);
    enableLedR = true;
  }
  else{
    digitalWrite(SLINGR, LOW);
    enableLedR = false;
  }

  if (buttonStateL == HIGH || buttonStateR == HIGH || enableLedL || enableLedR ) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);
  }
}
