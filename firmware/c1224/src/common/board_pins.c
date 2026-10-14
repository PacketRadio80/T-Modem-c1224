/*
 * Board GPIO setup — T-Modem-c1224 service Pico.
 * Copyright (C) 2026 T-Modem contributors
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "board_pins.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

void board_pins_init(void)
{
    /* Modem digital */
    gpio_init(PIN_MODEM_TXD);
    gpio_set_dir(PIN_MODEM_TXD, GPIO_OUT);
    gpio_put(PIN_MODEM_TXD, 0);

    gpio_init(PIN_MODEM_RXD);
    gpio_set_dir(PIN_MODEM_RXD, GPIO_IN);
    gpio_pull_up(PIN_MODEM_RXD);

    gpio_init(PIN_MODEM_CDT);
    gpio_set_dir(PIN_MODEM_CDT, GPIO_IN);
    /* CDT polarity is chip-dependent; no pull forced beyond weak up for floating unpopulated */
    gpio_pull_up(PIN_MODEM_CDT);

    /* GP14/GP15 mux placeholders — leave unused (single-XTAL v1.0) */

    /* LEDs */
    gpio_init(PIN_LED_PTT);
    gpio_set_dir(PIN_LED_PTT, GPIO_OUT);
    gpio_put(PIN_LED_PTT, 0);
    gpio_init(PIN_LED_DCD);
    gpio_set_dir(PIN_LED_DCD, GPIO_OUT);
    gpio_put(PIN_LED_DCD, 0);
    gpio_init(PIN_LED_TXRX);
    gpio_set_dir(PIN_LED_TXRX, GPIO_OUT);
    gpio_put(PIN_LED_TXRX, 0);

    /* I2C for LCD backpack (may be absent — display layer tolerates NACK) */
    i2c_init(i2c0, I2C_LCD_BAUDRATE);
    gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C_SDA);
    gpio_pull_up(PIN_I2C_SCL);
}
