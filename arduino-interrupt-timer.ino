const int led_pin = 9;
const int dip_1 = 2;
const int dip_2 = 3;
const int buzz_pin = 10;

volatile unsigned long seconds;
volatile unsigned int dip_1_state;
volatile unsigned int dip_2_state;
int unsigned led_state;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(dip_1, INPUT);
  pinMode(dip_2, INPUT);
  pinMode(buzz_pin, OUTPUT);
  attachInterrupt(0, dip_1_ISR, CHANGE);
  attachInterrupt(1, dip_2_ISR, CHANGE);
  seconds = 0;
  dip_1_state = 0;
  dip_2_state = 0;
  led_state = 0;
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 |= (1<<TOIE1);
  /*
  TCNT1 = 0x0BDC;
  TCCR1B |= (1<<CS12);
  */
}

ISR(TIMER1_OVF_vect) {
  TCNT1 = 0x0BDC;
  seconds++;
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void dip_1_ISR() {
  dip_1_state = digitalRead(dip_1);
}

void dip_2_ISR() {
  dip_2_state = digitalRead(dip_2);
}

void loop() {
  if (dip_1_state > 0) {
    TCNT1 = 0x0BDC;
    TCCR1B |= (1<<CS12);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    seconds = 0;
    int start = seconds;
    long on_time = 0;
    long off_time = 0;
    while (seconds - start < 300) {
      if ((led_state == 0) && (millis() - off_time >= 50)) {
        digitalWrite(led_pin, HIGH);
        led_state = 1;
        on_time = millis();
      }
      if ((led_state == 1) && (millis() - on_time >= 50)) {
        digitalWrite(led_pin, LOW);
        led_state = 0;
        off_time = millis();
      }
      if (dip_1_state < 1) {
        break;
      }
    }
    if (led_state == 1) {
        digitalWrite(led_pin, LOW);
        led_state = 0;
    }
    TCCR1B &= ~(1<<CS12);
    digitalWrite(LED_BUILTIN, LOW);
    on_time = 0;
    off_time = 0;
    while (dip_1_state > 0) {
      if ((led_state == 0) && (millis() - off_time >= 400)) {
        digitalWrite(led_pin, HIGH);
        digitalWrite(buzz_pin, HIGH);
        led_state = 1;
        on_time = millis();
      }
      if ((led_state == 1) && (millis() - on_time >= 800)) {
        digitalWrite(led_pin, LOW);
        digitalWrite(buzz_pin, LOW);
        led_state = 0;
        off_time = millis();
      }
    }
    if (led_state == 1) {
        digitalWrite(led_pin, LOW);
        digitalWrite(buzz_pin, LOW);
        led_state = 0;
    }
  }
}
