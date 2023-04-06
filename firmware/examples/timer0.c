#define ALLOCATE_EXTERN
#include <fx2regs.h>
#include <fx2types.h>
#include <delay.h>
#include <fx2macros.h>
#include <fx2ints.h>
#include "gpio_helper.h"

/*
    TIMER0_COUNT = (65536 - (48000000 / (12 * FREQ)) = 25536
    eg:
        TIMER0_COUNT = 61536 for 1000Hz
        TIMER0_COUNT = 25536 for 100Hz
 */
#define TIMER0_COUNT 25536

volatile __bit led_flag;
volatile WORD t0_counter = 0;

void toggle_led(void)
{
    led_flag = !led_flag;
    if (led_flag)
    {
        digitalWrite(0, HIGH);
        digitalWrite(1, LOW);
    }
    else
    {
        digitalWrite(0, LOW);
        digitalWrite(1, HIGH);
    }
}

void timer0_isr() __interrupt TF0_ISR
{
    // toogle led after every 10 iterations
    if (t0_counter++ == 10)
    {
        toggle_led();
        t0_counter = 0;
    }
    STOP_TIMER0();
    SET_TIMER0_COUNT(TIMER0_COUNT);
    START_TIMER0();
}
void timer2_isr() __interrupt TF2_ISR
{
    // timer 2 does not clears automatically
    CLEAR_TIMER2();
}
void timer0_init(void)
{
    DISABLE_INTRUPTS();
    STOP_TIMER0();
    CKCON = 0x03;  // Timer 0 using CLKOUT/12
    TMOD &= ~0x0F; // clear Timer 0 mode bits
    TMOD |= 0x01;  // setup Timer 0 as a 16-bit timer
    SET_TIMER0_COUNT(TIMER0_COUNT);
    PT0 = 0; // sets the Timer 0 interrupt to low(0) priority high priority 1
    ENABLE_TIMER0();
    START_TIMER0();
    ENABLE_INTRUPTS();
}
void main(void)
{

    SETCPUFREQ(CLK_48M);

    // 100Hz
    timer0_init();

    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);

    for (;;)
    {
    }
}
