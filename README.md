# Pulsar signal dispersion & dedispersion simulation

A C project simulating a pulsar's radio emission as it would be captured by a multi-channel radio telescope, accounting for **interstellar dispersion**, then implementing the **dedispersion** algorithm to recover the original signal.

## Physical background

A pulsar emits short, periodic radio pulses across a wide range of frequencies simultaneously (broadband emission). As the signal travels through the interstellar medium (ionized gas), it gets delayed differently depending on frequency: lower frequencies arrive later than higher ones, following a `1/f²` law. This is **dispersion**.

A telescope observes this band split into several frequency channels. Without correction, the pulse appears smeared out in time across all channels (a parabolic "sweep"), buried in noise on each individual channel. **Dedispersion** consists of shifting each channel back by its expected delay (computed from the pulsar's dispersion measure, DM) and then summing them, making a sharp peak reappear.

## Project files

- [`generate.c`](generate.c) — generates the simulated signal: a 2D array `signal[channel][i]` containing a Gaussian pulse per channel, shifted in time according to the dispersion law, plus noise. Writes the result to `signal.csv`.

## Build

```bash
gcc -Wall -Wextra -Werror -std=c17 -g -fsanitize=address,undefined -lm -o generate generate.c
./generate
```

## Physical parameters (constants in `generate.c`)

| Constant | Role | Current value |
|---|---|---|
| `DT` | Time resolution: real duration represented by one sample | to be set (precision / array-size trade-off, see below) |
| `N` | Number of time samples per channel | to be set together with `DT` to cover the max dispersion delay |
| `N_CHANNEL` | Number of frequency channels | 32 |
| `F_MIN`, `F_MAX` | Observed frequency band (MHz) | 1200 – 1500 |
| `F_REF` | Reference frequency (zero delay) | `F_MAX` |
| `DM` | Pulsar's dispersion measure (pc/cm³) | 56.8 (Crab pulsar, [ATNF catalogue](https://www.atnf.csiro.au/research/pulsar/psrcat/)) |
| `K_DM` | Physical dispersion constant (MHz² pc⁻¹ cm³ s) | 4148.8 |
| `PULSE_POS` | Pulse position (index) for the reference channel | to be set based on the expected max delay |
| `PULSE_WIDTH` | Gaussian pulse width (in samples) | 5 |

**`DT` / `N` trade-off**: `DT` must be small enough to properly resolve `PULSE_WIDTH` (the signal's time detail), but the total observable duration (`N × DT`) must exceed the maximum dispersion delay (`delta_t` between the lowest channel and `F_REF`), otherwise the peak falls outside the array for the most delayed channels.

## Output format (`signal.csv`)

Long format, one row per data point:
```
canal,i,amplitude
0,0,0.000000
0,1,0.000012
...
```

## Visualization

The CSV can be plotted in Python (2D or 3D waterfall) to observe the dispersion curve:

```python
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("signal.csv")
pivot = df.pivot(index="canal", columns="i", values="amplitude")

plt.imshow(pivot.values, aspect="auto", origin="lower", cmap="viridis", interpolation="nearest")
plt.xlabel("i (time)")
plt.ylabel("channel")
plt.colorbar(label="amplitude")
plt.show()
```

## Project steps

- [x] Simulate a noisy Gaussian pulse, single channel
- [x] Extend to multiple channels with per-channel delay following the dispersion law (`1/f²`)
- [x] CSV export and visualization (2D / 3D waterfall)
- [ ] Dedispersion: shift each channel back by its delay then sum, to recover a clean 1D time profile
- [ ] Simulate a periodic pulse train (multiple pulsar rotations)
- [ ] FFT on the dedispersed profile to recover the pulsar's rotation period

## References

- [ATNF Pulsar Catalogue](https://www.atnf.csiro.au/research/pulsar/psrcat/) — parameters (including DM) of real pulsars
- Crab pulsar (B0531+21): DM ≈ 56.8 pc/cm³
