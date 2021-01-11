#AdvancedAnalogWrite Library#

AdvancedAnalogWrite is a library for enabling PWM features on certain pins beyond what the standard Arduino IDE offers.

Revisions
v1.0	First release
v1.01	Eliminated requirement for backup registers affecting millis()/micros()/delay() functions

FEATURES:
Supported boards (and derivatives): Uno (Mega/Leonardo/ATmega8 will be included in a later release)
Fast PWM (maximum 1/256 CPU frequency vs 1/32768 for most pins vs 1/16384 for certain pins) and Phase Correct PWM
On certain pins, PWM resolution
Programmable PWM frequency prescaler
Ability to stop and resume micros()/millis()/delay() to avoid conflict with these functions
Readback of current PWM value

USAGE:
init(pin, BitDepth, mode, polarity): Initialize PWM on a certain pin and if set for external clocking, set the external clock input pin as an input and enable its pullup
BitDepth: Must be compatible with the value of pin
polarity: NORMAL/INVERTED - independent for each pin
mode: Common to a given set of pins
mode (Uno):
FastPWM and PhaseCorrectPWM (both 8 bit) is only supported on Pins 6/5/11/3
FastPWM_8bit/FastPWM_9bit/FastPWM_10bit/PhaseCorrectPWM_8bit/PhaseCorrectPWM_9bit/PhaseCorrectPWM_10bit is only supported on Pins 9/10

start(pin, prescaler): Start PWM on a certain pin - prescaler is common to a given set of pins
prescaler (Uno):
PS_NONE/PS_8/PS_64/PS_256/PS_1024 is supported on all pins
PS_32 and PS_128 is also supported only on Pins 11/3
T0_EXT_FALLING/T0_EXT_RISING is also supported only on Pins 6/5 with an external clock applied to Pin 4 
T1_EXT_FALLING/T1_EXT_RISING is also supported only on Pins 9/10 with an external clock applied to Pin 5

stop(pin): Stop PWM on a certain pin
write(pin, BitDepth, value): Writes a value to a certain pin - a short pulse may still be present with a PWM value of 0; if your application requires the elimination of this pulse under this condition, initialize with reverse polarity and use the inverse of the required PWM value
RestartMillisMicros(): Disables PWM on pins used by the timer (usually Timer/Counter 0 and its corresponding OCxx pins) and restarts millis()/micros()/delay()
read(pin): Reads the PWM value of a certain pin

If you do not intend to use the RestartMillisMicros() function, use #define NO_MILLIS_MICROS_RESTORE before the #include <AdvancedAnalogWrite.h> line to save RAM.

PIN DETAILS WITH PWM RESOLUTIONS:
Uno (and derivatives): (9/10) (8-10 bit); 6/5 (used by millis/micros/delay - 8 bit), 11/3 (8 bit)
Mega (and derivatives): (5/2/3)/(6/7/8)/(11/12)/(46/45/44) (8-10 bit), (10/9)/(13/4 - used by millis/micros/delay) (8 bit)
Leonardo (and derivatives): 11/3 (used by millis/micros/delay - 8 bit), (13/10) (10 bit), 5/(9/10) (8-10 bit)