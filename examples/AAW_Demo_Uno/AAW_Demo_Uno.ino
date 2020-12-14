/*

   AdvancedAnalogWrite demo for Arduno Uno and compatibles by Bryce Cherry

   Command format:
   MILLIS_MICROS_START (DELAY_TEST): Stop PWM on Pins 5 and 6 and restart millis()/micros()/delay() - DELAY_TEST will test delay
   (3/5/6/9/10/11) STOP: Stop PWM on a pin
   (3/5/6/9/10/11) READ: Read current PWM value from a pin with a given bit resolution
   (3/5/6/9/10/11) (8/9/10) (PHASE_CORRECT/FAST) (NORMAL/INVERTED) (T0_EXT_RISING/T0_EXT_FALLING/T1_EXT_RISING/T1_EXT_FALLING/1/8/32/64/128/256/1024) (PWM value 0-(255/511/1023) for 8/9/10 bit):
   Pin number, bit resolution (8/9/10 is supported only on Pins 9/10 - all others are 8), polarity, prescaler (T0_EXT_RISING/T0_EXT_FALLING is supported only on Pins 6/5, T1_EXT_RISING/T1_EXT_FALLING is supported only on Pins 9/10, 32 and 128 is supported only on on Pins 3/11)

*/

#include <AdvancedAnalogWrite.h>

// ensures that the serial port is flushed fully on request
const unsigned long SerialPortRate = 9600;
const byte SerialPortRateTolerance = 5; // percent - increase to 50 for rates above 115200 up to 4000000
const byte SerialPortBits = 10; // start (1), data (8), stop (1)
const unsigned long TimePerByte = ((((1000000ULL * SerialPortBits) / SerialPortRate) * (100 + SerialPortRateTolerance)) / 100); // calculated on serial port rate + tolerance and rounded down to the nearest uS, long caters for even the slowest serial port of 75 bps

const byte commandSize = 50;
char command[commandSize];
const byte FieldSize = 25;

void getField (char* buffer, int index) {
  int CommandPos = 0;
  int FieldPos = 0;
  int SpaceCount = 0;
  while (CommandPos < commandSize) {
    if (command[CommandPos] == 0x20 || command[CommandPos] == 0x0D || command[CommandPos] == 0x0A) {
      SpaceCount++;
      CommandPos++;
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

void PrintOK() {
  Serial.println(F("OK"));
}

void PrintError() {
  Serial.println(F("ERROR"));
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
        Serial.println(F("If you see this line five seconds after the previous, delayMicroseconds() is functional"));
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
        byte BitDepth = 0;
        channel = atoi(field);
        if (channel == 3 || channel == 5 || channel == 6 || channel == 9 || channel == 10 || channel == 11) {
        }
        else {
          ValidField = false;
          Serial.println(F("FIELD 0 ERROR"));
        }
        getField(field, 1);
        if (strcmp(field, "STOP") == 0) {
          StopPWM = true;
          if (ValidField == true) {
            AdvancedAnalogWrite.stop(channel);
            Serial.print(F("Stopping PWM on Channel "));
            Serial.println(channel);
          }
        }
        else if (strcmp(field, "READ") == 0) {
          ReadChannel = true;
          Serial.println(AdvancedAnalogWrite.read(channel));
        }
        else {
          BitDepth = atoi(field);
          Serial.print(F("Bit depth: "));
          Serial.println(BitDepth);
        }
        if (StopPWM == false && ReadChannel == false && ValidField == true) {
          Serial.print(F("Channel: "));
          Serial.println(channel);
          getField(field, 2);
          byte PWMtype;
          if (strcmp(field, "PHASE_CORRECT") == 0) {
            if (channel == 9 || channel == 10) { // 8 or 16 bit channel
              if (BitDepth == 8) {
                PWMtype = PhaseCorrectPWM_8bit;
                Serial.println(F("Phase correct 8 bit PWM"));
              }
              else if (BitDepth == 9) {
                PWMtype = PhaseCorrectPWM_9bit;
                Serial.println(F("Phase correct 9 bit PWM"));
              }
              else if (BitDepth == 10) {
                PWMtype = PhaseCorrectPWM_10bit;
                Serial.println(F("Phase correct 10 bit PWM"));
              }
              else {
                ValidField = false;
                Serial.println(F("FIELD 1 ERROR"));
              }
            }
            else { // 8 bit only channel
              PWMtype = PhaseCorrectPWM;
              Serial.println(F("Phase correct PWM"));
            }
          }
          else if (strcmp(field, "FAST") == 0) {
            if (channel == 9 || channel == 10) { // 8 or 16 bit channel
              if (BitDepth == 8) {
                PWMtype = FastPWM_8bit;
                Serial.println(F("Fast 8 bit PWM"));
              }
              else if (BitDepth == 9) {
                PWMtype = FastPWM_9bit;
                Serial.println(F("Fast 9 bit PWM"));
              }
              else if (BitDepth == 10) {
                PWMtype = FastPWM_10bit;
                Serial.println(F("Fast 10 bit PWM"));
              }
              else {
                ValidField = false;
                Serial.println(F("FIELD 1 ERROR"));
              }
            }
            else { // 8 bit only channel
              PWMtype = FastPWM;
              Serial.println(F("Fast PWM"));
            }
          }
          else {
            ValidField = false;
            Serial.println(F("FIELD 2 ERROR"));
          }
          getField(field, 3);
          byte PWMpolarity;
          if (strcmp(field, "NORMAL") == 0) {
            PWMpolarity = NORMAL;
            Serial.println(F("Normal polarity"));
          }
          else if (strcmp(field, "INVERTED") == 0) {
            PWMpolarity = INVERTED;
            Serial.println(F("Inverted polarity"));
          }
          else {
            ValidField = false;
            Serial.println(F("FIELD 3 ERROR"));
          }
          getField(field, 4);
          bool ExternalCLK = false;
          bool RisingCLK = false;
          word prescaler_int;
          byte prescaler;
          if (channel == 5 || channel == 6) { // pins which can be clocked via the T0 pin
            if (strcmp(field, "T0_EXT_RISING") == 0) {
              ExternalCLK = true;
              RisingCLK = true;
              Serial.println(F("T0 external clock, rising"));
            }
            else if (strcmp(field, "T0_EXT_FALLING") == 0) {
              ExternalCLK = true;
              Serial.println(F("T0 external clock, falling"));
            }
          }
          else if (channel == 9 || channel == 10) { // pins which can be clocked via the T0 pin
            if (strcmp(field, "T1_EXT_RISING") == 0) {
              ExternalCLK = true;
              RisingCLK = true;
              Serial.println(F("T1 external clock, rising"));
            }
            else if (strcmp(field, "T1_EXT_FALLING") == 0) {
              ExternalCLK = true;
              Serial.println(F("T1 external clock, falling"));
            }
          }
          if (ExternalCLK == false) {
            prescaler_int = atoi(field);
            if (prescaler_int == 1) {
              prescaler = PS_NONE;
              Serial.println(F("Prescaler: 1"));
            }
            else if (prescaler_int == 8) {
              prescaler = PS_8;
              Serial.println(F("Prescaler: 8"));
            }
            else if (prescaler_int == 32 && (channel == 3 || channel == 11)) { // pins which support this prescaler ratio
              prescaler = PS_32;
              Serial.println(F("Prescaler: 32"));
            }
            else if (prescaler_int == 64) {
              prescaler = PS_64;
              Serial.println(F("Prescaler: 64"));
            }
            else if (prescaler_int == 128 && (channel == 3 || channel == 11)) { // pins which support this prescaler ratio
              prescaler = PS_128;
              Serial.println(F("Prescaler: 128"));
            }
            else if (prescaler_int == 256) {
              prescaler = PS_256;
              Serial.println(F("Prescaler: 256"));
            }
            else if (prescaler_int == 1024) {
              prescaler = PS_1024;
              Serial.println(F("Prescaler: 1024"));
            }
            else {
              ValidField = false;
              Serial.println(F("FIELD 4 ERROR"));
            }
          }
          getField(field, 5);
          unsigned long PWMvalue_long = atol(field);
          word PWMvalue = PWMvalue_long;
          Serial.print(F("PWM value: "));
          Serial.println(PWMvalue_long);
          if ((BitDepth == 8 && PWMvalue_long > 255) || (BitDepth == 9 && PWMvalue_long > 511) || (BitDepth == 10 && PWMvalue_long > 1023)) {
            ValidField = false;
            Serial.println(F("FIELD 5 ERROR"));
          }
          if (ValidField == true) {
            if (ExternalCLK == true && (channel == 5 || channel == 6 || channel == 9 || channel == 10)) { // pins which support 16 bit PWM and can be externally clocked via the T0 (5/6) or T1 (9/10) pins
              if (RisingCLK == true) {
                if (channel == 5 || channel == 6) {
                  prescaler = T0_EXT_RISING;
                }
                else { // channels 9/10
                  prescaler = T1_EXT_RISING;
                }
              }
              else {
                if (channel == 5 || channel == 6) {
                  prescaler = T0_EXT_FALLING;
                }
                else { // channels 9/10
                  prescaler = T1_EXT_FALLING;
                }
              }
            }
            AdvancedAnalogWrite.init(channel, BitDepth, PWMtype, PWMpolarity);
            AdvancedAnalogWrite.write(channel, BitDepth, PWMvalue);
            AdvancedAnalogWrite.start(channel, prescaler);
          }
        }
      }
      if (ValidField == true) {
        PrintOK();
      }
      else {
        PrintError();
      }
      FlushSerialBuffer();
    }
  }
}