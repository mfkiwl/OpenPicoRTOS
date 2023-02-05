#ifndef PICORTOSCONFIG_H
#define PICORTOSCONFIG_H

/* CLOCKS */
#define CONFIG_SYSCLK_HZ        32768 /* Periodic clock is INT32K */
#define CONFIG_TICK_HZ          1000

/* TASKS */
#define CONFIG_TASK_COUNT       1

/* STACK */
#define CONFIG_DEFAULT_STACK_COUNT 64

/* IPCs */
#define CONFIG_ARCH_EMULATE_ATOMIC
#define CONFIG_DEADLOCK_COUNT 1000

#endif