#include <fx2regs.h>   // FX2LP Register Definitions
#include <fx2macros.h> // FX2LP Macro Definitions
#include <fx2ints.h>
#include <gpif.h>
#include <autovector.h>
void main(void)
{


	/* Set DYN_OUT and ENH_PKT bits, as recommended by the TRM. */
	REVCTL = bmNOAUTOARM | bmSKIPCOMMIT;

	got_sud = FALSE;
	vendor_command = 0;

	/* Renumerate. */
	RENUMERATE_UNCOND();

	SETCPUFREQ(CLK_48M);

	USE_USB_INTS();

	/* TODO: Does the order of the following lines matter? */
	ENABLE_SUDAV();
	ENABLE_HISPEED();
	ENABLE_USBRESET();
    /*
     * Setup the FX2 in GPIF master mode, using the internal clock
     * (non-inverted) at 48MHz, and using async sampling.
     */
    IFCONFIG = 0xee;

    /* Abort currently executing GPIF waveform (if any). */
    GPIFABORT = 0xff;

    /* Setup the GPIF registers. */
    /* TODO. Value probably irrelevant, as we don't use RDY* signals? */
    GPIFREADYCFG = 0;

    /* Set TRICTL = 0, thus CTL0-CTL5 are CMOS outputs. */
    GPIFCTLCFG = 0;

    /* When GPIF is idle, tri-state the data bus. */
    /* Bit 7: DONE, bit 0: IDLEDRV. TODO: Set/clear DONE bit? */
    GPIFIDLECS = (0 << 0);

    /* When GPIF is idle, set CTL0-CTL5 to 0. */
    GPIFIDLECTL = 0;

    /*
     * Map index 0 in WAVEDATA to FIFORD. The rest is assigned too,
     * but not used by us.
     *
     * GPIFWFSELECT: [7:6] = SINGLEWR index, [5:4] = SINGLERD index,
     *               [3:2] = FIFOWR index, [1:0] = FIFORD index
     */
    GPIFWFSELECT = (0x3u << 6) | (0x2u << 4) | (0x1u << 2) | (0x0u << 0);

    /* Contains RDY* pin values. Read-only according to TRM. */
    GPIFREADYSTAT = 0;

    /* Make GPIF stop on transaction count not flag. */
    EP2GPIFPFSTOP = (0 << 0);

    /* Reset GPIF WAVEDATA. */
    AUTOPTRSETUP = 0x03;
    // source
    AUTOPTRH1 = 0xe4;
    AUTOPTRL1 = 0x00;
    for (int i = 0; i < 128; i++)
        EXTAUTODAT1 = 0;

    /* Initialize GPIF address pins, output initial values. */
    /*
     * Configure the 9 GPIF address pins (GPIFADR[8:0], which consist of
     * PORTC[7:0] and PORTE[7]), and output an initial address (zero).
     * TODO: Probably irrelevant, the 56pin FX2 has no ports C and E.
     */
    PORTCCFG = 0xff;  /* Set PORTC[7:0] as alt. func. (GPIFADR[7:0]). */
    OEC = 0xff;       /* Configure PORTC[7:0] as outputs. */
    PORTECFG |= 0x80; /* Set PORTE[7] as alt. func. (GPIFADR[8]). */
    OEE |= 0x80;      /* Configure PORTE[7] as output. */
    SYNCDELAY();
    GPIFADRL = 0x00; /* Clear GPIFADR[7:0]. */
    SYNCDELAY();
    GPIFADRH = 0x00; /* Clear GPIFADR[8]. */

    /* Initialize flowstate registers (not used by us). */
    /* Clear all flowstate registers, we don't use this functionality. */
    FLOWSTATE = 0;
    FLOWLOGIC = 0;
    FLOWEQ0CTL = 0;
    FLOWEQ1CTL = 0;
    FLOWHOLDOFF = 0;
    FLOWSTB = 0;
    FLOWSTBEDGE = 0;
    FLOWSTBHPERIOD = 0;

    // Main loop
    while (1)
    {
        // Check for Endpoint 6 OUT (Bulk) data available
        if (EP6FIFOFLGS & EP_EMPTY)
        {
            // Perform GPIF transfer

            // Wait for GPIF transfer to complete
            while (!GPIF_DONE)
                ;

            // Clear GPIF done flag
            GPIF_DONE = 0;

            // Check for GPIF error
            if (GPIF_ERROR)
            {
                // Handle GPIF error
                // ... Add error handling code as needed
            }

            // Acknowledge Endpoint 6 OUT (Bulk) data
            EP6FIFOFLGS = 0x04; // Clear EP_EMPTY flag
            EP6FIFOIRQ = 0x04;  // Clear EP_EMPTY IRQ flag
        }

        // ... Add other tasks as needed
    }
}