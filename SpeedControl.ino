const int potPin = A0;
const int zvcPin = 2;
const int switchPin = 4;
const int triacPin = 5;

int potValue = 0;
int oldPotValue = 0;
int switchValue = 0;
int switchState = 0;
bool zeroCross = 0;                     // Flag to indicate that a zero crossing occured

//unsigned long lastDebounceTime = 0;   // Time switchPin was last toggled
//unsigned long debounceDelay = 50000;  // Desired debounce time in microseconds (0.05s)
//unsigned long lastPollTime = 0;       // Last time the potentiometer pin was polled
//unsigned long pollDelay = 500000;     // Desured delay between potentiometer pollings microseconds (0.5s)

void setup() {
  Serial.begin(9600);

  pinMode(potPin, INPUT);
  pinMode(zvcPin, INPUT);
  pinMode(triacPin, OUTPUT);
  pinMode(switchPin, INPUT);

  potValue = analogRead(potPin);
  oldPotValue = potValue;

  // Triggers interrupt when zvcPin goes from low to high
  attachInterrupt(digitalPinToInterrupt(zvcPin), ZeroCrossing, RISING);

  // ATmega328P's 8-bit Timer/Counter0
  // Reference: https://create.arduino.cc/projecthub/Marcazzan_M/internal-timers-of-arduino-58f6c9
  // Datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf
  TCCR0B = 0;                                 // Clear register
  TCCR0A = 0;                                 // Clear register
  OCR0B = map(potValue, 0, 1023, 20, 120);    // TODO: narrow the mapped range to better fit usable range of motor (motor stalling at OCR0B >= 85)
  OCR0A = OCR0B + 5;                          // OCR0A is TOP, the highest value in the count sequence before TCNT0 resets to 0.
  TCCR0A |= (1 << WGM01);                     // Set timer0 to CTC mode (clear timer on compare match)
  TCCR0B |= (1 << CS00) | (1 << CS02);        // Set timer0 prescaler to 1024. The timer/counter clock starts when this register is set.
  TIMSK0 |= (1 << OCIE0B) | (1 << OCIE0A);    // Enable timer compare match interrupt for OCR0A and OCR0B
  sei();                                      // Enable global interrupt
}

// External interrupt that is called after every zero crossing
void ZeroCrossing() {
  // OCRn = [(clock_speed/prescaler) * (desired time in seconds)] - 1
  OCR0B = map(potValue, 0, 1023, 20, 120);    // Set OCR0B value between 20 and 120 (1.344ms - 7.744ms) according to the potentiometer value
  OCR0A = OCR0B + 5;                          // 5 used to ensure triac gate pulse duration sufficient to latch 
  TCNT0 = 0;                                  // Clear timer0 counter
  zeroCross = 1;                              // Set zero crossing flag to 1
}

// Compare match interrupt for OCR0B output compare register
ISR(TIMER0_COMPB_vect) {
  if ( (zeroCross == 1) && (switchState == 1) ) {
    digitalWrite(triacPin, 1);  // Start pulse to triac gate
  }
}

// Compare match interrupt for OCR0A output compare register
ISR(TIMER0_COMPA_vect) {
  if (zeroCross == 1) {
    digitalWrite(triacPin, 0);  // Stop pulse to triac gate
    zeroCross = 0;
  }
}

void loop() {

  switchValue = digitalRead(switchPin);
  if (switchValue != switchState) {
    switchState = switchValue;
    Serial.print("switch = ");
    Serial.println(switchState);
  }

  potValue = analogRead(potPin);
  if ( (potValue != oldPotValue) && (abs(potValue - oldPotValue) > 30) ) {
    oldPotValue = potValue;
    Serial.print("potValue = ");
    Serial.print(potValue);

    Serial.print("\t OCR0B = ");
    Serial.print(OCR0B);

    Serial.print("\t delay(ms) = ");
    Serial.println((OCR0B + 1) * (1024 / 16e6) * 1000);   // Delay between zero crossing and triac pulse in milliseconds
  }

  /*
    // poll switch with debounce logic
    if ( (micros() - lastDebounceTime) > debounceDelay ) {
    switchValue = digitalRead(switchPin);
    if (switchValue != switchState) {
      switchState = switchValue;
      lastDebounceTime = micros();
      Serial.print("switch = ");
      Serial.print(switchState);
    }
    }

    // poll potentiometer
    if ( (micros() - lastPollTime) > pollDelay ) {
    potValue = analogRead(potPin);
    lastPollTime = micros();
    if ( (potValue != oldPotValue) && (abs(potValue - oldPotValue) > 30) ) {
      oldPotValue = potValue;
      Serial.print("\t potValue = ");
      Serial.println(potValue);
    }
    }
  */
}
