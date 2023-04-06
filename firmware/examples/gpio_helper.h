
#ifndef GPIO_HELPER_H
#define GPIO_HELPER_H

#define START_TIMER0() TR0 = 1
#define START_TIMER1() TR1 = 1
#define START_TIMER2() TR2 = 1

#define STOP_TIMER0() TR0 = 0
#define STOP_TIMER1() TR1 = 0
#define STOP_TIMER2() TR2 = 0

#define ENABLE_INTRUPTS() EA = 1
#define DISABLE_INTRUPTS() EA = 0

#define LOW_BYTE(val) (val & 0x00FF)
#define HIGH_BYTE(val) (val >> 8)

// set value to 16 bit timer 2 counter
#define SET_TIMER2_COUNT(val) \
    RCAP2L = (val & 0x00FF);  \
    RCAP2H = (val >> 8)

// set value to 16 bit timer 0 counter
#define SET_TIMER0_COUNT(val) \
    TL0 = (val & 0x00FF);     \
    TH0 = (val >> 8)

typedef enum
{
    INPUT = 0,
    OUTPUT = 1
} PortMode;

typedef enum
{
    LOW = 0,
    HIGH = 1
} OutputLevel;

void pinMode(BYTE pin, PortMode mode)
{
    if (mode)
    {
        OEA |= (mode << pin);
    }
    else
    {
        OEA &= ~(mode << pin);
    }
}

void digitalWrite(BYTE pin, OutputLevel level)
{
    // Make sure n is a valid bit index (0 to 15)
    // Create a mask with a 1 in the nth bit position
    BYTE mask = 1 << pin;
    if (level)
    {
        // Set the nth bit to 1
        IOA |= mask;
    }
    else
    {
        // Set the nth bit to 0
        IOA &= ~mask;
    }
}

#endif