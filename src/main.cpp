/*
Ed Boiko
1/24/2020
----------
Portcullis is a little box that lives on a door and adds some unsophisticated security
to a room. Keep track of door accesses and/or attach your own callback process w/ the
provided python "driver" (coming soon).

Powered by Arduino & C++.
*/

#include <Arduino.h>

/*
If we want to keep track of this data on the other side of the serial port, 
it may be easier with a delimiter. For this, we can use negative integers.
*/
#define RESET -1
#define WRAP -2
#define ARM -3
#define DISARM -4
#define OPEN -5
#define CLOSE -6
#define START -7

// Globals
// Pins as const unsigned 8-bit int
const uint8_t BTN_RESET_PIN{2};
const uint8_t BTN_ARM_PIN{3};
const uint8_t LED_RED_PIN{5};
const uint8_t LED_YELLOW_PIN{6};
const uint8_t LED_GREEN_PIN{7};
const uint8_t SEGMENT_STB_PIN{8};
const uint8_t SEGMENT_CLK_PIN{9};
const uint8_t SEGMENT_DIO_PIN{10};
const uint8_t LASER_RECEIVE_PIN{11};
const uint8_t LASER_EMIT_PIN{12};

// Counter & flags
unsigned short int counter{0};
bool reset = false;
bool armed = false;

// Hex data for segment display => LSBfirst
const uint8_t DIGITS[] = {
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

/*
    A simple pattern we can shift and animate the display with, 
    or a message of arbitrary length. Only 4 characters fit at once.
*/
uint8_t zero_roll[] = {
    0x00, // Off
    0x3F, // 0
    0x3F, // 0
    0x3F, // 0
};
const uint8_t MESSAGE_LENGTH = sizeof(zero_roll) / sizeof(*zero_roll);

void roll_message(uint8_t message[], const uint8_t length) {
    /* 
        Array shifter for zero_roll. This lets the segment display "scroll" a 
        message longer than 4 characters. 
    */

    int temp = message[0];

    for (int i = 0; i < length-1; i++) {
        message[i] = message[i+1];
    }

    message[length-1] = temp;

}

void display_counter() {
    /*
        https://i.imgur.com/vsRpUjp.png
        https://retrocip.cz/files/tm1638.pdf
        Updates a 4-bit, 7-segment LED display (TM1638):
        Place values are individually calculated & indexed from const global array DIGITS.
        Data is shifted out least significant bit first.
    */

    digitalWrite(SEGMENT_STB_PIN, LOW); 
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0xc0);

    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, DIGITS[counter/1000%10]);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0x00);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, DIGITS[counter/100%10]);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0x00);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, DIGITS[counter/10%10]);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0x00);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, DIGITS[counter%10]);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0x00);

    digitalWrite(SEGMENT_STB_PIN, HIGH);
}

void display_counter(uint8_t message[]) {
    // Overloaded for message argument

    digitalWrite(SEGMENT_STB_PIN, LOW); 
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0xc0);

    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, message[0]);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0x00);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, message[1]);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0x00);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, message[2]);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0x00);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, message[3]);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0x00);

    digitalWrite(SEGMENT_STB_PIN, HIGH);
}

void wrap_counter() {
    // Max supported int for the 4-bit 7-segment LED is 9999, wrap to 1 if we exceed.

    if (counter > 9999) {
        Serial.println(WRAP);
        counter = 1;
        Serial.println(counter);
    }
}

void set_reset() {
    /*
        Runs at least once per mainloop, checking if the user reset the counter.
        If the reset button got pressed, block while it's depressed & set counter
        back to 0 after the button is released. bool reset is set to true, so we 
        don't increment the counter if we reset while armed.
    */

    if (digitalRead(BTN_RESET_PIN) == LOW) {
        delay(10); //Not perfect but easier than writing a debounce for now.
        while (digitalRead(BTN_RESET_PIN) == LOW) {
            // yellow LED on
            digitalWrite(LED_YELLOW_PIN, HIGH);
            // shift message array by 1
            roll_message(zero_roll, MESSAGE_LENGTH);
            // display first four chars in message aray
            display_counter(zero_roll);
            // delay so we can watch it scroll while the button is down
            delay(100);
        }
        digitalWrite(LED_YELLOW_PIN, LOW);
        reset = true;
        counter = 0;
        Serial.println(RESET);
        /*
        Probably the coolest line in the whole program, though useless in the context
        of the mainloop. This is included for use in read_laser(), where we use it to
        break the while conditional and fall back into loop().
        Comment the line below, compile + upload, and notice the counter works almost
        correctly, but not quite. We're reset for that cycle, so we're not going to
        begin counting until the next cycle comes and bool reset is set back to false.
        We'll blink the laser off so we don't meet the conditional anymore, which lets
        the counter seamlessly resume next cycle no matter what. Without a delay, this 
        happens remarkably quick and isn't noticeable to the eye- how fascinating.
        */
        digitalWrite(LASER_EMIT_PIN, LOW);
    }
}

void set_armed() {
    /*
        Pause & resume behavior:
        If the arm button got pressed, block while it's depressed & flip bool armed
        after release. Laser emitter & yellow LED pins are written accordingly.
    */

    if (digitalRead(BTN_ARM_PIN) == LOW) {
        delay(10); //see set_reset()
        while (digitalRead(BTN_ARM_PIN) == LOW);
        armed = !armed;
        (armed) ? Serial.println(ARM) : Serial.println(DISARM);
    }

    if (armed) {
        digitalWrite(LED_YELLOW_PIN, LOW);
        digitalWrite(LASER_EMIT_PIN, HIGH);
    } else {
        digitalWrite(LED_YELLOW_PIN, HIGH);
        digitalWrite(LASER_EMIT_PIN, LOW);
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
    if (digitalRead(LASER_RECEIVE_PIN) == LOW && armed) {
        digitalWrite(LED_GREEN_PIN, LOW);
        digitalWrite(LED_RED_PIN, HIGH);

    // (3)
    } else if (digitalRead(LASER_RECEIVE_PIN) == HIGH && armed) {
        digitalWrite(LED_GREEN_PIN, HIGH);
        digitalWrite(LED_RED_PIN, LOW);
        Serial.println(CLOSE);
        while (digitalRead(LASER_RECEIVE_PIN) == HIGH && armed) {
            set_armed();
            set_reset();
            display_counter();
        }
        
        // (4)
        if (!reset && armed) {
            ++counter;
            Serial.println(OPEN);
            if (counter <= 9999) {
                Serial.println(counter);
            }
        } 

    // (1)
    } else if (!armed) {
        digitalWrite(LED_GREEN_PIN, LOW);
        digitalWrite(LED_RED_PIN, LOW);
    }
}

void activate_display() {
    // Enable segment display from setup()
    digitalWrite(SEGMENT_STB_PIN, LOW);
    shiftOut(SEGMENT_DIO_PIN, SEGMENT_CLK_PIN, LSBFIRST, 0x8f);
    digitalWrite(SEGMENT_STB_PIN, HIGH);
}

void setup() {
    // Digital I/O pin modes:
    pinMode(BTN_RESET_PIN, INPUT);
    pinMode(BTN_ARM_PIN, INPUT);
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_YELLOW_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(SEGMENT_STB_PIN, OUTPUT);
    pinMode(SEGMENT_CLK_PIN, OUTPUT);
    pinMode(SEGMENT_DIO_PIN, OUTPUT);
    pinMode(LASER_RECEIVE_PIN, INPUT);
    pinMode(LASER_EMIT_PIN, OUTPUT);
    // Enable segment display
    activate_display();
    // Enable COM @ low baudrate 
    Serial.begin(9600);
    Serial.println(START);
}

void loop() {
    // Arduino's mainloop
    set_armed();
    set_reset();
    read_laser();
    wrap_counter();
    display_counter();
    delay(25);
}
