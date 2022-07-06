#AdvancedAnalogWrite Library#

AdvancedAnalogWrite is a library for enabling PWM features on certain pins beyond what the standard Arduino IDE offers.

Revisions

1.0.0	First release

1.0.1	Eliminated requirement for backup registers affecting millis()/micros()/delay() functions

1.1.0	Added variable resolution modes for 16 bit counters and OCR TOP for 8 bit counters and Phase Correct PWM on 16 bit counters; requirement for BitDepth field for write() is eliminated and PWM mode specifier for write() can be 0 for speed to skip checks on PWM value for speed

1.2.0	Added initialization with a PWM frequency option

1.3.0	Added query functions and demo sketch is now universal

1.3.1 Added counter reset function

FEATURES:

Supported boards (and derivatives): Uno (Mega/Leonardo/ATmega8 will be included in a later release)

Fast PWM (maximum 1/256 CPU frequency vs 1/32768 for most pins vs 1/16384 for certain pins) and Phase Correct PWM

On certain sets of pins, PWM resolution (can be faster than 1/256 CPU frequency)

Programmable PWM frequency prescaler with external T input for external clocking of supported pins

Ability to stop and resume micros()/millis()/delay() to avoid conflict with these functions

Readback of current PWM value

Reset of counter which can be useful for controlling a dual modulus prescaler

USAGE:
init(pin, BitDepth, mode, polarity): Initialize PWM on a certain pin and if set for external clocking, set the external clock input pin as an input and enable its pullup

initWithFrequency(pin, freq, mode, polarity, *ActualFrequency, *PrescalerValue): Initialize PWM on a set of pins (except OCxA for OCR option) with a specified PWM frequency - returns maximum PWM value other than 0 if within range - mode must contain OCR or ICR on a supported pin, ActualFrequency is a uint32_t and PrescalerValue is a uint8_t

BitDepth: Must be compatible with the value of pin

polarity: NORMAL/INVERTED - independent for each pin

mode: Common to a given set of pins

mode:

FastPWM(OCR) and PhaseCorrectPWM(OCR) (both 8 bit) is only supported on 8 bit counters

FastPWM_8bit/FastPWM_9bit/FastPWM_10bit/PhaseCorrectPWM_8bit/PhaseCorrectPWM_9bit/PhaseCorrectPWM_10bit/PhaseFrequencyCorrectPWM_ICR/PhaseCorrectPWM_ICR/FastPWM_ICR is only supported on 16 bit counters

When an OCR option is used, PWM cannot be used on an OCxA pin for a given set

If ICR or OCR16bit options are used, BitDepth becomes MaximumPWMvalue (3-65535)


start(pin, prescaler): Start PWM on a certain pin - prescaler is common to a given set of pins

prescaler:

T_EXT_FALLING/T_EXT_RISING is also supported only on OCx pins with a correspond Tx hardware function pin


stop(pin): Stop PWM on a certain pin

write(pin, value, mode): Writes a value to a certain pin - a short pulse may still be present with a PWM value of 0; if your 
application requires the elimination of this pulse under this condition, initialize with reverse polarity and use the inverse of the required PWM value - specifying a PWM mode performs checks if values are within limits (0 to ignore checks)

increment(pin, value): Increments PWM by a given value on a given pin (will not overflow)

decrement(pin, value): Decrements PWM by a given value on a given pin (will not underflow)

RestartMillisMicros(): Disables PWM on pins used by the timer (usually Timer/Counter 0 and its corresponding OCxx pins) and restarts millis()/micros()/delay()

read(pin): Reads the PWM value of a certain pin

RestartMillisMicros(): Disables clock output on pins used by the timer (usually Timer/Counter 0 and its corresponding OCxx pins) and restarts millis()/micros()/delay()

ReturnAvailablePrescalers(pin, *AvaiablePrescalers, *AvailablePrescalers_byte)): Returns available internal prescalers to *AvailablePrescalers (uint16_t) and *AvailablePrescalers_byte based on a given pin; a 0 indicates the end of the number of prescalers and the array passed to *AvailablePrescalers is AvailablePrescalersPerTimer - the corresponding AvailablePrescalers_byte array value is used for start()

ReturnMaximumPWMvalue(pin): Returns a uint32_t maximum PWM value on a given pin (result will be 0 if unsupported)

ExternalClockCapabilityCheck(pin): Returns a bool if this pin can be externally clocked by a Tx pin input

ReturnTpin(pin): Returns the T input hardware function e.g. on an ATmega328P, 5/6 (OC0x) will return 0 for T0 and 9/10 (OC1x) will return 1 for T1; otherwise, it will return 255 if a pin does not support external clocking by a T pin

ReturnAvailablePWMmodes(pin, *AvailablePWMmodes): Returns *AvailablePWMmodes (uint8_t) based on a given pin; a 0 indicates the end of the number of PWM modes and the array passed to *AvailablePWMmodes is AvailablePWMmodesPerTimer

ResetCounter(pin): Reset a counter

Minimum_OCRA_ICR is used for variable frequency based on OCRA and ICR values.

PIN DETAILS WITH PWM RESOLUTIONS (T pin is not used by millis/micros/delay - first pin is OCxA):

Uno (and derivatives): (9/10/T1) (variable resolution); 6/5/T0 (used by millis/micros/delay - 8 bit), 11/3 (8 bit)

Mega (and derivatives): (5/2/3/T3)/(6/7/8/T4)/(11/12/TOSC1)/(46/45/44/T5: 47) (variable resuolution), (10/9/T1)/(13/4/T0: 38 - used by millis/micros/delay) (8 bit) - T1/T2/T3/T4/TOSC1 have no traces connected to its headers on the Mega board (official and most of the compatibles)

Leonardo (and derivatives): (11/3/T0: 6 - used by millis/micros/delay) (8 bit), (13/10/T1: 12) (10 bit), 5/(9/10) (variable resolution)