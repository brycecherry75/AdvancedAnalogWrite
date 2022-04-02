/*

   AdvancedAnalogWrite demo by Bryce Cherry

   Command format:
   MILLIS_MICROS_START (DELAY_TEST): Stop PWM on OC0x pins and restart millis()/micros()/delay() - DELAY_TEST will test delay
   pin STOP: Stop PWM on a pin
   pin READ: Read current PWM value from a pin with a given bit resolution
   pin (8/9/10/(3-65535)) (PHASE_CORRECT/FAST/PHASE_FREQ_CORRECT) (NORMAL/INVERTED) (T_EXT_RISING/T_EXT_FALLING/1/8/32/64/128/256/1024) (0-65535) for 8/9/10 bit) (ICR/OCR): Initialize pin, bit resolution (if ICR/OCR option is used, maximum PWM value), PWM type, polarity, prescaler, PWM value, OCR/ICR option
   pin INCREMENT value - increments PWM by a given value on a given pin (will not overflow)
   pin DECREMENT value - decrements PWM by a given value on a given pin (will not underflow)
   pin VARIABLE_FREQ frequency (PHASE_CORRECT/FAST/PHASE_FREQ_CORRECT) (NORMAL/INVERTED) (ICR/OCR) value: Initialize pin, set frequency, mode, value
   pin QUERY: Querys the capabilities of a pin

*/

#include <AdvancedAnalogWrite.h>

// ensures that the serial port is flushed fully on request
const unsigned long SerialPortRate = 9600;
const byte SerialPortRateTolerance = 5; // percent - increase to 50 for rates above 115200 up to 4000000
const byte SerialPortBits = 10; // start (1), data (8), stop (1)
const unsigned long TimePerByte = ((((1000000ULL * SerialPortBits) / SerialPortRate) * (100 + SerialPortRateTolerance)) / 100); // calculated on serial port rate + tolerance and rounded down to the nearest uS, long caters for even the slowest serial port of 75 bps

const byte commandSize = 75;
char command[commandSize];
const byte FieldSize = 25;

void getField (char* buffer, int index) {
  int CommandPos = 0;
  int FieldPos = 0;
  int SpaceCount = 0;
  while (CommandPos < commandSize) {
    if (command[CommandPos] == 0x20) {
      SpaceCount++;
      CommandPos++;
    }
    if (command[CommandPos] == 0x0D || command[CommandPos] == 0x0A) {
      break;
    }
    if (SpaceCount == index) {
      buffer[FieldPos] = command[CommandPos];
      FieldPos++;
    }
    CommandPos++;
  }
  for (int ch = 0; ch < strlen(buffer); ch++) { // correct case of command
    buffer[ch] = toupper(buffer[ch]);
  }
  buffer[FieldPos] = '\0';
}

void FlushSerialBuffer() {
  while (true) {
    if (Serial.available() > 0) {
      byte dummy = Serial.read();
      while (Serial.available() > 0) { // flush additional bytes from serial buffer if present
        dummy = Serial.read();
      }
      if (TimePerByte <= 16383) {
        delayMicroseconds(TimePerByte); // delay in case another byte may be received via the serial port
      }
      else { // deal with delayMicroseconds limitation
        unsigned long DelayTime = TimePerByte;
        DelayTime /= 1000;
        if (DelayTime > 0) {
          for (int i = 0; i < DelayTime; i++) {
            delayMicroseconds(1000);
          }
        }
        DelayTime = TimePerByte;
        DelayTime %= 1000;
        if (DelayTime > 0) {
          delayMicroseconds(DelayTime);
        }
      }
    }
    else {
      break;
    }
  }
}

void PrintFieldError(byte value) {
  Serial.print(F("FIELD "));
  Serial.print(value);
  Serial.println(F(" INVALID"));
}

void PrintPWMtype(byte value, byte pin) {
  bool ValidChannel = true;
  if (value == PhaseCorrectPWM || value == PhaseCorrectPWM_8bit || value == PhaseCorrectPWM_9bit || value == PhaseCorrectPWM_10bit || value == PhaseCorrectPWM_ICR || value == PhaseCorrectPWM_OCR || value == PhaseCorrectPWM_OCR16bit) {
    Serial.print(F("Phase correct "));
  }
  else if (value == FastPWM || value == FastPWM_8bit || value == FastPWM_9bit || value == FastPWM_10bit || value == FastPWM_ICR || value == FastPWM_OCR || value == FastPWM_OCR16bit) {
    Serial.print(F("Fast "));
  }
  else if (value == PhaseFrequencyCorrectPWM_ICR || value == PhaseFrequencyCorrectPWM_OCR16bit) {
    Serial.print(F("Phase and frequency correct "));
  }
  else {
    ValidChannel = false;
  }
  if (ValidChannel == true) {
    if (value == FastPWM || value == PhaseCorrectPWM || value == PhaseCorrectPWM_8bit || value == FastPWM_8bit) {
      Serial.print(F("8 bit "));
    }
    else if (value == PhaseCorrectPWM_9bit || value == FastPWM_9bit) {
      Serial.print(F("9 bit "));
    }
    else if (value == PhaseCorrectPWM_10bit || value == FastPWM_10bit) {
      Serial.print(F("10 bit "));
    }
    Serial.print(F("PWM"));
    if (value == PhaseCorrectPWM_ICR || value == PhaseCorrectPWM_OCR || value == PhaseCorrectPWM_OCR16bit || value == FastPWM_ICR || value == FastPWM_OCR || value == FastPWM_OCR16bit || value == PhaseFrequencyCorrectPWM_ICR || value == PhaseFrequencyCorrectPWM_OCR16bit) {
      Serial.print(F(" with maximum value set on "));
      if (value == PhaseCorrectPWM_ICR || value == FastPWM_ICR || value == PhaseFrequencyCorrectPWM_ICR) {
        Serial.print(F("ICR"));
      }
      else {
        Serial.print(F("OCR"));
      }
    }
    Serial.println(F(""));
  }
}

void PrintExternalClockType(byte ChannelUsed, bool RisingClockUsed) {
  bool ValidChannel = true;
  byte Tpin = AdvancedAnalogWrite.ReturnTpin(ChannelUsed);
  if (Tpin != 255) {
    Serial.print(F("T"));
    Serial.print(Tpin);
    Serial.print(F(" "));
  }
  else {
    ValidChannel = false;
  }
  if (ValidChannel == true) {
    Serial.print(F("external clock, "));
    if (RisingClockUsed == true) {
      Serial.println(F("rising"));
    }
    else {
      Serial.println(F("falling"));
    }
  }
}

bool IsPWMtypeSupported(byte ChannelUsed, byte PWMtype) {
  bool IsSupported = false;
  byte AvailablePWMmodes[AvailablePWMmodesPerTimer];
  AdvancedAnalogWrite.ReturnAvailablePWMmodes(ChannelUsed, AvailablePWMmodes);
  for (int i = 0; i < AvailablePWMmodesPerTimer; i++) {
    if (AvailablePWMmodes[i] == PWMtype) {
      IsSupported = true;
      break;
    }
  }
  return IsSupported;
}

void setup() {
  Serial.begin(SerialPortRate);
}

void loop() {
  static int ByteCount = 0;
  if (Serial.available() > 0) {
    char inData = Serial.read();
    if (inData != '\n' && ByteCount < commandSize) {
      command[ByteCount] = inData;
      ByteCount++;
    }
    else {
      bool ValidField = true;
      char field[FieldSize];
      ByteCount = 0;
      getField(field, 0);
      if (strcmp(field, "MILLIS_MICROS_START") == 0) {
        getField(field, 1);
        if (strcmp(field, "DELAY_TEST") == 0) {
          Serial.println(F("Start of delay"));
          delay(5000);
          Serial.println(F("If you see this line immediately after the previous, delay() is inoperative"));
        }
        Serial.print(F("millis() is currently "));
        Serial.println(millis());
        for (int i = 0; i < 500; i++) {
          delayMicroseconds(10000);
        }
        Serial.println(F("If you see this line 5 seconds after the previous, delayMicroseconds() is functional"));
        AdvancedAnalogWrite.RestartMillisMicros();
        Serial.println(F("Stopping PWM on Pins 5/6 and restoring millis() and micros()"));
        Serial.print(F("millis() is currently "));
        Serial.println(millis());
        delay(5000);
        Serial.println(F("If you see this line 5 seconds from the previous line, millis()/micros()/delay() has been fully restored"));
      }
      else {
        bool StopPWM = false;
        bool ReadChannel = false;
        byte channel = 0;
        unsigned long BitDepth = 0;
        channel = atoi(field);
        getField(field, 1);
        if (strcmp(field, "QUERY") == 0) {
          if (AdvancedAnalogWrite.ReturnMaximumPWMvalue(channel) != 0) {
            word AvailablePrescalers[AvailablePrescalersPerTimer];
            byte AvailablePrescalers_byte[AvailablePrescalersPerTimer];
            AdvancedAnalogWrite.ReturnAvailablePrescalers(channel, AvailablePrescalers, AvailablePrescalers_byte);
            Serial.print(F("Available prescalers for this pin: "));
            for (int i = 0; i < AvailablePrescalersPerTimer; i++) {
              word temp = AvailablePrescalers[i];
              if (temp != 0) {
                Serial.print(temp);
                Serial.print(F(" "));
              }
              else {
                break;
              }
            }
            Serial.println(F(""));
            Serial.print(F("Maximum PWM for this pin is "));
            Serial.println(AdvancedAnalogWrite.ReturnMaximumPWMvalue(channel));
            Serial.println(F("Available PWM modes:"));
            byte AvailablePWMmodes[AvailablePWMmodesPerTimer];
            AdvancedAnalogWrite.ReturnAvailablePWMmodes(channel, AvailablePWMmodes);
            for (int i = 0; i < AvailablePWMmodesPerTimer; i++) {
              if (AvailablePWMmodes[i] != 0) {
                switch (AvailablePWMmodes[i]) {
                  case PhaseCorrectPWM:
                    Serial.println(F("Phase correct PWM"));
                    break;
                  case FastPWM:
                    Serial.println(F("Fast PWM"));
                    break;
                  case PhaseCorrectPWM_OCR:
                    Serial.println(F("Phase correct PWM with OCR"));
                    break;
                  case FastPWM_OCR:
                    Serial.println(F("Fast PWM with OCR"));
                    break;
                  case PhaseCorrectPWM_8bit:
                    Serial.println(F("8 bit phase correct PWM"));
                    break;
                  case PhaseCorrectPWM_9bit:
                    Serial.println(F("9 bit phase correct PWM"));
                    break;
                  case PhaseCorrectPWM_10bit:
                    Serial.println(F("10 bit phase correct PWM"));
                    break;
                  case FastPWM_8bit:
                    Serial.println(F("8 bit fast PWM"));
                    break;
                  case FastPWM_9bit:
                    Serial.println(F("9 bit fast PWM"));
                    break;
                  case FastPWM_10bit:
                    Serial.println(F("10 bit fast PWM"));
                    break;
                  case PhaseFrequencyCorrectPWM_ICR:
                    Serial.println(F("Phase and frequency correct PWM with ICR"));
                    break;
                  case PhaseFrequencyCorrectPWM_OCR16bit:
                    Serial.println(F("Phase and frequency correct PWM with OCR"));
                    break;
                  case PhaseCorrectPWM_ICR:
                    Serial.println(F("Phase correct PWM with ICR"));
                    break;
                  case FastPWM_ICR:
                    Serial.println(F("Fast PWM with ICR"));
                    break;
                  case PhaseCorrectPWM_OCR16bit:
                    Serial.println(F("Phase correct PWM with OCR"));
                    break;
                  case FastPWM_OCR16bit:
                    Serial.println(F("Fast PWM with OCR"));
                    break;
                }
              }
              else {
                break;
              }
            }
            if (AdvancedAnalogWrite.ExternalClockCapabilityCheck(channel) == true) {
              byte Tpin = AdvancedAnalogWrite.ReturnTpin(channel);
              if (Tpin != 255) {
                Serial.print(F("This pin supports external clocking on T"));
                Serial.println(Tpin);
              }
            }
          }
          else {
            Serial.println(F("PWM is not supported on this pin"));
            ValidField = false;
            PrintFieldError(0);
          }
        }
        else if (AdvancedAnalogWrite.ReturnMaximumPWMvalue(channel) != 0) {
          if (strcmp(field, "VARIABLE_FREQ") == 0) {
            StopPWM = true;
            getField(field, 2);
            unsigned long frequency = atol(field);
            byte PWMtype;
            getField(field, 3);
            if (strcmp(field, "PHASE_CORRECT") == 0) {
              getField(field, 6);
              if (strcmp(field, "ICR") == 0) {
                if (IsPWMtypeSupported(channel, PhaseCorrectPWM_ICR) == true) {
                  PWMtype = PhaseCorrectPWM_ICR;
                }
                else {
                  ValidField = false;
                  PrintFieldError(0);
                }
              }
              else if (strcmp(field, "OCR") == 0) {
                if (IsPWMtypeSupported(channel, PhaseCorrectPWM_OCR16bit) == true) {
                  PWMtype = PhaseCorrectPWM_OCR16bit;
                }
                else if (IsPWMtypeSupported(channel, PhaseCorrectPWM_OCR) == true) {
                  PWMtype = PhaseCorrectPWM_OCR;
                }
                else {
                  ValidField = false;
                  PrintFieldError(0);
                }
              }
            }
            else if (strcmp(field, "FAST") == 0) {
              getField(field, 5);
              if (strcmp(field, "ICR") == 0) {
                if (IsPWMtypeSupported(channel, FastPWM_ICR) == true) {
                  PWMtype = FastPWM_ICR;
                }
                else {
                  ValidField = false;
                  PrintFieldError(0);
                }
              }
              else if (strcmp(field, "OCR") == 0) {
                if (IsPWMtypeSupported(channel, FastPWM_OCR16bit) == true) {
                  PWMtype = FastPWM_OCR16bit;
                }
                else if (IsPWMtypeSupported(channel, FastPWM_OCR) == true) {
                  PWMtype = FastPWM_OCR;
                }
                else {
                  ValidField = false;
                  PrintFieldError(0);
                }
              }
            }
            else if (strcmp(field, "PHASE_FREQ_CORRECT") == 0) {
              getField(field, 5);
              if (strcmp(field, "ICR") == 0) {
                if (IsPWMtypeSupported(channel, PhaseFrequencyCorrectPWM_ICR) == true) {
                  PWMtype = PhaseFrequencyCorrectPWM_ICR;
                }
                else {
                  ValidField = false;
                  PrintFieldError(0);
                }
              }
              else if (strcmp(field, "OCR") == 0) {
                if (IsPWMtypeSupported(channel, PhaseFrequencyCorrectPWM_OCR16bit) == true) {
                  PWMtype = PhaseFrequencyCorrectPWM_OCR16bit;
                }
                else {
                  ValidField = false;
                  PrintFieldError(0);
                }
              }
            }
            else {
              ValidField = false;
              PrintFieldError(3);
            }
            getField(field, 5);
            word PWMvalue = atol(field);
            PrintPWMtype(PWMtype, channel);
            if (ValidField == true) {
              getField(field, 4);
              byte polarity;
              if (strcmp(field, "NORMAL") == 0) {
                polarity = NORMAL;
              }
              else if (strcmp(field, "INVERTED") == 0) {
                polarity = INVERTED;
              }
              else {
                ValidField = false;
                PrintFieldError(3);
              }
              if (ValidField == true) {
                unsigned long ActualFrequency[1];
                byte PrescalerValue[1];
                word MaximumPWMvalue = AdvancedAnalogWrite.initWithFrequency(channel, frequency, PWMtype, polarity, ActualFrequency, PrescalerValue);
                if (MaximumPWMvalue != 0) {
                  Serial.print(F("Maximum PWM value is "));
                  Serial.println(MaximumPWMvalue);
                  Serial.print(F("Actual PWM frequency is "));
                  Serial.print(ActualFrequency[0]);
                  Serial.println(F(" Hz"));
                  if (PWMvalue <= MaximumPWMvalue) {
                    AdvancedAnalogWrite.write(channel, PWMvalue, PWMtype);
                    AdvancedAnalogWrite.start(channel, PrescalerValue[0]);
                  }
                  else {
                    ValidField = false;
                    PrintFieldError(6);
                  }
                }
                else {
                  ValidField = false;
                  PrintFieldError(2);
                }
              }
            }
          }
          else if (strcmp(field, "STOP") == 0) {
            StopPWM = true;
            if (ValidField == true) {
              AdvancedAnalogWrite.stop(channel);
              Serial.print(F("Stopping PWM on Channel "));
              Serial.println(channel);
            }
          }
          else if (strcmp(field, "READ") == 0) {
            ReadChannel = true;
            Serial.print(F("Current PWM value is "));
            Serial.println(AdvancedAnalogWrite.read(channel));
          }
          else if (strcmp(field, "INCREMENT") == 0) {
            ReadChannel = true;
            getField(field, 2);
            unsigned long value = atol(field);
            if (AdvancedAnalogWrite.ReturnMaximumPWMvalue(channel) >= value) {
              AdvancedAnalogWrite.increment(channel, value);
            }
            else {
              ValidField = false;
              PrintFieldError(2);
            }
          }
          else if (strcmp(field, "DECREMENT") == 0) {
            ReadChannel = true;
            getField(field, 2);
            unsigned long value = atol(field);
            if (AdvancedAnalogWrite.ReturnMaximumPWMvalue(channel) >= value) {
              AdvancedAnalogWrite.decrement(channel, value);
            }
            else {
              ValidField = false;
              PrintFieldError(2);
            }
          }
          else {
            BitDepth = atol(field);
            Serial.print(F("Bit depth: "));
            Serial.println(BitDepth);
          }
          if (StopPWM == false && ReadChannel == false && ValidField == true) {
            Serial.print(F("Channel: "));
            Serial.println(channel);
            getField(field, 2);
            byte PWMtype;
            if (strcmp(field, "FAST") == 0) {
              getField(field, 6);
              if (strcmp(field, "ICR") == 0 || strcmp(field, "OCR") == 0) {
                if (BitDepth >= Minimum_OCRA_ICR && BitDepth <= AdvancedAnalogWrite.ReturnMaximumPWMvalue(channel)) {
                  if (strcmp(field, "ICR") == 0) {
                    PWMtype = FastPWM_ICR;
                  }
                  else {
                    PWMtype = FastPWM_OCR16bit;
                    if (IsPWMtypeSupported(channel, PWMtype) == false) {
                      PWMtype = FastPWM_OCR;
                    }
                  }
                  PrintPWMtype(PWMtype, channel);
                }
                else {
                  ValidField = false;
                  PrintFieldError(1);
                }
              }
              else {
                if (BitDepth == 8) {
                  PWMtype = FastPWM_8bit;
                  if (IsPWMtypeSupported(channel, PWMtype) == false) {
                    PWMtype = FastPWM;
                  }
                }
                else if (BitDepth == 9) {
                  PWMtype = FastPWM_9bit;
                }
                else if (BitDepth == 10) {
                  PWMtype = FastPWM_10bit;
                }
                else {
                  ValidField = false;
                  PrintFieldError(1);
                }
              }
              PrintPWMtype(PWMtype, channel);
              if (IsPWMtypeSupported(channel, PWMtype) == false) {
                ValidField = false;
                PrintFieldError(2);
              }
            }
            else if (strcmp(field, "PHASE_CORRECT") == 0) {
              getField(field, 6);
              if (strcmp(field, "ICR") == 0 || strcmp(field, "OCR") == 0) {
                getField(field, 6);
                if (BitDepth >= Minimum_OCRA_ICR && BitDepth <= AdvancedAnalogWrite.ReturnMaximumPWMvalue(channel)) {
                  if (strcmp(field, "ICR") == 0) {
                    PWMtype = PhaseCorrectPWM_ICR;
                  }
                  else {
                    PWMtype = PhaseCorrectPWM_OCR16bit;
                    if (IsPWMtypeSupported(channel, PWMtype) == false) {
                      PWMtype = PhaseCorrectPWM_OCR;
                    }
                  }
                  PrintPWMtype(PWMtype, channel);
                }
                else {
                  ValidField = false;
                  PrintFieldError(1);
                }
              }
              else {
                if (BitDepth == 8) {
                  PWMtype = PhaseCorrectPWM_8bit;
                  if (IsPWMtypeSupported(channel, PWMtype) == false) {
                    PWMtype = PhaseCorrectPWM;
                  }
                }
                else if (BitDepth == 9) {
                  PWMtype = PhaseCorrectPWM_9bit;
                }
                else if (BitDepth == 10) {
                  PWMtype = PhaseCorrectPWM_10bit;
                }
                else {
                  ValidField = false;
                  PrintFieldError(1);
                }
              }
              PrintPWMtype(PWMtype, channel);
              if (IsPWMtypeSupported(channel, PWMtype) == false) {
                ValidField = false;
                PrintFieldError(2);
              }
            }
            else if (strcmp(field, "PHASE_FREQ_CORRECT") == 0) {
              getField(field, 6);
              if (strcmp(field, "ICR") == 0 || strcmp(field, "OCR") == 0) {
                if (BitDepth >= Minimum_OCRA_ICR && BitDepth <= AdvancedAnalogWrite.ReturnMaximumPWMvalue(channel)) {
                  if (strcmp(field, "ICR") == 0) {
                    PWMtype = PhaseFrequencyCorrectPWM_ICR;
                  }
                  else {
                    PWMtype = PhaseFrequencyCorrectPWM_OCR16bit;
                  }
                  PrintPWMtype(PWMtype, channel);
                }
                else {
                  ValidField = false;
                  PrintFieldError(1);
                }
              }
              PrintPWMtype(PWMtype, channel);
              if (IsPWMtypeSupported(channel, PWMtype) == false) {
                ValidField = false;
                PrintFieldError(2);
              }
            }
            else {
              ValidField = false;
              PrintFieldError(2);
            }
            getField(field, 3);
            byte PWMpolarity;
            if (strcmp(field, "NORMAL") == 0) {
              PWMpolarity = NORMAL;
              Serial.print(F("Normal "));
            }
            else if (strcmp(field, "INVERTED") == 0) {
              PWMpolarity = INVERTED;
              Serial.print(F("Inverted "));
            }
            else {
              ValidField = false;
              PrintFieldError(3);
            }
            if (ValidField == true) {
              Serial.println(F("polarity"));
            }
            getField(field, 4);
            bool ExternalCLK = false;
            bool RisingCLK = false;
            word prescaler_int;
            byte prescaler;
            if (AdvancedAnalogWrite.ExternalClockCapabilityCheck(channel) == true) { // pins which can be clocked via the T pin
              if (strcmp(field, "T_EXT_RISING") == 0) {
                ExternalCLK = true;
                RisingCLK = true;
              }
              else if (strcmp(field, "T_EXT_FALLING") == 0) {
                ExternalCLK = true;
              }
              if (ExternalCLK == true) {
                PrintExternalClockType(channel, RisingCLK);
              }
            }
            if (ExternalCLK == false) {
              prescaler_int = atoi(field);
              word AvailablePrescalers[AvailablePrescalersPerTimer];
              byte AvailablePrescalers_byte[AvailablePrescalersPerTimer];
              AdvancedAnalogWrite.ReturnAvailablePrescalers(channel, AvailablePrescalers, AvailablePrescalers_byte);
              bool OldValidField = ValidField;
              ValidField = false;
              for (int i = 0; i < AvailablePrescalersPerTimer; i++) {
                if (prescaler_int == AvailablePrescalers[i]) {
                  prescaler = AvailablePrescalers_byte[i];
                  if (OldValidField == true) {
                    ValidField = true;
                  }
                  break;
                }
              }
              if (ValidField == true) {
                Serial.print(F("Prescaler: "));
                Serial.println(prescaler_int);
              }
              else {
                PrintFieldError(4);
              }
            }
            getField(field, 5);
            unsigned long PWMvalue_long = atol(field);
            word PWMvalue = PWMvalue_long;
            Serial.print(F("PWM value: "));
            Serial.println(PWMvalue_long);
            if (((PWMtype == PhaseFrequencyCorrectPWM_ICR || PWMtype == PhaseCorrectPWM_ICR || PWMtype == FastPWM_ICR || PWMtype == FastPWM_OCR || PWMtype == PhaseCorrectPWM_OCR || PWMtype == PhaseFrequencyCorrectPWM_OCR16bit || PWMtype == PhaseCorrectPWM_OCR16bit || PWMtype == FastPWM_OCR16bit) && PWMvalue_long > BitDepth) ||
                ((PWMtype == FastPWM_8bit || PWMtype == PhaseCorrectPWM_8bit || PWMtype == FastPWM || PWMtype == PhaseCorrectPWM) && PWMvalue_long > 255) ||
                ((PWMtype == FastPWM_9bit || PWMtype == PhaseCorrectPWM_9bit) && PWMvalue_long > 511) ||
                ((PWMtype == FastPWM_10bit || PWMtype == PhaseCorrectPWM_10bit) && PWMvalue_long > 1023))
            {
              ValidField = false;
              PrintFieldError(5);
            }
            if (ValidField == true) {
              if (ExternalCLK == true) {
                if (RisingCLK == true) {
                  prescaler = T_EXT_RISING;
                }
                else {
                  prescaler = T_EXT_FALLING;
                }
              }
              AdvancedAnalogWrite.init(channel, BitDepth, PWMtype, PWMpolarity);
              AdvancedAnalogWrite.write(channel, PWMvalue, PWMtype);
              AdvancedAnalogWrite.start(channel, prescaler);
            }
          }
        }
        else {
          ValidField = false;
          PrintFieldError(0);
        }
      }
      FlushSerialBuffer();
      if (ValidField == true) {
        Serial.println(F("OK"));
      }
      else {
        Serial.println(F("ERROR"));
      }
    }
  }
}