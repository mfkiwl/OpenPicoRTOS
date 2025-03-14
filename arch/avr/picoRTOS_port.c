#include "picoRTOS_port.h"
#include "picoRTOS_device.h"

#include <generated/autoconf.h>

/* ASM */
/*@external@*/ extern /*@temp@*/
picoRTOS_stack_t *arch_save_first_context(picoRTOS_stack_t *sp,
                                          arch_entry_point_fn fn,
                                          /*@null@*/ void *priv);

/*@external@*/ extern void arch_start_first_task(picoRTOS_stack_t *sp);

/* AVR is one of the rare CPUs that can switch contexts without an interrupt,
 * so this is directly defined in assembly language */
/*@external@*/ extern void arch_syscall(syscall_t syscall, void *priv);

/*@external@*/ extern void arch_timer_init(void);

/* FUNCTIONS TO IMPLEMENT */

void arch_init(void)
{
    /* hack to ensure picoRTOS_tick exists as a symbol */
    arch_assert(picoRTOS_tick != NULL, return );

    /* disable interrupts */
    ASM("cli");
    /* TIMER */
    arch_timer_init();
}

void arch_suspend(void)
{
    /* disable tick */
    ASM("cli");
}

void arch_resume(void)
{
    /* enable tick */
    ASM("sei");
}

picoRTOS_stack_t *arch_prepare_stack(picoRTOS_stack_t *stack,
                                     size_t stack_count,
                                     arch_entry_point_fn fn,
                                     void *priv)
{
    arch_assert_void(stack_count >= (size_t)ARCH_MIN_STACK_COUNT);
    /* AVRs have a post-decrementing stack */
    return arch_save_first_context(stack + (stack_count - 1), fn, priv);
}

void __attribute__((weak)) arch_idle(const void *null)
{
    arch_assert_void(null == NULL);

    for (;;)
        ASM("sleep");
}

/* ATOMIC OPS EMULATION */

picoRTOS_atomic_t arch_compare_and_swap(picoRTOS_atomic_t *var,
                                        picoRTOS_atomic_t old,
                                        picoRTOS_atomic_t val)
{
    ASM("cli");

    if (*var == old) {
        *var = val;
        val = old;
    }

    ASM("sei");
    return val;
}

picoRTOS_atomic_t arch_test_and_set(picoRTOS_atomic_t *ptr)
{
    return arch_compare_and_swap(ptr, (picoRTOS_atomic_t)0,
                                 (picoRTOS_atomic_t)1);
}

/* INTERRUPTS MANAGEMENT */

/*@external@*/
extern struct {
    arch_isr_fn fn;
    /*@temp@*/ /*@null@*/ void *priv;
} ISR_TABLE[DEVICE_INTERRUPT_VECTOR_COUNT];

void arch_register_interrupt(picoRTOS_irq_t irq, arch_isr_fn fn, void *priv)
{
    arch_assert(irq > (picoRTOS_irq_t)0, return );
    arch_assert(irq < (picoRTOS_irq_t)(DEVICE_INTERRUPT_VECTOR_COUNT + 1), return );

    ISR_TABLE[irq - 1].fn = fn;
    ISR_TABLE[irq - 1].priv = priv;
}

void arch_enable_interrupt(picoRTOS_irq_t irq)
{
    arch_assert_void(irq > (picoRTOS_irq_t)0);
    arch_assert_void(irq < (picoRTOS_irq_t)(DEVICE_INTERRUPT_VECTOR_COUNT + 1));
    /* no effect */
}

void arch_disable_interrupt(picoRTOS_irq_t irq)
{
    arch_assert_void(irq > (picoRTOS_irq_t)0);
    arch_assert_void(irq < (picoRTOS_irq_t)(DEVICE_INTERRUPT_VECTOR_COUNT + 1));
    /* no effect */
}

/* CACHES */

void arch_invalidate_dcache(/*@unused@*/ void *addr, /*@unused@*/ size_t n)
{
    /*@i@*/ (void)addr;
    /*@i@*/ (void)n;
}

void arch_flush_dcache(/*@unused@*/ void *addr, /*@unused@*/ size_t n)
{
    /*@i@*/ (void)addr;
    /*@i@*/ (void)n;
}
