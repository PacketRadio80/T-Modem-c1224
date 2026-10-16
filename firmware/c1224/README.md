# T-Modem-c1224 — Pico USB KISS half-TNC firmware

**AS-IS** · GPLv3 (repo `LICENSE`) · no support, no warranty · **v1.0-prototype**.

**One** UF2 for Raspberry Pi Pico (RP2040). Chip does air AFSK; Pico is **USB KISS half-TNC** (HDLC + bitbang to TCM3105 TXD/RXD) plus display / PTT / sense.

| Owns | Does **not** |
|------|----------------|
| HDLC TX/RX (flags, bit-stuff, FCS-CCITT) | Softmodem / AFSK DSP |
| Bitbang GP6 TXD / GP7 RXD at crystal rate | Transparent Host↔Chip bit bridge |
| USB CDC: KISS `0xC0` + ASCII service | Baud switch / XTAL mux |
| Auto PTT GP9 during KISS TX · `PTT ON`/`OFF` | Dual baud UF2 builds |
| Display / INI texts · CDT Signal · packet sense · fixed boot banner | INI/CDC boot-banner toggle |

## Build

Requires [Pico SDK](https://github.com/raspberrypi/pico-sdk) and `arm-none-eabi-gcc`.

```bash
export PICO_SDK_PATH=/path/to/pico-sdk   # or repo .deps/pico-sdk
cd firmware/c1224
mkdir -p build && cd build
cmake .. -DC1224_BITRATE=1200    # or 2400 if Y2 stuffed
make -j$(nproc) c1224
# → c1224.uf2  c1224.elf
```

`C1224_BITRATE` must match the stuffed crystal (**Y1 4.4336 → 1200**, **Y2 6.5536 → 2400**). Default **1200**. Same source tree; rebuild if field board is 2400.

### Flash

1. Hold **BOOTSEL**, plug USB (Path B — flash/service; board power = **12 V** barrel).
2. Copy `c1224.uf2` to RPI-RP2 (or `picotool load -f c1224.uf2`).
3. Host opens `/dev/ttyACM*` (USB CDC).

**Unattended:** CLI `BOOTSEL`/`DFU` enters the bootloader · `REBOOT` restarts · 1200bps DTR-drop touches the bootloader too.

## Any KISS terminal (standalone — no MAX25)

**MAX25-Stack is not required.** No proprietary host protocol. **All terminals should work insofar as applicable** — any client that speaks **KISS and/or ASCII service** over a raw serial port. Optional convenience only: max25d device **`tmodem`** / `max25-terminal`.

How-to: flash UF2 → plug USB → open `/dev/ttyACM*` → KISS `0xC0` or ASCII `HELP`.

| Step | Detail |
|------|--------|
| Device | `/dev/ttyACM*` (Linux) / equivalent CDC ACM |
| Framing | Raw **8N1** · binary **KISS** (`0xC0` … `0xC0`) · DATA cmd `0x00` |
| Params | `TXDELAY` / `TXTAIL` apply · `SLOTTIME` / `PERSIST` / `FULLDUP` ignored with notice |
| Packet path | Send/receive KISS frames only — do **not** interleave ASCII while mid-frame |
| Service path | Optional line ASCII (`STATUS`, `PTT`, …) when **not** inside a KISS frame |
| Boot | Fixed source banner **always** once after CDC ready · `quiet=1` damps extra STATUS/CONFIG ASCII only |

Examples: **kissattach** / ax25 tools, **kissutil**, Dire Wolf serial KISS, custom terminal, or optionally **max25-terminal** via **`tmodem`**.

```bash
# Example: open CDC raw and use KISS (tool-dependent)
# stty -F /dev/ttyACM0 raw -echo
# kissutil -p /dev/ttyACM0
# # or: kissattach /dev/ttyACM0 tm0   (ax25-tools; names vary by distro)
```

## Dual protocol (USB CDC)

| Traffic | Rule |
|---------|------|
| Packet | Bytes starting with / continuing **`0xC0` KISS** → HDLC TX / RX encode |
| Service | Line-oriented ASCII → `PTT`, `STATUS`, `INI`, … |
| Custom terminals | Prefer **KISS-only** for packet; service commands optional |

## Host path (summary)

| Traffic | Format |
|---------|--------|
| Packet | Binary **KISS** frames (`0xC0` … `0xC0`) — DATA cmd `0x00` |
| Service | Line ASCII: `PTT`, `STATUS`, `ID`, `QUIET`/`VERBOSE`, `INI` / `GET` / `SET` / `SAVE` |

## Pins

| GPIO / rail | Role |
|-------------|------|
| GP0 / GP1 | I2C LCD |
| GP6 | Modem **TXD** — HDLC bitbang out |
| GP7 | Modem **RXD** — HDLC bitbang in + Packet UI sense |
| GP8 | Modem **CDT** → Signal/RX |
| GP9 | **PTT** (auto on TX + manual CDC) |
| GP10–12 | Optional LEDs |
| — | LED_PWR on **5 V rail** |

## Boot banner

Fixed source text in `src/common/banner.c` / `include/version.h` (not INI / CDC). **Always** printed once after USB CDC ready. Edit source only — no `banner=` INI key, no `BANNER ON|OFF`. `quiet=1` never suppresses the boot banner (only extra STATUS/PROBE/CONFIG ASCII).

## USB CDC ASCII

| Command | Effect |
|---------|--------|
| `HELP` | Command list |
| `STATUS` | state, PTT, signal, packet, flag, bitrate, raw CDT/RXD, `gp9` |
| `ID` | `device_id` / `callsign` / bitrate / quiet (solicited) |
| `PTT ON` / `PTT OFF` | Key GP9 |
| `QUIET ON` / `OFF` | Suppress extra unsolicited CDC ASCII (default ON via INI `quiet=1`; never the boot banner) |
| `VERBOSE ON` / `OFF` | Alias: `QUIET OFF` / `QUIET ON` |
| `INI` / `GET` / `SET` / `SAVE` / `RELOAD` / `DEFAULTS` | Config |
| `BOOTSEL` / `DFU` | Enter UF2 bootloader (unattended flash) |
| `REBOOT` | Restart the firmware |

**`SAVE`:** writes a config **blob** into the **last 4 KiB** of Pico flash — **not** a filesystem.

## INI (`[terminal]`)

| Key | Default | Role |
|-----|---------|------|
| `callsign` | empty | Operator call / station string for `ID` |
| `device_id` | `c1224` | Short device mark (`c1224` or `tmodem`) |
| `quiet` | `1` | Suppress extra unsolicited CDC ASCII (not boot banner) |

## INI (`[modem]`)

| Key | Default | Role |
|-----|---------|------|
| `tx_delay_ms` | `500` | PTT key-up before the first data bit (KISS `TXDELAY` × 10 ms) |
| `txtail_ms` | `100` | Tail after the last data bit before PTT off (KISS `TXTAIL` × 10 ms) |

Display text keys (`starting`, `ready`, …) and sense keys unchanged. See [`share/c1224.ini.example`](../../share/c1224.ini.example).

## Rate

Stuff **Y1 4.4336** (1200) or **Y2 6.5536** (2400). Firmware does not switch baud at runtime. Rebuild with `-DC1224_BITRATE=2400` for Y2 boards.
