# T-Modem-c1224

**Prototype Notice:** This repository contains hardware prototypes that have **passed all automated software tests** (unit, integration, stress). **Practical field testing with real radio equipment is still pending.** Use at your own risk. GPLv3 — non-profit.

---

USB KISS half-TNC for Raspberry Pi Pico. Chip PHY via TCM3105-class modem IC with crystal-selected baud rate (1200 / 2400). HDLC + bitbang TX/RXD, PTT, LCD display, sense inputs.

**Firmware language:** C · **License:** GPLv3 · **Platform:** RP2040 (Raspberry Pi Pico)

**Docs:** [`docs/dok/README.md`](docs/dok/README.md) — complete documentation (build · flash · commission · operate · host · troubleshooting).

## Firmware Images

Pre-built UF2 files in `firmware/c1224/bin/`:

| File | Baud | Crystal | Description |
|------|------|---------|-------------|
| `c1224-1200.uf2` | 1200 | Y1 (4.4336 MHz) | Bell-202 AFSK standard |
| `c1224-2400.uf2` | 2400 | Y2 (6.5536 MHz) | Higher-rate packet |

**One board = one rate = one crystal.** Rebuild firmware if you change crystals.

## Quick Guide — Firmware Update

### Prerequisites
- Raspberry Pi Pico with USB cable
- T-Modem-c1224 board (or any RP2040 board for testing)

### Flash via BOOTSEL (Recommended)

1. **Disconnect** the Pico from USB.
2. **Hold the BOOTSEL button** on the Pico while plugging in USB.
3. The Pico appears as a USB mass storage device (`RPI-RP2`).
4. **Copy** the appropriate `.uf2` file to the `RPI-RP2` drive:
   ```bash
   # Linux
   cp firmware/c1224/bin/c1224-2400.uf2 /media/$USER/RPI-RP2/
   ```
5. The Pico **automatically reboots** and runs the new firmware.
6. Verify: open `/dev/ttyACM0` at 115200 baud, send `STATUS` → should respond.

### Flash via picotool (Advanced)

```bash
picotool load firmware/c1224/bin/c1224-2400.uf2 -x
```

**Unattended (CLI):** send `BOOTSEL` (or `DFU`) to enter the UF2 bootloader, `REBOOT` to restart. Opening the port at **1200 baud** and dropping **DTR** also enters the bootloader (classic 1200bps touch).

### Build from Source

```bash
export PICO_SDK_PATH=/path/to/pico-sdk
cd firmware/c1224
mkdir build && cd build
cmake .. -DC1224_BITRATE=2400   # 1200 or 2400
make -j$(nproc)
# → c1224.uf2
```

## Operation

1. Power from **12 V** barrel (centre-positive). USB = data only.
2. Connect USB to host. Open `/dev/ttyACM0` at 115200 baud.
3. **KISS mode:** Send `0xC0`-framed packets (standard KISS TNC).
4. **Service mode:** Send ASCII commands (`HELP`, `STATUS`, `PTT ON/OFF`, `ID`).

### ASCII Commands

| Command | Description |
|---------|-------------|
| `HELP` | List available commands |
| `STATUS` | Show modem state, PTT, signal, bitrate |
| `ID` | Show device ID, callsign, bitrate |
| `PTT ON` / `PTT OFF` | Manual PTT control |
| `QUIET ON/OFF` | Enable/disable unsolicited output |
| `INI` | Dump configuration |
| `GET <key>` | Read config value |
| `SET <key>=<value>` | Set config value (RAM only) |
| `SAVE` | Persist config to flash |
| `RELOAD` | Reload config from flash |
| `DEFAULTS` | Reset to factory defaults |
| `BOOTSEL` / `DFU` | Enter UF2 bootloader (unattended flash) |
| `REBOOT` | Restart the firmware |

## Hardware

- **Schematic:** `hardware/kicad/`
- **BOM:** `hardware/BOM.md` and `hardware/bom/BOM.csv`
- **Crystal:** Y1 (4.4336 MHz) for 1200 baud, Y2 (6.5536 MHz) for 2400 baud
- **PTT:** GP9 → optocoupler → radio PTT input
- **Sense:** GP8 (CDT/carrier detect), GP7 (RXD/packet detect)
- **Display:** I2C HD44780 via PCF8574 backpack (optional)
- **Missing component (nachrüstbar):** the PCB has **no TX AF output buffer** → weak/distorted TX audio (TCM3105 `TXA` needs ≥50 kΩ load, but `RV1` loads it with 10 kΩ). **Retrofit (Lösung 1):** unity-gain op-amp buffer between `TXA` and `RV1` — see `hardware/BOM.md` §6.1.

## Architecture

```
Host (USB CDC) ←→ Pico (RP2040) ←→ TCM3105 modem chip ←→ Radio
                  │
                  ├── KISS parser (0xC0 framing)
                  ├── HDLC encode/decode
                  ├── AFSK bitbang (GP6 TXD, GP7 RXD)
                  ├── PTT control (GP9)
                  ├── Sense (GP8 CDT, GP7 RXD)
                  ├── I2C display (optional)
                  └── Config (flash storage)
```

## Project Structure

```
T-Modem-c1224/
├── firmware/c1224/
│   ├── src/common/          # C source files
│   ├── include/             # Headers
│   ├── config/              # Default INI
│   ├── bin/                 # Pre-built UF2 images
│   └── CMakeLists.txt
├── hardware/
│   ├── kicad/               # Schematic + PCB
│   ├── bom/                 # Bill of materials
│   └── BOM.md
├── docs/                    # User guide, figures
└── share/                   # Example INI files
```

## License

GNU General Public License v3.0 — see [LICENSE](LICENSE).

**Non-profit project.** Not for commercial use without permission.