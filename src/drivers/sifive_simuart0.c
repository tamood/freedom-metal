/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <metal/machine/platform.h>

#ifdef METAL_SIFIVE_SIMUART0

#include <metal/init.h>
#include <metal/drivers/sifive_simuart0.h>
#include <metal/machine.h>

/* TXDATA Fields */
#define SIMUART_TXEN (1 << 0)

#define SIMUART_REG(offset) (((unsigned long)control_base + offset))
#define SIMUART_REGB(offset)                                                   \
    (__METAL_ACCESS_ONCE((__metal_io_u8 *)SIMUART_REG(offset)))
#define SIMUART_REGW(offset)                                                   \
    (__METAL_ACCESS_ONCE((__metal_io_u32 *)SIMUART_REG(offset)))

struct metal_interrupt *
metal_uart_interrupt_controller(struct metal_uart *uart) {
    return __metal_driver_sifive_simuart0_interrupt_parent(uart);
}

int metal_uart_get_interrupt_id(struct metal_uart *uart) {
    return (__metal_driver_sifive_simuart0_interrupt_line(uart) +
            METAL_INTERRUPT_ID_GL0);
}

int metal_uart_putc(struct metal_uart *uart, int c) {
    long control_base = __metal_driver_sifive_simuart0_control_base(uart);

    SIMUART_REGW(METAL_SIFIVE_SIMUART0_TXDATA) = c;
    return 0;
}

int metal_uart_getc(struct metal_uart *uart, int *c) {
    return 0;
}

int metal_uart_get_baud_rate(struct metal_uart *guart) {
    struct __metal_driver_sifive_simuart0 *uart = (void *)guart;
    return uart->baud_rate;
}

int metal_uart_set_baud_rate(struct metal_uart *guart,
                                                 int baud_rate) {
    struct __metal_driver_sifive_simuart0 *uart = (void *)guart;
    long control_base = __metal_driver_sifive_simuart0_control_base(guart);
    struct metal_clock *clock = __metal_driver_sifive_simuart0_clock(guart);

    uart->baud_rate = baud_rate;

    if (clock != NULL) {
        long clock_rate = clock->vtable->get_rate_hz(clock);
        SIMUART_REGW(METAL_SIFIVE_SIMUART0_DIV) = clock_rate / baud_rate - 1;
        SIMUART_REGW(METAL_SIFIVE_SIMUART0_TXCTRL) |= SIMUART_TXEN;
    }
    return 0;
}

void metal_uart_init(struct metal_uart *guart,
                                         int baud_rate) {}

#ifdef METAL_STDOUT_SIFIVE_SIMUART0
#if defined(__METAL_DT_STDOUT_UART_HANDLE)

int metal_tty_putc(int c) {
    return metal_uart_putc((struct metal_uart *)__METAL_DT_STDOUT_UART_HANDLE, c);
}

#ifndef __METAL_DT_STDOUT_UART_BAUD
#define __METAL_DT_STDOUT_UART_BAUD 115200
#endif

#endif /* __METAL_DT_STDOUT_UART_HANDLE */
#endif /* METAL_STDOUT_SIFIVE_SIMUART0 */

#endif /* METAL_SIFIVE_SIMUART0 */

typedef int no_empty_translation_units;
