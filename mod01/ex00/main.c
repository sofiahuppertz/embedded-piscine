#include <avr/io.h>

/* Cost of one iteration of the delay loop below, measured from the
 * disassembly (make asm) and the "Instruction Set Summary"
 * table of the datasheet (#Clocks column):
 *   4 x LDD  (2) =  8   load the 32-bit counter from the stack
 *   1 x SBIW (2) =  2   subtract 1 from the low 16 bits
 *   2 x SBC  (1) =  2   propagate the borrow to the high bytes
 *   4 x STD  (2) =  8   store the counter back (forced by volatile)
 *   3 x OR   (1) =  3   merge the 4 bytes to test for zero
 *   1 x BRNE (2) =  2   branch back while non-zero (2 cycles when taken)
 *                 ----
 *                  25 cycles per iteration */
#define CYCLES_PER_ITERATION 25

// Iterations for a 0.5 s delay: cycles in 0.5 s / cycles per iteration
#define HALF_SECOND_ITERATIONS (F_CPU / 2 / CYCLES_PER_ITERATION)

int main(void)
{

    // Datasheet: "I/O-Ports" > "Configuring the Pin", DDRB register description.
    DDRB = (1 << PB1);

    while (1) {

        // volatile stops the optimizer from removing the loop, since its body is empty.
        volatile uint32_t cycles = HALF_SECOND_ITERATIONS;
        while (--cycles)
            ;

        // Datasheet: "I/O-Ports" > "Toggling the Pin".
        PINB = (1 << PB1);
    }
}
