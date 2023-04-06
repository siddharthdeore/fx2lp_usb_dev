#define ALLOCATE_EXTERN
#include <fx2regs.h>
#include <fx2types.h>
#include <delay.h>
#include <fx2macros.h>

#include "gpio_helper.h"

void main(void)
{
    SETCPUFREQ(CLK_48M);

    delay(1);

    pinMode(0, OUTPUT);
    for (;;)
    {
        digitalWrite(0, LOW);
        delay(1000);
        digitalWrite(0, HIGH);
        delay(1000);
    }
}
