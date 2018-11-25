/*
   Copyright 2018 Yann Dumont

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

const int switch_pin = 2;
const int dip_pins[] = {4, 5, 6};
const int led_pin = 7;
const int buzz_pin = 8;

volatile unsigned int seconds = 0;
volatile int switch_pin_state = 0;
int led_state = 0;
int buzz_state = 0;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(led_pin, OUTPUT);
    pinMode(switch_pin, INPUT);
    for(int i = 0; i < 3; i++){
        pinMode(dip_pins[i], INPUT);
    }
    pinMode(buzz_pin, OUTPUT);
    digitalWrite(buzz_pin, HIGH);
    attachInterrupt(0, switch_pin_ISR, CHANGE);
    TCCR1A = 0;
    TCCR1B = 0;
    TIMSK1 |= (1<<TOIE1);
}

ISR(TIMER1_OVF_vect) {
    TCNT1 = 0x0BDC;
    seconds++;
}

void switch_pin_ISR() {
    switch_pin_state = digitalRead(switch_pin);
}

int option;
int duration;
int start;
long on_time;
long off_time;
long b_on_time;
long b_off_time;

void loop() {
    if (switch_pin_state > 0) {
        option = 0;
        duration = 0;
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
        seconds = 0;
        TCNT1 = 0x0BDC;
        TCCR1B |= (1<<CS12);
        digitalWrite(LED_BUILTIN, HIGH);
        on_time = 0;
        off_time = 0;
        start = seconds;
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
            if (switch_pin_state < 1) {
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
        b_on_time = 0;
        b_off_time = 0;
        while (switch_pin_state > 0) {
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
