const int potPin = A0;
const int interruptPin = 2;
const int switchPin = 4;
const int triacPin = 5;

int potValue = 0;
int oldPotValue = 0;
int switchValue = 0;
int switchState = 0;    //variable as a status flag which shows if the switch is pressed
bool zero = 0;          //variable as a status flag which shows whether a zero crossing point occured

unsigned long lastDebounceTime = 0;   //the last time the switch pin was toggled
unsigned long debounceDelay = 50;     //the debounce time in ms

unsigned long lastPollTime = 0;   //the last time the potentiometer pin was polled
unsigned long pollDelay = 500;    //the time in ms inbetween potentiometer pollings

void setup() {
  Serial.begin(9600);

  pinMode(potPin, INPUT);
  pinMode(interruptPin, INPUT);
  pinMode(triacPin, OUTPUT);
  pinMode(switchPin, INPUT);
  
  //triggers when pin goes from low to high
  attachInterrupt(digitalPinToInterrupt(interruptPin), zerocrossing, RISING);

  //reference: https://create.arduino.cc/projecthub/Marcazzan_M/internal-timers-of-arduino-58f6c9
  //datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf
  TCCR0A = 0;                                 //clear timer0 register (TCCR0A)
  TCCR0A |= (1 << WGM01);                     //set the CTC mode (clear timer on compare match) for timer0 with OCR0A as top (top is the highest value in the count sequence)
  TCCR0B = 0;                                 //clear timer0 register (TCCR0B)
  TCCR0B |= (1 << CS00) | (1 << CS02);        //set prescaler of timer0 to 1024. the timer/counter clock starts when this register is set.
  TIMSK0 |= (1 << OCIE0B) | (1 << OCIE0A);    //activate timer compare match interrupt for OCR0A and OCR0B
  sei();

  potValue = analogRead(potPin);
  oldPotValue = potValue;
}

//external interrupt that is called after every zero crossing point
//uses ATmega328P's 8-bit Timer/Counter0
void zerocrossing() {                         
  OCR0B = map(potValue, 0, 1023, 20, 120);    //set OCR0B value between 20 and 120 (1.344ms - 7.744ms) according to the potentiometer value. this corresponds to 84%-7% duty cycle.
  OCR0A = OCR0B + 1;                          //set OCR0A to OCR0B + 1. this way the TOP is 1 Timer/Counter clock cycle after
  TCNT0 = 0;                                  //clear timer0 counter. Q: do we even neet to reset the counter value though, as we are operating in CTC mode? it should reset when TCNT0's value == OCR0A
  zero = 1;                                   //set zero crossing point flag to 1
}

//compare match interrupt for OCR0B output compare register
ISR(TIMER0_COMPB_vect) {                      
  if ( (zero == 1) && (switchState == 1) ) {
    digitalWrite(triacPin, 1);    //if a zero crossing occurred, activate the triac
    zero = 0;                     //set the zero crossing point flag to 0
  }
}

//compare match interrupt for OCR0A output compare register
ISR(TIMER0_COMPA_vect) {
  digitalWrite(triacPin, 0);    //deactivate the triac at TCNT0 == OCR0A + 1 Timer/Counter cycle
}

void loop() {  
  // switch read and debounce logic
  if ( (millis() - lastDebounceTime) > debounceDelay ) {
    switchValue = digitalRead(switchPin);
    if (switchValue != switchState) {
      switchState = switchValue;
      lastDebounceTime = millis();
      Serial.print("\t switch = ");
      Serial.println(switchState);
    }
  }

//// original potentiometer logic
//  potValue = analogRead(potPin);
//  if ( (potValue != oldPotValue) && (abs(potValue - oldPotValue) > 30) ) {
//    oldPotValue = potValue;
//  }
  
  // don't need to poll potentiometer every loop...
  if ( (millis() - lastPollTime) > pollDelay ) {  
    potValue = analogRead(potPin);
    lastPollTime = millis();
    if ( (potValue != oldPotValue) && (abs(potValue - oldPotValue) > 30) ) {
      oldPotValue = potValue;
      Serial.print("\t potValue = ");
      Serial.println(potValue);
    }
  }
}
