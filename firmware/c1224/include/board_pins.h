/**
 * T-Modem-c1224 pin map (service Pico — not softmodem).
 * Frozen copper intent (v0.25 schematic) reused as planning default for 180×180.
 * GP14/GP15 = unused mux placeholders for v1.0 (single-XTAL; stuff matches image).
 */
#ifndef C1224_BOARD_PINS_H
#define C1224_BOARD_PINS_H

#include <stdint.h>
#include <stdbool.h>

/* I2C LCD (shared-kit class; level-shifted on board) */
#define PIN_I2C_SDA           0  /* GP0 */
#define PIN_I2C_SCL           1  /* GP1 */
#define I2C_LCD_BAUDRATE      100000
#define I2C_LCD_ADDR_DEFAULT  0x27  /* common PCF8574 backpack; probe may fail → log-only */

/* Modem chip digital (TCM3105-class) — Pico does NOT generate AFSK */
#define PIN_MODEM_TXD         6  /* GP6 → chip TXD */
#define PIN_MODEM_RXD         7  /* GP7 ← chip RXD → Packet UI sense (required · status only) */
#define PIN_MODEM_CDT         8  /* GP8 ← chip CDT → Signal/RX (required) */

/* PTT keying: Pico GPIO → OC → radio PTT (USB CDC service). Not the TX LED. */
#define PIN_PTT_DRV           9  /* GP9 · required for USB PTT ON */

/*
 * Status LEDs (c1224 copper — active-high source via series R; LED cathode = GND).
 * LED_PWR is NOT a GPIO: +5V_OR → R_LED1 → LED_PWR → GND (rail Power indicator).
 *
 * Settled UI (2026-07-26):
 *   - Dual sense REQUIRED: CDT→GP8 Signal/RX · RXD→GP7 Packet (status only, not data bridge).
 *   - CDT = carrier/tone · RXD activity ≠ full AX.25 decode.
 *   - LED_PTT (GP10) = OPTIONAL/SPARE — not required product TX indicator.
 *   - Display TX optional; PTT keying GPIO (GP9) stays separate from LED_PTT.
 *   - LED_TXRX = spare/activity.
 */
#define PIN_LED_PTT           10 /* GP10 · LED_PTT_N · optional/spare (not required TX LED) */
#define PIN_LED_DCD           11 /* GP11 · LED_DCD_N · optional RX mirror of CDT */
#define PIN_LED_TXRX          12 /* GP12 · LED_TXRX_N · spare / optional activity */

/*
 * Crystal mux placeholders — UNUSED for v1.0.
 * Product: one crystal stuffed matching flash (Y1 4.4336 / Y2 6.5536).
 * Dual/mux path abandoned; crystal_steer is a no-op.
 * Optional keep defines if mux ever returns later.
 */
#define PIN_XTAL_SEL_A        14 /* GP14 — unused v1.0 */
#define PIN_XTAL_SEL_B        15 /* GP15 — unused v1.0 */

void board_pins_init(void);

#endif /* C1224_BOARD_PINS_H */
