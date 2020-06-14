#include <stdint.h>
#include <stdio.h>

#include <libopencm3/cm3/scb.h>

void hardFaultDebug(uint32_t *sp);
void hard_fault_handler(void);
void mem_manage_handler(void);
void bus_fault_handler(void);
void usage_fault_handler(void);

void hardFaultDebug(uint32_t *sp)
{
    // https://interrupt.memfault.com/blog/cortex-m-fault-debug
    // https://github.com/ferenc-nemeth/arm-hard-fault-handler/blob/master/Src/FaultHandler.c
    // https://www.silabs.com/community/mcu/32-bit/knowledge-base.entry.html/2014/05/26/debug_a_hardfault-78gc

    uint32_t cfsr = SCB_CFSR;
    uint32_t hfsr = SCB_HFSR;

    uint32_t r0  = sp[0];
    uint32_t r1  = sp[1];
    uint32_t r2  = sp[2];
    uint32_t r3  = sp[3];
    uint32_t r12 = sp[4];
    uint32_t lr  = sp[5];
    uint32_t pc  = sp[6];
    uint32_t psr = sp[7];

    printf("HardFault\n");
    printf("CFSR    0x%08lx\n", cfsr);
    printf("HFSR    0x%08lx\n", hfsr);
    printf("SP      0x%08lx\n", (uint32_t)sp);
    printf("R0      0x%08lx\n", r0);
    printf("R1      0x%08lx\n", r1);
    printf("R2      0x%08lx\n", r2);
    printf("R3      0x%08lx\n", r3);
    printf("R12     0x%08lx\n", r12);
    printf("LR      0x%08lx\n", lr);
    printf("PC      0x%08lx\n", pc);
    printf("PSR     0x%08lx\n", psr);

    __asm__("bkpt #0");

    for (;;)
        ;
}

__attribute__((naked)) void hard_fault_handler(void)
{
    // Get correct stack pointer and jump to hardFaultDebug
    __asm__("tst   lr, #4         ;"
            "ite   eq             ;"
            "mrseq r0, msp        ;"
            "mrsne r0, psp        ;"
            "b     hardFaultDebug ;");
}

void mem_manage_handler(void)
{
    printf("mem_manage_handler");
    for (;;)
        ;
}

void bus_fault_handler(void)
{
    printf("bus_fault_handler");
    for (;;)
        ;
}

void usage_fault_handler(void)
{
    printf("usage_fault_handler");
    for (;;)
        ;
}
