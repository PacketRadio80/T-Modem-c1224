# T-Modem-c1224 — user guide

**Audience:** anyone · **Version:** v1.0-prototype · **AS-IS** · GPLv3 · non-profit

Classic **chip** AFSK modem (TCM3105-class) + Pico as USB **KISS** half-TNC. Power **12 V**; USB = flash + KISS + ASCII service only.

## What you need

- Assembled board (one crystal: **Y1 4.4336** → 1200 or **Y2 6.5536** → 2400)
- **12 V** centre-positive supply
- USB host + **any** serial/KISS terminal that speaks the CDC interface below

## Assemble · flash (overview)

Figures: [`figures/`](figures/) — steps 1–8 (do not edit PNGs for text-only doc updates).

1. Power path on board · **no** USB VBUS power.  
2. Stuff **one** crystal + both Pierce load caps · insert TCM3105.  
3. Flash **`c1224.uf2`** (BOOTSEL) · match `-DC1224_BITRATE` to crystal.  
4. Apply **12 V** · plug USB for CDC.

Build: [`../firmware/c1224/README.md`](../firmware/c1224/README.md).

## Terminals (all that match)

USB CDC ACM · raw **8N1**:

| Path | Use |
|------|-----|
| **KISS** `0xC0` … `0xC0` | Packet (half-TNC) |
| ASCII lines when not mid-frame | `HELP` · `STATUS` · `PTT` · `ID` · INI |

**All terminals should work insofar as applicable** — serial apps for service ASCII; KISS tools (kissattach, kissutil, Dire Wolf serial, custom, optional max25-terminal) for packet.

Open `/dev/ttyACM*` → fixed boot banner once → KISS or `HELP`.

## CDC commands

`HELP` · `STATUS` · `ID` · `PTT ON`/`OFF` · `QUIET`/`VERBOSE` · `INI`/`GET`/`SET`/`SAVE`/`RELOAD`/`DEFAULTS`

## INI · SAVE

`SET key=value` then **`SAVE`**. Persist storage = **last 4 KiB of Pico flash** as a **config blob** — **not** a filesystem. Example: [`../share/c1224.ini.example`](../share/c1224.ini.example).

## Optional stacks

Standalone is enough. MAX25 (`tmodem`) is optional. HyBBX is not built into this product.

## Power

See [`POWER.md`](POWER.md).

**AS-IS** · no warranty · no support · standard reference only.
