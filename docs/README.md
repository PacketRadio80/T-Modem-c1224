# Docs — T-Modem-c1224

| Doc | Role |
|-----|------|
| [`USER-GUIDE.md`](USER-GUIDE.md) | Anyone — what it is · assemble · flash · terminals · SAVE |
| [`../hardware/BOM.md`](../hardware/BOM.md) | Planning BOM v0.26 (tracked) |
| [`POWER.md`](POWER.md) | 12 V primary · USB data-only |
| [`../firmware/c1224/README.md`](../firmware/c1224/README.md) | Pico service FW (C + Pico SDK) |
| [`figures/`](figures/) | Board overview + assembly steps 1–8 |

## Operate (standalone)

**No MAX25-Stack required.** Flash `c1224.uf2` → plug USB → open `/dev/ttyACM*` → KISS (`0xC0`) or ASCII `HELP`. Fixed boot banner prints once. **All matching terminals work** insofar as applicable. Optional: MAX25 device **`tmodem`** / max25-terminal.

**INI `SAVE`:** last **4 KiB** of Pico flash as a config **blob** — not a filesystem.

## Figures

| File | Content |
|------|---------|
| [`figures/c1224-board-overview.png`](figures/c1224-board-overview.png) | Top-down board · major parts |
| [`figures/fig-01-pcb-standoffs.png`](figures/fig-01-pcb-standoffs.png) | Step 1 — PCB + standoffs |
| [`figures/fig-02-power-section.png`](figures/fig-02-power-section.png) | Step 2 — 12 V → buck → 5 V |
| [`figures/fig-03-modem-crystal.png`](figures/fig-03-modem-crystal.png) | Step 3 — socket · single crystal · Pierce |
| [`figures/fig-04-pico.png`](figures/fig-04-pico.png) | Step 4 — Pico H |
| [`figures/fig-05-display.png`](figures/fig-05-display.png) | Step 5 — I²C display |
| [`figures/fig-06-leds.png`](figures/fig-06-leds.png) | Step 6 — indicators |
| [`figures/fig-07-connectors.png`](figures/fig-07-connectors.png) | Step 7 — AF/PTT · USB |
| [`figures/fig-08-final-check.png`](figures/fig-08-final-check.png) | Step 8 — final check |

**AS-IS** · GPLv3 · non-profit.
