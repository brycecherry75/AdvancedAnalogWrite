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

const byte FastPWM = 3; // 8 bit only
const byte PhaseCorrectPWM = 1; // 8 bit only
const byte FastPWM_8bit = 5; // Pins 9/10 only
const byte FastPWM_9bit = 6; // Pins 9/10 only
const byte FastPWM_10bit = 7; // Pins 9/10 only
const byte PhaseCorrectPWM_8bit = 1; // Pins 9/10 only
const byte PhaseCorrectPWM_9bit = 2; // Pins 9/10 only
const byte PhaseCorrectPWM_10bit = 3; // Pins 9/10 only

const byte NORMAL = 0;
const byte INVERTED = 1;

// Arduino Uno, Duemilanove, Diecimila, LilyPad, Mini, Fio, etc
class AdvancedAnalogWriteClass {
  public:
    void init(uint8_t pin, uint8_t BitDepth, uint8_t mode, uint8_t polarity);
    void start(uint8_t pin, uint8_t prescaler);
    void stop(uint8_t pin);
    void write(uint8_t pin, uint8_t BitDepth, uint16_t value);
    void RestartMillisMicros(); // re-enables millis()/micros()/delay() from its last count and delay()/delayMicroseconds()
    word read(uint8_t pin);
 
  private:
    // registers for the previous value of millis()/micros() which uses Timer 0 should it be required again
    byte Old_TCCR0A;
    byte Old_TCCR0B;
    byte Old_TCNT0;
    byte Old_OCR0A;
    byte Old_OCR0B;
    byte Old_TIFR0;
    byte Old_TIMSK0;
    bool MillisMicrosStopped = false;

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