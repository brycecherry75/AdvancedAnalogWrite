#ifndef AdvancedAnalogWrite_h
#define AdvancedAnalogWrite_h

#include <Arduino.h>

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega48__)

// For all Timers/Counters
const byte PS_STOP = 0;
const byte PS_NONE = 1;
// For all Timers/Counters except Timer/Counter 2 requires different values for the following except PS_8
const byte PS_8 = 2;
const byte PS_64 = 3;
const byte PS_256 = 4;
const byte PS_1024 = 5;
// For Timer/Counter 0
const byte T0_EXT_FALLING = 6;
const byte T0_EXT_RISING = 7;
// For Timer/Counter 1
const byte T1_EXT_FALLING = 6;
const byte T1_EXT_RISING = 7;
// For Timer/Counter 2
const byte PS_32 = 13;
const byte PS_128 = 15;

const byte PWM_8bitParameterMask = 0x10; // deal with overlap of definitions of Pins 9/10 and will be masked out on init()
const byte FastPWM = (3 | PWM_8bitParameterMask); // 8 bit only
const byte PhaseCorrectPWM = (1 | PWM_8bitParameterMask); // 8 bit only
const byte FastPWM_OCR = (7 | PWM_8bitParameterMask); // 8 bit only 
const byte PhaseCorrectPWM_OCR = (5 | PWM_8bitParameterMask); // 8 bit only 
const byte FastPWM_8bit = 5; // Pins 9/10 only
const byte FastPWM_9bit = 6; // Pins 9/10 only
const byte FastPWM_10bit = 7; // Pins 9/10 only
const byte PhaseCorrectPWM_8bit = 1; // Pins 9/10 only
const byte PhaseCorrectPWM_9bit = 2; // Pins 9/10 only
const byte PhaseCorrectPWM_10bit = 3; // Pins 9/10 only
const byte PhaseFrequencyCorrectPWM_ICR = 8; // Pins 9/10 only
const byte PhaseCorrectPWM_ICR = 10; // Pins 9/10 only
const byte FastPWM_ICR = 14; // Pins 9/10 only
const byte PhaseFrequencyCorrectPWM_OCR16bit = 9; // Pins 9/10 only
const byte PhaseCorrectPWM_OCR16bit = 11; // Pins 9/10 only
const byte FastPWM_OCR16bit = 15; // Pins 9/10 only
const byte NORMAL = 0;
const byte INVERTED = 1;

// Arduino Uno, Duemilanove, Diecimila, LilyPad, Mini, Fio, etc
class AdvancedAnalogWriteClass {
  public:
    void init(uint8_t pin, uint16_t BitDepth, uint8_t mode, uint8_t polarity);
    void start(uint8_t pin, uint8_t prescaler);
    void stop(uint8_t pin);
    void write(uint8_t pin, uint16_t value, uint8_t PWMmode);
    void increment(uint8_t pin, uint16_t value);
    void decrement(uint8_t pin, uint16_t value);
    void RestartMillisMicros(); // re-enables millis()/micros()/delay() from its last count and delay()/delayMicroseconds()
    uint16_t read(uint8_t pin);
};

    // Arduino Leonardo etc
// #elif defined(__AVR_ATmega32U4__)

    // Arduino Mega etc
// #elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#else
#error "Unknown chip, please edit AdvancedAnalogWrite library with timer+counter definitions"
#endif

extern AdvancedAnalogWriteClass AdvancedAnalogWrite;

#endif