# Pulsar signal dispersion & dedispersion simulation

A C project simulating a pulsar's radio emission as it would be captured by a multi-channel radio telescope, accounting for **interstellar dispersion**, then implementing the **dedispersion** algorithm to recover the original signal.

## Physical background

A pulsar emits short, periodic radio pulses across a wide range of frequencies simultaneously (broadband emission). As the signal travels through the interstellar medium (ionized gas), it gets delayed differently depending on frequency: lower frequencies arrive later than higher ones, following a `1/f²` law. This is **dispersion**.

A telescope observes this band split into several frequency channels. Without correction, the pulse appears smeared out in time across all channels (a parabolic "sweep"), buried in noise on each individual channel. **Dedispersion** consists of shifting each channel back by its expected delay (computed from the pulsar's dispersion measure, DM) and then summing them, making a sharp peak reappear.

## Project files

- [`generate.c`](generate.c) - Generates the simulated signal: a 2D array `signal[channel][i]` containing a Gaussian pulse per channel, shifted in time according to the dispersion law, plus noise. Writes the result to `signal.csv`.

- [`dedispersion.c`](dedispersion.c) - De-disperse the previously generated signal : summing for every instant, the amplitude of every channel, taking into acount the decay (prevously computed in [`generate.c`](generate.c) and stored into [`decays.csv`](decays.csv)). Writes the result to `dedispered.csv`

- [`dft.c`](dft.c) - Computes the Discrete Fourier Transform using a naive/linear approche (O(n))) to find the dominant frequency amongst all.

- [`config.h`](config.h) - Cotains the project's constants.

- [`plot_generated.py`](plot/plot_generated.py) - To visualise the generated signal. Plots the amplitude for each channel with respect to instants.

- [`plot_dedispersed.py`](plot/plot_dedispersed.py) - To visualise the dedispersed signal. Plots the amplitude with respect to instants.

- [`plot_magnitudes.py`](plot/plot_magnitudes.py) - To visualise the fourier spectrum.

## Build

```bash
gcc -Wall -Wextra -Werror -std=c17 -g -fsanitize=address,undefined -lm -o file file.c
./file
```

## Physical parameters (constants defines in [`config.h`](config.h))

| Constant | Role | Current value |
|---|---|---|
| `DT` | Time resolution: real duration represented by one sample | to be set (precision / array-size trade-off, see below) | 10<sup>-3</sup> |
| `N` | Number of time samples per channel | to be set together with `DT` to cover the max dispersion delay | 4096 |
| `N_CHANNEL` | Number of frequency channels | 64 |
| `F_MIN`, `F_MAX` | Observed frequency band (MHz) | 1200 – 1500 |
| `F_REF` | Reference frequency (zero delay) | `F_MAX` |
| `DM` | Pulsar's dispersion measure (pc/cm³) | 56.8 (Crab pulsar, [ATNF catalogue](https://www.atnf.csiro.au/research/pulsar/psrcat/)) |
| `K_DM` | Physical dispersion constant (MHz² pc⁻¹ cm³ s) | 4148.8 |
| `PULSE_POS` | Pulse position (index) for the reference channel | to be set based on the expected max delay | N/2 |
| `PULSE_WIDTH` | Gaussian pulse width (in samples) | 5 |

**`DT` / `N` trade-off**: `DT` must be small enough to properly resolve `PULSE_WIDTH` (the signal's time detail), but the total observable duration (`N × DT`) must exceed the maximum dispersion delay (`delta_t` between the lowest channel and `F_REF`), otherwise the peak falls outside the array for the most delayed channels.

## Output format (`signal.csv`)

Long format, one row per data point:
```
channel,i,amplitude
0,0,0.000000
0,1,0.000012
...
```

## Output format (`dedispersed.csv`)

```
channel,amplitude
0,0.019300
1,0.039726
...
```

## Visualization

The CSVs can be plotted in Python to observe the dispersion curve:

`plot_generated.py` :
```python
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("signal.csv")
pivot = df.pivot(index="channel", columns="i", values="amplitude")

plt.imshow(pivot.values, aspect="auto", origin="lower", cmap="viridis", interpolation="nearest")
plt.xlabel("i (time)")
plt.ylabel("channel")
plt.colorbar(label="amplitude")
plt.show()
```
`plot_dedispersed.py` :
```python
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("dedispersed.csv")

plt.plot(df["i"], df["amplitude"])
plt.xlabel("i (tie)")
plt.ylabel("Ampitude")
plt.title("Dedispersed signal")
plt.show()
```


## Project steps

- [x] Simulate a noisy Gaussian pulse, single channel
- [x] Extend to multiple channels with per-channel delay following the dispersion law (`1/f²`)
- [x] CSV export and visualization (2D / 3D waterfall)
- [X] Dedispersion: shift each channel back by its delay then sum, to recover a clean 1D time profile
- [X] Simulate a periodic pulse train (multiple pulsar rotations)
- [ ] FFT on the dedispersed profile to recover the pulsar's rotation period

## References

- [ATNF Pulsar Catalogue](https://www.atnf.csiro.au/research/pulsar/psrcat/) — parameters (including DM) of real pulsars
- Crab pulsar (B0531+21): DM ≈ 56.8 pc/cm³
