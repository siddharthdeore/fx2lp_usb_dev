#define ALLOCATE_EXTERN

#include <fx2regs.h>
#include <fx2macros.h>
#include <delay.h>
#include <fx2ints.h>
#include "gpio_helper.h"

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
    if (++t0_counter == 10)
    {
        toggle_led();
        t0_counter = 0;
    }
    STOP_TIMER0();
    SET_TIMER0_COUNT(TIMER0_COUNT);
    START_TIMER0();
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
//! Initialize cpu clock, fifo and bulk-in end point 6
static void initialize(void)
{
    /* set the CPU clock to 48MHz*/
    SETCPUFREQ(CLK_48M);

    /* set the slave FIFO interface to 48MHz*/ /* set the slave FIFO interface to 48MHz IFCONFIG |= 0x40;*/
    SETIF48MHZ();

    // Set DYN_OUT and ENH_PKT bits, as recommended by the TRM.
    REVCTL = bmNOAUTOARM | bmSKIPCOMMIT; // REVCTL = 0x03;
    SYNCDELAY4;

    /*
        EP2CFG = 0xa2; // EP2 BULK-OUT
        EP4CFG = 0xa0; // EP4 BULK-OUT
        EP6CFG = 0xe2; // EP6 BULK-IN
        EP8CFG = 0xe0; // EP8 BULK-IN
    */

    /* out endpoints do not come up armed */
    /* set NAKALL bit to NAK all transfers from host */
    EP6CFG = 0xe2;
    SYNCDELAY4; // 1110 0010 (bulk IN, 512 bytes, double-buffered)
    FIFORESET = 0x80;
    SYNCDELAY4; // NAK all requests from host.
    FIFORESET = 0x82;
    SYNCDELAY4; // Reset individual EP (2,4,6,8)
    FIFORESET = 0x84;
    SYNCDELAY4;
    FIFORESET = 0x86;
    SYNCDELAY4;
    FIFORESET = 0x88;
    SYNCDELAY4;
    FIFORESET = 0x00;
    SYNCDELAY4; // Resume normal operation.
}

void main(void)
{

    // 100Hz
    timer0_init();

    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);

    initialize();

    // set PORT-B to input
    OEB = 0x0;
    SYNCDELAY4;
    // set PORT-D to input
    OED = 0x0;
    SYNCDELAY4;
    /* set AUTOIN commit length to 512 bytes */
    // EP6AUTOINLENH = 0x02;
    // SYNCDELAY4;
    // EP6AUTOINLENL = 0x00;
    // SYNCDELAY4;
    for (;;)
    {
        // Wait for the EP6 buffer to become non-full.
        if (!(EP2468STAT & bmEP6FULL))
        {
            // alternatively fill buffer with port b and d
            for (int i = 0; i < 512; i += 2)
            {
                EP6FIFOBUF[i] = IOB;
                EP6FIFOBUF[i + 1] = IOD;
            }

            // Arm the endpoint. Be sure to set BCH before BCL because BCL access
            // actually arms the endpoint.
            // SYNCDELAY4;
            EP6BCH = 0x02; // commit 512 bytes
            EP6BCL = 0x00;
        }
    }
}