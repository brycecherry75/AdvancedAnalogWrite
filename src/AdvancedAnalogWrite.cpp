#include "AdvancedAnalogWrite.h"

// Arduino Uno, Duemilanove, Diecimila, LilyPad, Mini, Fio, etc
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega48__)

void AdvancedAnalogWriteClass::init(uint8_t pin, uint16_t BitDepth, uint8_t mode, uint8_t polarity) {
  if ((pin == 6 || pin == 5) && (pin == 6 && mode != FastPWM_OCR && mode != PhaseCorrectPWM_OCR && BitDepth == 8) || (pin == 5 && (mode == FastPWM_OCR || mode == PhaseCorrectPWM_OCR || BitDepth == 8))) { // OC0 - will stop millis() and micros() functions and disable delay() but not delayMicroseconds()
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
    if (mode == FastPWM_OCR || mode == PhaseCorrectPWM_OCR) {
      OCR0A = BitDepth;
    }
    pinMode(pin, OUTPUT);
  }
  else if ((pin == 3 || pin == 11) && (pin == 11 && mode != FastPWM_OCR && mode != PhaseCorrectPWM_OCR && BitDepth == 8) || (pin == 3 && (mode == FastPWM_OCR || mode == PhaseCorrectPWM_OCR || BitDepth == 8))) { // OC2
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
    if (mode == FastPWM_OCR || mode == PhaseCorrectPWM_OCR) {
      OCR2A = BitDepth;
    }
    pinMode(pin, OUTPUT);
  }
  else if ((pin == 9 || pin == 10) &&
           ((BitDepth >= 8 && BitDepth <= 10 && (mode == PhaseCorrectPWM_8bit || mode == PhaseCorrectPWM_9bit || mode == PhaseCorrectPWM_10bit || mode == FastPWM_8bit || mode == FastPWM_9bit || mode == FastPWM_10bit)) ||
            (BitDepth >= 3 && BitDepth <= 65535 && (mode == PhaseFrequencyCorrectPWM_ICR || mode == PhaseCorrectPWM_ICR || mode == FastPWM_ICR || (pin == 10 && (mode == PhaseFrequencyCorrectPWM_OCR16bit || mode == PhaseCorrectPWM_OCR16bit || mode == FastPWM_OCR16bit)))))) { // OC1A - resolution higher than 8 bits is supported on this pin
    TIMSK1 = 0; // no interrupts
    TCCR1A &= 0b11111100; // clear WGM10/WGM11
    TCCR1A |= (mode & 0b00000011);
    TCCR1B &= 0b11100111; // clear WGM12/WGM13
    TCCR1B |= ((mode << 1) & 0b00011000);
    if (mode == PhaseFrequencyCorrectPWM_ICR || mode == PhaseCorrectPWM_ICR || mode == FastPWM_ICR) {
      ICR1 = BitDepth;
    }
    else if (mode == PhaseFrequencyCorrectPWM_OCR16bit || mode == PhaseCorrectPWM_OCR16bit || mode == FastPWM_OCR16bit) {
      OCR1A = BitDepth;
    }
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
    pinMode(pin, OUTPUT);
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

uint16_t AdvancedAnalogWriteClass::initWithFrequency(uint8_t pin, uint32_t freq, uint8_t mode, uint8_t polarity, uint32_t *ActualFrequency, uint8_t *PrescalerValue) {
  uint32_t temp = F_CPU;
  uint16_t PWMprescaler = 1;
  if (mode == PhaseCorrectPWM || mode == PhaseCorrectPWM_OCR || mode == PhaseFrequencyCorrectPWM_ICR || mode == PhaseCorrectPWM_ICR || mode == PhaseFrequencyCorrectPWM_OCR16bit || mode == PhaseCorrectPWM_OCR16bit) { // Phase Correct PWM will be half frequency
    temp /= 2;
  }
  if ((pin == 5 || pin == 3) && (mode == FastPWM_OCR || mode == PhaseCorrectPWM_OCR)) { // 8 bit PWM
    if ((temp / freq) <= 256 && (temp / freq) >= 4) {
      PrescalerValue[0] = PS_NONE;
    }
    else if ((temp / freq / 8) <= 256 && (temp / freq / 8) >= 4) {
      PWMprescaler = 8;
      PrescalerValue[0] = PS_8;
    }
    else if (pin == 3 && (temp / freq / 32) <= 256 && (temp / freq / 32) >= 4) { // only supported by OC2x
      PWMprescaler = 32;
      PrescalerValue[0] = PS_32;
    }
    else if ((temp / freq / 64) <= 256 && (temp / freq / 64) >= 4) {
      PWMprescaler = 64;
      PrescalerValue[0] = PS_64;
    }
    else if (pin == 3 && (temp / freq / 128) <= 256 && (temp / freq / 128) >= 4) { // only supported by OC2x
      PWMprescaler = 128;
      PrescalerValue[0] = PS_128;
    }
    else if ((temp / freq / 256) <= 256 && (temp / freq / 256) >= 4) {
      PWMprescaler = 256;
      PrescalerValue[0] = PS_256;
    }
    else if ((temp / freq / 1024) <= 256 && (temp / freq / 1024) >= 4) {
      PWMprescaler = 1024;
      PrescalerValue[0] = PS_1024;
    }
    else { // frequency is out of range (divider is outside (4 to 256) - 1 according to microcontroller datasheet)
      return 0;
    }
  }
  else if ((pin == 9 && (mode == PhaseFrequencyCorrectPWM_ICR || mode == PhaseCorrectPWM_ICR || mode == FastPWM_ICR)) ||
           (pin == 10 && (mode == PhaseFrequencyCorrectPWM_OCR16bit || mode == PhaseFrequencyCorrectPWM_ICR || mode == PhaseCorrectPWM_ICR || mode == FastPWM_ICR || mode == PhaseCorrectPWM_OCR16bit || mode == FastPWM_OCR16bit)))
  { // 16 bit PWM
    if ((temp / freq) <= 65536UL && (temp / freq) >= 4) {
      PrescalerValue[0] = PS_NONE;
    }
    else if ((temp / freq / 8) <= 65536UL && (temp / freq / 8) >= 4) {
      PWMprescaler = 8;
      PrescalerValue[0] = PS_8;
    }
    else if ((temp / freq / 64) <= 65536UL && (temp / freq / 64) >= 4) {
      PWMprescaler = 64;
      PrescalerValue[0] = PS_64;
    }
    else if ((temp / freq / 256) <= 65536UL && (temp / freq / 256) >= 4) {
      PWMprescaler = 256;
      PrescalerValue[0] = PS_256;
    }
    else if ((temp / freq / 1024) <= 65536UL && (temp / freq / 1024) >= 4) {
      PWMprescaler = 1024;
      PrescalerValue[0] = PS_1024;
    }
    else { // frequency is out of range (divider is outside (4 to 65536) - 1 according to microcontroller datasheet)
      return 0;
    }
  }
  else { // not a supported pin or mode supported by this pin
    return 0;
  }
  uint32_t MaximumPWMvalue = (temp / freq / PWMprescaler); // 16000000 / 120000 = 133 rounded down
  uint32_t RoundedUpFrequencyError = (temp / MaximumPWMvalue / PWMprescaler); // now 120300
  uint32_t RoundedDownFrequencyError = (temp / (MaximumPWMvalue + 1) / PWMprescaler); // now 119402
  if (RoundedUpFrequencyError >= freq) { // now 300 - should take this fork for 120 kHz (actual 120.3 kHz) PWM with a 16 MHz CPU
    RoundedUpFrequencyError -= freq;
  }
  else { // now 300
    RoundedUpFrequencyError = (freq - RoundedUpFrequencyError);
  }
  if (RoundedDownFrequencyError >= freq) { // now 598
    RoundedDownFrequencyError -= freq;
  }
  else { // now 598 - should take this fork for 120 kHz (actual 120.3 kHz) PWM with a 16 MHz CPU
    RoundedDownFrequencyError = (freq - RoundedDownFrequencyError);
  }
  if (((pin == 9 || pin == 10) && MaximumPWMvalue < 65536UL) || MaximumPWMvalue < 256) { // overflow will not occur under these conditions
    if (RoundedUpFrequencyError > RoundedDownFrequencyError) {
      MaximumPWMvalue++;
      freq -= RoundedDownFrequencyError;
    }
    else { // should take this fork for 120 kHz (actual 120.3 kHz) PWM with a 16 MHz CPU
      freq += RoundedUpFrequencyError;
    }
  }
  else { // minimize frequency error by not using a higher prescaler on maximum PWM value
    freq += RoundedUpFrequencyError;
  }
  ActualFrequency[0] = freq;
  MaximumPWMvalue--; // frequency divisor at the microcontroller will be (MaximumPWMvalue + 1) and will be no greater than 65535 (16 bit) or 255 (8 bit)
  init(pin, MaximumPWMvalue, mode, polarity);
  return MaximumPWMvalue;
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

void AdvancedAnalogWriteClass::write(uint8_t pin, uint16_t value, uint8_t PWMmode) {
  switch (pin) {
    case 6:
      if (PWMmode == 0 ||
          (PWMmode != FastPWM_OCR && PWMmode != PhaseCorrectPWM_OCR && value <= 255)) {
        OCR0A = value;
      }
      break;
    case 5:
      if (PWMmode == 0 ||
          ((PWMmode == FastPWM || PWMmode == PhaseCorrectPWM) && value <= 255) ||
          ((PWMmode == FastPWM_OCR || PWMmode == PhaseCorrectPWM_OCR) && value <= OCR0A)) {
        OCR0B = value;
      }
      break;
    case 9:
      if (PWMmode == 0 ||
          ((PWMmode == FastPWM_8bit || PWMmode == PhaseCorrectPWM_8bit) && value <= 255) ||
          ((PWMmode == FastPWM_9bit || PWMmode == PhaseCorrectPWM_9bit) && value <= 511) ||
          ((PWMmode == FastPWM_10bit || PWMmode == PhaseCorrectPWM_10bit) && value <= 1023) ||
          ((PWMmode == PhaseFrequencyCorrectPWM_ICR || PWMmode == PhaseCorrectPWM_ICR || PWMmode == FastPWM_ICR) && value <= ICR1)) {
        OCR1A = value;
      }
      break;
    case 10:
      if (PWMmode == 0 ||
          ((PWMmode == FastPWM_8bit || PWMmode == PhaseCorrectPWM_8bit) && value <= 255) ||
          ((PWMmode == FastPWM_9bit || PWMmode == PhaseCorrectPWM_9bit) && value <= 511) ||
          ((PWMmode == FastPWM_10bit || PWMmode == PhaseCorrectPWM_10bit) && value <= 1023) ||
          ((PWMmode == PhaseFrequencyCorrectPWM_ICR || PWMmode == PhaseCorrectPWM_ICR || PWMmode == FastPWM_ICR) && value <= ICR1) ||
          ((PWMmode == PhaseFrequencyCorrectPWM_OCR16bit || PWMmode == PhaseCorrectPWM_OCR16bit || PWMmode == FastPWM_OCR16bit) && value <= OCR1A)) {
        OCR1B = value;
      }
      break;
    case 11:
      if (PWMmode == 0 ||
          (PWMmode != FastPWM_OCR && PWMmode != PhaseCorrectPWM_OCR && value <= 255)) {
        OCR2A = value;
      }
      break;
    case 3:
      if (PWMmode == 0 ||
          ((PWMmode == FastPWM || PWMmode == PhaseCorrectPWM) && value <= 255) ||
          ((PWMmode == FastPWM_OCR || PWMmode == PhaseCorrectPWM_OCR) && value <= OCR2A)) {
        OCR2B = value;
      }
      break;
  }
}

void AdvancedAnalogWriteClass::increment(uint8_t pin, uint16_t value) {
  uint32_t NewValue;
  switch (pin) {
    case 6: // OC0A
      NewValue = OCR0A;
      break;
    case 5: // OC0B
      NewValue = OCR0B;
      break;
    case 9: // OC1A
      NewValue = OCR1A;
      break;
    case 10: // OC1B
      NewValue = OCR1B;
      break;
    case 11: // OC2A
      NewValue = OCR2A;
      break;
    case 3: // OC2B
      NewValue = OCR2B;
      break;
  }
  NewValue += value;
  if (pin == 6 || pin == 5) {
    uint8_t OperatingMode = 0;
    OperatingMode = ((TCCR2A & 0b00000011) | ((TCCR1B >> 1) & 0b00000100) | PWM_8bitParameterMask);
    if (((OperatingMode == FastPWM || OperatingMode == PhaseCorrectPWM) && NewValue <= 255) || (pin == 5 && (OperatingMode == FastPWM_OCR || OperatingMode == PhaseCorrectPWM_OCR) && NewValue <= OCR0A)) {
      if (pin == 6) {
        OCR0A = NewValue;
      }
      else {
        OCR0B = NewValue;
      }
    }
  }
  else if (pin == 9 || pin == 10) {
    uint8_t OperatingMode = 0;
    OperatingMode = ((TCCR1A & 0b00000011) | ((TCCR1B >> 1) & 0b00001100));
    if (((OperatingMode == FastPWM_8bit || OperatingMode == PhaseCorrectPWM_8bit) && NewValue <= 255) ||
        ((OperatingMode == FastPWM_9bit || OperatingMode == PhaseCorrectPWM_9bit) && NewValue <= 511) ||
        ((OperatingMode == FastPWM_10bit || OperatingMode == PhaseCorrectPWM_10bit) && NewValue <= 1023) ||
        ((OperatingMode == PhaseFrequencyCorrectPWM_ICR || OperatingMode == PhaseCorrectPWM_ICR || OperatingMode == FastPWM_ICR) && NewValue <= ICR1) ||
        (pin == 10 && (OperatingMode == PhaseFrequencyCorrectPWM_OCR16bit || OperatingMode == PhaseCorrectPWM_OCR16bit || OperatingMode == FastPWM_OCR16bit) && NewValue <= OCR1A))
    {
      if (pin == 9) {
        OCR1A = NewValue;
      }
      else {
        OCR1B = NewValue;
      }
    }
  }
  else if (pin == 11 || pin == 3) {
    uint8_t OperatingMode = 0;
    OperatingMode = ((TCCR2A & 0b00000011) | ((TCCR1B >> 1) & 0b00000100) | PWM_8bitParameterMask);
    if (((OperatingMode == FastPWM || OperatingMode == PhaseCorrectPWM) && NewValue <= 255) || (pin == 3 && (OperatingMode == FastPWM_OCR || OperatingMode == PhaseCorrectPWM_OCR) && NewValue <= OCR2A)) {
      if (pin == 11) {
        OCR2A = NewValue;
      }
      else {
        OCR2B = NewValue;
      }
    }
  }
}

void AdvancedAnalogWriteClass::decrement(uint8_t pin, uint16_t value) {
  uint32_t NewValue;
  switch (pin) {
    case 6: // OC0A
      NewValue = OCR0A;
      break;
    case 5: // OC0B
      NewValue = OCR0B;
      break;
    case 9: // OC1A
      NewValue = OCR1A;
      break;
    case 10: // OC1B
      NewValue = OCR1B;
      break;
    case 11: // OC2A
      NewValue = OCR2A;
      break;
    case 3: // OC2B
      NewValue = OCR2B;
      break;
  }
  if (value <= NewValue) {
    NewValue -= value;
    switch (pin) {
      case 6: // OC0A
        OCR0A = NewValue;
        break;
      case 5: // OC0B
        OCR0B = NewValue;
        break;
      case 9: // OC1A
        OCR1A = NewValue;
        break;
      case 10: // OC1B
        OCR1B = NewValue;
        break;
      case 11: // OC2A
        OCR2A = NewValue;
        break;
      case 3: // OC2B
        OCR2B = NewValue;
        break;
    }
  }
}

void AdvancedAnalogWriteClass::RestartMillisMicros() { // will start millis() and micros() and reenable delay() functions from the count at the time of disabling
  // values observed after delay() then millis() or micros()
  TCCR0A = 0x03;
  TCNT0 = 0x00; // always incrementing and overflowing
  OCR0A = 0x00;
  OCR0B = 0x00;
  TIFR0 = 0x06;
  TCCR0B = 0x03;
  TIMSK0 = 0x01;
}

uint16_t AdvancedAnalogWriteClass::read(uint8_t pin) {
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

void AdvancedAnalogWriteClass::ReturnAvailablePrescalers(uint8_t pin, uint16_t *AvailablePrescalers, uint8_t *AvailablePrescalers_byte) {
  for (int i = 0; i < AvailablePrescalersPerTimer; i++) {
    AvailablePrescalers[i] = 0;
  }
  switch (pin) {
    case 6: // OC0A
    case 5: // OC0B
    case 9: // OC1A
    case 10: // OC1B
      AvailablePrescalers[0] = 1;
      AvailablePrescalers[1] = 8;
      AvailablePrescalers[2] = 64;
      AvailablePrescalers[3] = 256;
      AvailablePrescalers[4] = 1024;
      AvailablePrescalers_byte[0] = PS_NONE;
      AvailablePrescalers_byte[1] = PS_8;
      AvailablePrescalers_byte[2] = PS_64;
      AvailablePrescalers_byte[3] = PS_256;
      AvailablePrescalers_byte[4] = PS_1024;
      break;
    case 11: // OC2A
    case 3: // OC2B
      AvailablePrescalers[0] = 1;
      AvailablePrescalers[1] = 8;
      AvailablePrescalers[2] = 32;
      AvailablePrescalers[3] = 64;
      AvailablePrescalers[4] = 128;
      AvailablePrescalers[5] = 256;
      AvailablePrescalers[6] = 1024;
      AvailablePrescalers_byte[0] = PS_NONE;
      AvailablePrescalers_byte[1] = PS_8;
      AvailablePrescalers_byte[2] = PS_32;
      AvailablePrescalers_byte[3] = PS_64;
      AvailablePrescalers_byte[4] = PS_128;
      AvailablePrescalers_byte[5] = PS_256;
      AvailablePrescalers_byte[6] = PS_1024;
      break;
  }
}

uint32_t AdvancedAnalogWriteClass::ReturnMaximumPWMvalue(uint8_t pin) {
  uint32_t MaximumValue = 0;
  switch (pin) {
    case 6: // OC0A
    case 5: // OC0B
    case 11: // OC2A
    case 3: // OC2A
      MaximumValue = 255;
      break;
    case 9: // OC1A
    case 10: // OC1B
      MaximumValue = 65535UL;
      break;
  }
  return MaximumValue;
}

bool AdvancedAnalogWriteClass::ExternalClockCapabilityCheck(uint8_t pin) {
  bool CanBeExternallyClocked = false;
  switch (pin) {
    case 6: // OC0A
    case 5: // OC0B
    case 9: // OC1A
    case 10: // OC1B
      CanBeExternallyClocked = true;
      break;
  }
  return CanBeExternallyClocked;
}


uint8_t AdvancedAnalogWriteClass::ReturnTpin(uint8_t pin) {
  uint8_t Tpin = 255;
  switch (pin) {
    case 6: // OC0A
    case 5: // OC0B
      Tpin = 0;
      break;
    case 9: // OC1A
    case 10: // OC1B
      Tpin = 1;
      break;
  }
  return Tpin;
}

void AdvancedAnalogWriteClass::ReturnAvailablePWMmodes(uint8_t pin, uint8_t *AvailablePWMmodes) {
  for (int i = 0; i < AvailablePWMmodesPerTimer; i++) {
    AvailablePWMmodes[i] = 0x00; // stop
  }
  switch (pin) {
    case 6: // OC0A
    case 5: // OC0A
    case 11: // OC2A
    case 3: // OC2B
      AvailablePWMmodes[0] = PhaseCorrectPWM;
      AvailablePWMmodes[1] = FastPWM;
      break;
    case 9: // OC1A
    case 10: // OC1B
      AvailablePWMmodes[0] = PhaseCorrectPWM_8bit;
      AvailablePWMmodes[1] = PhaseCorrectPWM_9bit;
      AvailablePWMmodes[2] = PhaseCorrectPWM_10bit;
      AvailablePWMmodes[3] = FastPWM_8bit;
      AvailablePWMmodes[4] = FastPWM_9bit;
      AvailablePWMmodes[5] = FastPWM_10bit;
      AvailablePWMmodes[6] = PhaseFrequencyCorrectPWM_ICR;
      AvailablePWMmodes[7] = PhaseCorrectPWM_ICR;
      AvailablePWMmodes[8] = FastPWM_ICR;
      break;
  }
  switch (pin) {
    case 5: // OC0B
    case 3: // OC2B
      AvailablePWMmodes[2] = PhaseCorrectPWM_OCR;
      AvailablePWMmodes[3] = FastPWM_OCR;
      break;
    case 10: // OC1B
      AvailablePWMmodes[9] = PhaseFrequencyCorrectPWM_OCR16bit;
      AvailablePWMmodes[10] = PhaseCorrectPWM_OCR16bit;
      AvailablePWMmodes[11] = FastPWM_OCR16bit;
      break;
  }
}

void AdvancedAnalogWriteClass::ResetCounter(uint8_t pin) {
  switch(pin) {
    case 6: // OC0A
    case 5: // OC0B
      TCNT0 = 0;
      break;
    case 9: // OC1A
    case 10: // OC1B
      TCNT1 = 0;
      break;
    case 11: // OC2A
    case 3: // OC2B
      TCNT2 = 0;
      break;
  }
}

#endif

AdvancedAnalogWriteClass AdvancedAnalogWrite;