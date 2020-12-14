#include "AdvancedAnalogWrite.h"

// Arduino Uno, Duemilanove, Diecimila, LilyPad, Mini, Fio, etc
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega48__)

void AdvancedAnalogWriteClass::init(uint8_t pin, uint8_t BitDepth, uint8_t mode, uint8_t polarity) {
  if ((pin == 6 || pin == 5) && BitDepth == 8) { // OC0 - will stop millis() and micros() functions and disable delay() but not delayMicroseconds()
    if (MillisMicrosStopped == false) {
      MillisMicrosStopped = true;
      Old_TIMSK0 = TIMSK0;
      TIMSK0 = 0; // disabling the interrupt first will ensure proper restoration
      Old_TCCR0B = TCCR0B;
      TCCR0B = 0;
      Old_TIFR0 = TIFR0;
      TIFR0 = 0;
      Old_OCR0B = OCR0B;
      OCR0B = 0;
      Old_OCR0A = OCR0A;
      OCR0A = 0;
      Old_TCNT0 = TCNT0;
      TCNT0 = 0;
      Old_TCCR0A = TCCR0A;
      TCCR0A = 0;
    }
    pinMode(pin, OUTPUT);
    TIMSK0 = 0; // no interrupts
    TCCR0A &= 0b11111100; // clear WGM00/WGM01
    TCCR0A |= (mode & 0b00000011);
    TCCR0B &= 0b11110111; // clear WGM02
    TCCR0B |= ((mode << 1) & 0b00001000);
    if (pin == 6) { // pin 6/OC0A
      if (polarity == NORMAL) {
        TCCR0A &= 0b00111111;
        TCCR0A |= 0b10000000;
      }
      else if (polarity == INVERTED) {
        TCCR0A &= 0b00111111;
        TCCR0A |= 0b11000000;
      }
    }
    else { // pin 5/OC0B
      if (polarity == NORMAL) {
        TCCR0A &= 0b11001111;
        TCCR0A |= 0b00100000;
      }
      else if (polarity == INVERTED) {
        TCCR0A &= 0b11001111;
        TCCR0A |= 0b00110000;
      }
    }
  }
  else if ((pin == 3 || pin == 11) && BitDepth == 8) { // OC2
    pinMode(pin, OUTPUT);
    TIMSK2 = 0; // no interrupts
    TCCR2A &= 0b11111100; // clear WGM20/WGM21
    TCCR2A |= (mode & 0b00000011);
    TCCR2B &= 0b11110111; // clear WGM22
    TCCR2B |= ((mode << 1) & 0b00001000);
    if (pin == 11) { // pin 11/OC2A
      if (polarity == NORMAL) {
        TCCR2A &= 0b00111111;
        TCCR2A |= 0b10000000;
      }
      else if (polarity == INVERTED) {
        TCCR2A &= 0b00111111;
        TCCR2A |= 0b11000000;
      }
    }
    else { // pin 3/OC2B
      if (polarity == NORMAL) {
        TCCR2A &= 0b11001111;
        TCCR2A |= 0b00100000;
      }
      else if (polarity == INVERTED) {
        TCCR2A &= 0b11001111;
        TCCR2A |= 0b00110000;
      }
    }
  }
  else if ((pin == 9 || pin == 10) && BitDepth >= 8 && BitDepth <= 10) { // OC1A - resolution higher than 8 bits is supported on this pin
    pinMode(pin, OUTPUT);
    TIMSK1 = 0; // no interrupts
    TCCR1A &= 0b11111100; // clear WGM10/WGM11
    TCCR1A |= (mode & 0b00000011);
    TCCR1B &= 0b11100111; // clear WGM12/WGM13
    TCCR1B |= ((mode << 1) & 0b00011000);
    if (pin == 9) { // pin 9/OC1A
      if (polarity == NORMAL) {
        TCCR1A &= 0b00111111;
        TCCR1A |= 0b10000000;
      }
      else if (polarity == INVERTED) {
        TCCR1A &= 0b00111111;
        TCCR1A |= 0b11000000;
      }
    }
    else { // pin 10/OC1B
      if (polarity == NORMAL) {
        TCCR1A &= 0b11001111;
        TCCR1A |= 0b00100000;
      }
      else if (polarity == INVERTED) {
        TCCR1A &= 0b11001111;
        TCCR1A |= 0b00110000;
      }
    }
  }
}

void AdvancedAnalogWriteClass::start(uint8_t pin, uint8_t prescaler) {
  if ((pin == 6 || pin == 5) && prescaler <= 7) { // OC0A
    TCCR0B &= 0b11111000;
    if (prescaler == T0_EXT_FALLING || prescaler == T0_EXT_RISING) { // avoid unwanted PWM pulse
      pinMode(4, INPUT_PULLUP);
    }
    TCCR0B |= prescaler;
  }
  else if ((pin == 9 || pin == 10) && prescaler <= 7) { // OC1A/OC1B
    TCCR1B &= 0b11111000; // do not touch WGM12/WGM13 bits
    if (prescaler == T1_EXT_FALLING || prescaler == T1_EXT_RISING) { // avoid unwanted PWM pulse
      pinMode(5, INPUT_PULLUP);
    }
    TCCR1B |= prescaler;
  }
  else if (pin == 3 || pin == 11) { // OC2A/OC2B
    switch (prescaler) { // required since the values for a particular ratio are different for Timer/Counter 2 since it does not support external clocking
      case PS_32:
        prescaler = 3;
        break;
      case PS_64:
        prescaler = 4;
        break;
      case PS_128:
        prescaler = 5;
        break;
      case PS_256:
        prescaler = 6;
        break;
      case PS_1024:
        prescaler = 7;
        break;
    }
    if (prescaler <= 7) {
      TCCR2B &= 0b11111000;
      TCCR2B |= prescaler;
    }
  }
}

void AdvancedAnalogWriteClass::stop(uint8_t pin) {
  switch (pin) {
    case 6:
      TCCR0A &= 0b00111111; // disconnect the pin
      break;
    case 5:
      TCCR0A &= 0b11001111; // disconnect the pin
      break;
    case 9:
      TCCR1A &= 0b00111111; // disconnect the pin
      break;
    case 10:
      TCCR1A &= 0b11001111; // disconnect the pin
      break;
    case 11:
      TCCR2A &= 0b00111111; // disconnect the pin
      break;
    case 3:
      TCCR2A &= 0b11001111; // disconnect the pin
      break;
  }
}

void AdvancedAnalogWriteClass::write(uint8_t pin, uint8_t BitDepth, uint16_t value) {
  switch (pin) {
    case 6:
      OCR0A = value;
      break;
    case 5:
      OCR0B = value;
      break;
    case 9:
      OCR1A = value;
      break;
    case 10:
      OCR1B = value;
      break;
    case 11:
      OCR2A = value;
      break;
    case 3:
      OCR2B = value;
      break;
  }
}

void AdvancedAnalogWriteClass::RestartMillisMicros() { // will start millis() and micros() and reenable delay() functions from the count at the time of disabling
  if (MillisMicrosStopped == true) { // avoid writing values which have not been backed up
    TCCR0A = 0;
    TCNT0 = 0;
    OCR0A = 0;
    OCR0B = 0;
    TIFR0 = 0;
    TCCR0B = 0;
    TIMSK0 = 0;
    TCCR0A = Old_TCCR0A;
    TCNT0 = Old_TCNT0;
    OCR0A = Old_OCR0A;
    OCR0B = Old_OCR0B;
    TIFR0 = Old_TIFR0;
    TCCR0B = Old_TCCR0B;
    TIMSK0 = Old_TIMSK0; // enabling the interrupt last will ensure proper restoration
    MillisMicrosStopped = false;
  }
}

word AdvancedAnalogWriteClass::read(uint8_t pin) {
  word value = 0;
  switch (pin) {
    case 6:
      value = OCR0A;
      break;
    case 5:
      value = OCR0B;
      break;
    case 9:
      value = OCR1A;
      break;
    case 10:
      value = OCR1B;
      break;
    case 11:
      value = OCR2A;
      break;
    case 3:
      value = OCR2B;
      break;
  }
  return value;
}

#endif

AdvancedAnalogWriteClass AdvancedAnalogWrite;