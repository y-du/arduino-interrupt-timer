const int dip_1 = 2;
const int dip_pins[] = {4, 5, 6};
const int led_pin = 7;
const int buzz_pin = 8;

volatile unsigned int seconds;
volatile unsigned int dip_1_state;
int unsigned led_state;
int unsigned buzz_state;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(led_pin, OUTPUT);
    pinMode(dip_1, INPUT);
    for(int i = 0; i < 3; i++){
        pinMode(dip_pins[i], INPUT);
    }
    pinMode(buzz_pin, OUTPUT);
    digitalWrite(buzz_pin, HIGH);
    attachInterrupt(0, dip_1_ISR, CHANGE);
    seconds = 0;
    dip_1_state = 0;
    led_state = 0;
    buzz_state = 0;
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
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void dip_1_ISR() {
    dip_1_state = digitalRead(dip_1);
}

void loop() {
    if (dip_1_state > 0) {
        int option = 0;
        int duration = 0;
        for(int i = 0; i < 3; i++){
            option = (option << 1) | digitalRead(dip_pins[i]);
        }
        if (option == 4) {
            duration = 60;
        } else if (option == 6) {
            duration = 30;
        } else if (option == 7) {
            duration = 180;
        }
        TCNT1 = 0x0BDC;
        TCCR1B |= (1<<CS12);
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        seconds = 0;
        int start = seconds;
        long on_time = 0;
        long off_time = 0;
        while (seconds - start < duration) {
            if ((led_state == 0) && (millis() - off_time >= 70)) {
                digitalWrite(led_pin, HIGH);
                led_state = 1;
                on_time = millis();
            }
            if ((led_state == 1) && (millis() - on_time >= 70)) {
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
        long b_on_time = 0;
        long b_off_time = 0;
        while (dip_1_state > 0) {
            if ((led_state == 0) && (millis() - off_time >= 400)) {
                digitalWrite(led_pin, HIGH);
                led_state = 1;
                on_time = millis();
            }
            if ((buzz_state == 0) && (millis() - b_off_time >= 1)) {
                digitalWrite(buzz_pin, LOW);
                buzz_state = 1;
                b_on_time = millis();
            }
            if ((led_state == 1) && (millis() - on_time >= 800)) {
                digitalWrite(led_pin, LOW);
                led_state = 0;
                off_time = millis();
            }
            if ((buzz_state == 1) && (millis() - b_on_time >= 1)) {
                digitalWrite(buzz_pin, HIGH);
                buzz_state = 0;
                b_off_time = millis();
            }
        }
        if (led_state == 1) {
            digitalWrite(led_pin, LOW);
            led_state = 0;
        }
        if (buzz_state == 1) {
            digitalWrite(buzz_pin, HIGH);
            buzz_state = 0;
        }
    }
}
