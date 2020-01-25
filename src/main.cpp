/*
Ed Boiko
1/24/2020
Portcullis, powered by Arduino & C++
*/

#include <Arduino.h>

// Global pins as const unsigned 8-bit int
const uint8_t btn_reset_pin{2};
const uint8_t btn_arm_pin{3};
const uint8_t led_red_pin{5};
const uint8_t led_yellow_pin{6};
const uint8_t led_green_pin{7};
const uint8_t segment_stb_pin{8};
const uint8_t segment_clk_pin{9};
const uint8_t segment_dio_pin{10};
const uint8_t laser_receive_pin{11};
const uint8_t laser_emit_pin{12};

// Global counter & flags
unsigned short int counter{0};
bool reset = false;
bool armed = false;

// Global hex data for segment display
const uint8_t digits[] = {
    0x3f, //0
    0x06, //1
    0x5b, //2
    0x4f, //3
    0x66, //4
    0x6d, //5
    0x7d, //6
    0x07, //7
    0x7f, //8
    0x6f  //9
};

void display_counter() {
    /*
        https://i.imgur.com/vsRpUjp.png
        https://retrocip.cz/files/tm1638.pdf
        Updates a 4-bit, 7-segment LED display (TM1638):
            Place values are individually calculated & indexed from global array digits.
            Data is shifted out least significant bit first.
    */

    digitalWrite(segment_stb_pin, LOW); 
    shiftOut(segment_dio_pin, segment_clk_pin, LSBFIRST, 0xc0);

    shiftOut(segment_dio_pin, segment_clk_pin, LSBFIRST, digits[counter/1000%10]);
    shiftOut(segment_dio_pin, segment_clk_pin, LSBFIRST, 0x00);
    shiftOut(segment_dio_pin, segment_clk_pin, LSBFIRST, digits[counter/100%10]);
    shiftOut(segment_dio_pin, segment_clk_pin, LSBFIRST, 0x00);
    shiftOut(segment_dio_pin, segment_clk_pin, LSBFIRST, digits[counter/10%10]);
    shiftOut(segment_dio_pin, segment_clk_pin, LSBFIRST, 0x00);
    shiftOut(segment_dio_pin, segment_clk_pin, LSBFIRST, digits[counter%10]);
    shiftOut(segment_dio_pin, segment_clk_pin, LSBFIRST, 0x00);

    digitalWrite(segment_stb_pin, HIGH);
}

void wrap_counter() {
    /* 
        Max supported int for the 4-bit 7-segment LED is 9999. 
        If we want to keep track of this data on the other side of the 
        serial port, it may be easier with a delimiter, which most likely
        won't remain as a string literal. A hardcoded negative integer might
        work more elegantly with the drivers- it depends on your situation.
    */

    if (counter > 9999) {
        Serial.println("wrap");
        counter = 1;
        Serial.println(counter);
    }
}

void set_reset() {
    /*
        Runs at least once per loop, checking if the user reset the counter.
        If the reset button got pressed, block while it's depressed & set counter
        back to 0 after the button is released. The implicit delay is an attempt
        to negate input jittering. bool reset is set to true, so we don't increment
        the counter if we reset while armed. There's another ugly string literal 
        interpolater like in wrap_counter() that will probably get replaced with a 
        hardcoded negative integer.
    */

    if (digitalRead(btn_reset_pin) == LOW) {
        delay(10);
        while (digitalRead(btn_reset_pin) == LOW) {
            digitalWrite(led_yellow_pin, HIGH);
        }
        digitalWrite(led_yellow_pin, LOW);
        reset = true;
        counter = 0;
        Serial.println("reset");
    }
}

void set_armed() {
    /*
        Pause & resume behavior:
        If the arm button got pressed, block while it's depressed & flip bool armed
        after release. Laser emitter & yellow LED pins are written accordingly.
    */

    if (digitalRead(btn_arm_pin) == LOW) {
        delay(10);
        while (digitalRead(btn_arm_pin) == LOW);
        armed = !armed;
    }

    if (armed) {
        digitalWrite(led_yellow_pin, LOW);
        digitalWrite(laser_emit_pin, HIGH);
    } else {
        digitalWrite(led_yellow_pin, HIGH);
        digitalWrite(laser_emit_pin, LOW);
    }
}

void read_laser() {
    /*
        Monitor the laser:
        (1) If we're not armed, red LED off / green LED off.

        (2) If we're armed & not receiving the laser, red LED on / green LED off.

        (3) If we're armed & receiving the laser, red LED off / green LED on.
            Now we're blocking, and we have two ways out of this loop:
            1. The arm button is pressed and bool armed is flipped.
            2. The beam is discontinued.

            Otherwise, we'll remain in this loop while we're armed and the beam
            is unbroken. From here, the counter can be reset without a subsequent
            iteration, and we'll just update the display accordingly if that occurs.

        (4) If our booleans are in the right place, we'll update the counter & push
            that data over COM.
    */

    reset = false;
    // (2)
    if (digitalRead(laser_receive_pin) == LOW && armed) {
        digitalWrite(led_green_pin, LOW);
        digitalWrite(led_red_pin, HIGH);
    // (3)
    } else if (digitalRead(laser_receive_pin) == HIGH && armed) {
        digitalWrite(led_green_pin, HIGH);
        digitalWrite(led_red_pin, LOW);
        while (digitalRead(laser_receive_pin) == HIGH && armed) {
            set_armed();
            set_reset();
            display_counter();
        }
        // (4)
        if (!reset && armed) {
            ++counter;
            if (counter <= 9999) {
                Serial.println(counter);
            }
        } 
    // (1)
    } else if (!armed) {
        digitalWrite(led_green_pin, LOW);
        digitalWrite(led_red_pin, LOW);
    }
}

void setup() {
    // Digital I/O pin modes:
    pinMode(btn_reset_pin, INPUT);
    pinMode(btn_arm_pin, INPUT);
    pinMode(led_red_pin, OUTPUT);
    pinMode(led_yellow_pin, OUTPUT);
    pinMode(led_green_pin, OUTPUT);
    pinMode(segment_stb_pin, OUTPUT);
    pinMode(segment_clk_pin, OUTPUT);
    pinMode(segment_dio_pin, OUTPUT);
    pinMode(laser_receive_pin, INPUT);
    pinMode(laser_emit_pin, OUTPUT);
    // Enable COM @ low baudrate 
    Serial.begin(9600);
}

void loop() {
    // Arduino's mainloop
    set_armed();
    set_reset();
    read_laser();
    wrap_counter();
    display_counter();
}
