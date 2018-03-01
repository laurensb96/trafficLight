#define PIN_GR 2
#define PIN_BULB 4
#define PIN_SWITCH 7
#define PIN_LED 13

int switchState, switchTransition, ops;
unsigned int timSwitch, timDisco;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_GR, OUTPUT);
  pinMode(PIN_BULB, OUTPUT);
  pinMode(PIN_SWITCH, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(9600);

  cli();

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 1562;// = (16*10^6) / (10*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (digitalRead(PIN_SWITCH))
  {
    case LOW:
      if (switchState == 1) switchTransition++;
      switchState = 0;
      break;
    case HIGH:
      if (switchState == 0) switchTransition++;
      switchState = 1;
      break;
  }
  delay(100);
  switch (ops)
  {
    case 0:
      offOps();
      break;
    case 1:
      normalOps();
      break;
    case 2:
      break;
  }
}

void offOps()
{
  switch (switchState)
  {
    case 0:
      digitalWrite(PIN_BULB, LOW);
      digitalWrite(PIN_GR, LOW);
      break;
    case 1:
      digitalWrite(PIN_BULB, HIGH);
      digitalWrite(PIN_GR, LOW);
      break;
  }
}

void normalOps()
{
  switch (switchState)
  {
    case 0:
      digitalWrite(PIN_BULB, LOW);
      digitalWrite(PIN_GR, HIGH);
      break;
    case 1:
      digitalWrite(PIN_BULB, HIGH);
      digitalWrite(PIN_GR, LOW);
      break;
  }
}

void discoOps()
{
  digitalWrite(PIN_BULB, !digitalRead(PIN_BULB));
  digitalWrite(PIN_GR, !digitalRead(PIN_GR));
}

ISR(TIMER1_COMPA_vect)
{
  if (timSwitch == 20)
  {
    timSwitch = 0;
    Serial.print("switch state: ");
    Serial.println(switchState);
    Serial.print("ops mode: ");
    Serial.println(ops);
    Serial.print("transitions counted: ");
    Serial.println(switchTransition);
    Serial.println();
    if (switchTransition == 2)
    {
      if (ops == 1) ops = 0;
      else ops = 1;
    }
    if (switchTransition == 3)
    {
      ops = 2;
    }
    switchTransition = 0;
  }
  else timSwitch++;
  if (timDisco == 10)
  {
    timDisco = 0;
    discoOps();
  }
  else  timDisco++;
  digitalWrite(PIN_LED, !digitalRead(PIN_LED));
}

