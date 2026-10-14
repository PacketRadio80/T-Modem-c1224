# T-Modem-c1224 — BOM v0.26 (planning)

**Status:** complete **planning BOM** · PCB footprints/nets present · **not** fab release · **single-XTAL** settled  
**Product:** **T-Modem-c1224** · slave `/home/akb/Code/10-PROJECTS/T-Modem-c1224/`  
**License:** GNU **GPLv3** · **non-profit** · **AS-IS**  
**Outline:** **180×180 mm** · routed (see vault healthcheck)  
**Selection:** sicher · stabil · performant · günstig  
**CSV:** [`bom/BOM.csv`](bom/BOM.csv)

**Power law:** **12 V** barrel → buck → **5 V** · **USB VBUS does not power the board** (D_OR2 = **DNI**).

**Starting material:** symbols/footprints adapted from frozen **c12s24** (separate product; not a rename).

---

## Status legend

| Status | Meaning |
|--------|---------|
| **STOCK** | On hand from classic kit / prior buy |
| **BUY** | Need for first bring-up |
| **OPTIONAL** | May ship unpopulated |
| **TBD** | Class settled · exact MPN / MHz open |
| **DNI** | Do not install / leave unconnected |
| **DEFERRED** | Do not buy yet |
| **OUT** | Explicitly not in product |

---

## 0 — Operator summary

| When | What |
|------|------|
| **STOCK** | Pico H class · DIP-16 modem socket · TCM3105-class · 16×2 LCD · AF screw · LEDs · passives · barrel jack · USB Type-A · buck module class |
| **BUY (crystal)** | Per intended rate: **Y1 = 4.4336 MHz** (flash **1200**) **or** **Y2 = 6.5536 MHz** (flash **2400**) · **8.8672 deferred** |
| **Rate model** | **One board = one flashed rate = one crystal** · other footprint **DNI** · dual footprints optional · second board for other rate |
| **Mux** | **Not required** · dual/mux path abandoned for v1.0 |
| **DNI** | **D_OR2** USB→5 V Schottky OR · unused crystal footprint |
| **OUT** | Softmodem AF path · Pi host · T-PROT · XTAL mux IC |

---

## 1 — Modem PHY (chip 1200 / 2400)

| Ref | Qty | Description | Package | Candidate | Status |
|-----|-----|-------------|---------|-----------|--------|
| SKT_MODEM | 1 | 16-pin DIP socket 0.3″ | DIP-16 7.62 mm | Generic / turned-pin | **STOCK** |
| U_MODEM | 1 | Bell-202 AFSK modem IC | DIP-16 in socket | **TCM3105** or MX614P-class | **STOCK** |
| Y1 | 0/1 | Crystal chip **1200** | HC-49 TH | **4.4336 MHz** (Amateur-PR / Bell-202) | **BUY** if 1200 board · else **DNI** |
| Y2 | 0/1 | Crystal chip **2400** | HC-49 TH | **6.5536 MHz** (field 2400) · **8.8672 deferred** | **BUY** if 2400 board · else **DNI** |
| C_Y1 | 1 | Pierce load OSC1↔GND | TH ceramic | ~22 pF class | **STOCK** / **TBD** |
| C_Y2 | 1 | Pierce load OSC2↔GND | TH ceramic | ~22 pF class · **keep** with single XTAL | **STOCK** / **TBD** |
| U_XTAL_MUX | 0 | Crystal mux / analog switch | — | **Not for v1.0** | **OUT** |
| RV_CDL | 1 | CD level trimmer | TH | Commodity | **STOCK** |
| R_TRS, R_TXR* | 1–3 | Bell-202 strap resistors | 0.25 W TH | Per frozen strap practice | **STOCK** |

Pico GPIOs **GP14/GP15** are **unused** for v1.0 (optional pin placeholders in FW). Rate = **stuffed crystal** · **one** FW image · no mux drive · dual UF2 **obsolete**.

**Stuffing:** keep both crystal footprints optional on copper; populate **only** the crystal for the intended rate; **never** stuff both in parallel. **C_Y1 + C_Y2** are the Pierce load pair for that single crystal — do **not** remove C_Y2 when leaving Y2 empty.

---

## 2 — Pico / service

| Ref | Qty | Description | Candidate | Status |
|-----|-----|-------------|-----------|--------|
| U1 | 1 | MCU module native USB UF2 | **Raspberry Pi Pico H** (headers) | **STOCK** |
| HDR_PICO_F | 2 | Female 1×20 2.54 mm | Commodity | **STOCK** |
| SW1 | 1 | BOOTSEL N.O. tactile | Omron B3F class | **STOCK** |
| TP_BOOTSEL | 1 | Test point optional | — | **OPTIONAL** |

---

## 3 — Display (shared kit)

| Ref | Qty | Description | Candidate | Status |
|-----|-----|-------------|-----------|--------|
| DSP1 | 1 | 16×2 HD44780 LCD green BL | Commodity | **STOCK** |
| HDR_LCD_F / J_LCD_I2C | 1 | Header / I²C connector | As on PCB | **STOCK** |
| U_LVL | 0/1 | BSS138-class I²C level shift | LV=3V3 · HV=5 V | **STOCK** |
| RV2 | 1 | 10 kΩ contrast | TH | **STOCK** |
| R_BL | 0/1 | Backlight series R | Per module | **STOCK** |

---

## 4 — Power (12 V primary · no USB power)

| Ref | Qty | Description | Candidate | Status |
|-----|-----|-------------|-----------|--------|
| J_DC | 1 | Barrel **5.5×2.1** centre-**positive** | CUI PJ-002A class | **STOCK** |
| U_REG | 1 | **12 V → 5 V** buck module | **Mini560 5 V** class preferred if stocked · else MP1584/LM2596 class | **STOCK** / **BUY** |
| HDR_REG | 1 | Header for buck | 2.54 mm | **STOCK** |
| D_OR1 | 1 | Schottky buck → +5 V | 1N5819 class | **STOCK** |
| D_OR2 | 0 | USB VBUS → 5 V OR | — | **DNI** |
| F1 | 1 | PTC on +5 V | ~0.5 A hold class | **STOCK** |
| C_IN | 1 | 100 µF / 25 V | radial | **STOCK** |
| C_BULK | 1 | 100 µF / 16 V on +5 V | radial | **STOCK** |
| C_BYP* | ≥8 | 100 nF decoupling | TH | **STOCK** |
| PSU_WALL | 1 | 12 V centre+ wall wart ≥0.5 A | Commodity | Not on PCB |

Silk: **12 V CENTRE +** · **USB DATA ONLY — NO VBUS POWER**.

---

## 5 — USB (data / flash / command only)

| Ref | Qty | Description | Candidate | Status |
|-----|-----|-------------|-----------|--------|
| J_USB | 1 | USB Type-A receptacle TH | Amphenol/TE/Molex class | **STOCK** |
| J_USB_PICO | 1 | Path to Pico USB (D+/D−/GND) | As designed | **STOCK** |

**Net:** `USB_VBUS_NC` — isolated · not tied to `+5V_OR` / `+5V`.

---

## 6 — AF / PTT / indicators

| Ref | Qty | Description | Candidate | Status |
|-----|-----|-------------|-----------|--------|
| J_AF | 1 | 4-pin screw 5.08 mm | `AF_TX` `AF_RX` `PTT` `GND` | **STOCK** |
| Q1 | 1 | NPN OC PTT | 2N2222A / BC547 | **STOCK** |
| R_PTT | 1 | 4.7 kΩ | TH | **STOCK** |
| RV1 | 1 | 10 kΩ TX AF level | TH | **STOCK** |
| C_TX, C_RX / C_AC* | 2 | AF AC couple | TH | **STOCK** |
| R_PAD* | 2–4 | Pad/bias | TH | **STOCK** |
| D_PROT | 0/2 | 1N4148 clamp | TH | **OPTIONAL** |
| LED_PWR, LED_PTT, LED_DCD, LED_TXRX | 4 | 3 mm LED | Commodity | **STOCK** |
| R_LED1–4 | 4 | 330 Ω–1 kΩ | TH | **STOCK** |

---

## 7 — Explicitly out of BOM

| Item | Reason |
|------|--------|
| Softmodem / DSP AFSK on Pico | Product line forbids |
| USB VBUS board power / OR diode stuffed | Power law |
| Pi host / CM108 air path | That is **384b** |
| Protocol T / T-PROT | Classic only |
| XTAL mux IC (74HCT4053 class etc.) | Dual path abandoned for v1.0 |

---

## Open TBDs

1. ~~XTAL mux topology + MPN~~ — **OUT** / abandoned  
2. Final buck MPN (Mini560 5 V vs on-hand MP1584)  
3. Whether strap resistors need Pico GPIO  
4. Expected tones for **6.5536** path (AS-IS)

**AS-IS** · no support · no warranty.
