# Pulsar signal dispersion & dedispersion simulation

A C project simulating a pulsar's radio emission as it would be captured by a multi-channel radio telescope, accounting for **interstellar dispersion**, then implementing the **dedispersion** algorithm to recover the original signal and a **DFT** to recover the pulsar's rotation period.

## Physical background

A pulsar emits short, periodic radio pulses across a wide range of frequencies simultaneously (broadband emission). As the signal travels through the interstellar medium (ionized gas), it gets delayed differently depending on frequency: lower frequencies arrive later than higher ones, following a $1/f^2$ law. This is **dispersion**:

$$\Delta t(f) = K_{\mathrm{DM}} \, \mathrm{DM} \left( \frac{1}{f^2} - \frac{1}{f_{\mathrm{ref}}^2} \right)$$

with $K_{\mathrm{DM}} = 4148.8\ \mathrm{MHz^2\,pc^{-1}\,cm^3\,s}$, $\mathrm{DM}$ in $\mathrm{pc\,cm^{-3}}$ and $f$ in MHz.

A telescope observes this band split into several frequency channels. Without correction, the pulse appears smeared out in time across all channels (a parabolic "sweep"), buried in noise on each individual channel. **Dedispersion** consists of shifting each channel back by its expected delay (computed from the pulsar's dispersion measure, DM) and then summing them, making a sharp peak reappear.

## Project files

- [`generate.c`](generate.c) - Generates the simulated signal: a 2D array `signal[channel][i]` containing a periodic train of Gaussian pulses per channel, shifted in time according to the dispersion law, plus noise. Writes the signal to `signal.csv` and the per-channel delays to [`decays.csv`](decays.csv).

- [`dedispersion.c`](dedispersion.c) - Dedisperses the previously generated signal: for every time sample, sums the amplitude of all channels after shifting each one back by its delay (computed in [`generate.c`](generate.c) and stored in [`decays.csv`](decays.csv)). Writes the result to `dedispersed.csv`.

- [`dft.c`](dft.c) - Computes the Discrete Fourier Transform of the dedispersed profile with a naive direct approach ($O(N^2)$) to find the dominant frequency. Writes the spectrum to `magnitudes.csv`.

- [`config.h`](config.h) - Contains the project's constants.

- [`plot_generated.py`](plot/plot_generated.py) - Visualises the generated signal: amplitude for each channel with respect to time.

- [`plot_dedispersed.py`](plot/plot_dedispersed.py) - Visualises the dedispersed signal: amplitude with respect to time.

- [`plot_magnitudes.py`](plot/plot_magnitudes.py) - Visualises the Fourier spectrum.

## Build

```bash
gcc -Wall -Wextra -Werror -std=c17 -g -fsanitize=address,undefined -o file file.c -lm
./file
```

(`-lm` goes after the source file so that the linker resolves the math library correctly.)

## Physical parameters (constants defined in [`config.h`](config.h))

| Constant | Role | Value |
|---|---|---|
| `DT` | Time resolution: real duration represented by one sample (s) | $10^{-3}$ |
| `N` | Number of time samples per channel | 4096 |
| `N_CHANNEL` | Number of frequency channels | 64 |
| `F_MIN`, `F_MAX` | Observed frequency band (MHz) | 1200 – 1500 |
| `F_REF` | Reference frequency (zero delay) | `F_MAX` |
| `DM` | Pulsar's dispersion measure (pc/cm³) | 56.8 (Crab pulsar, [ATNF catalogue](https://www.atnf.csiro.au/research/pulsar/psrcat/)) |
| `K_DM` | Physical dispersion constant (MHz² pc⁻¹ cm³ s) | 4148.8 |
| `FIRST_PULSE_POS` | Pulse position (index) for the reference channel | 5 |
| `PULSE_WIDTH` | Gaussian pulse width (in samples) | 5 |
| `PERIOD` | Pulsar's rotation period (s) | 0.033 |
| `PERIOD_INDEX` | Period expressed in number of time samples (`PERIOD/DT`) | computed |

**`DT` / `N` trade-off**: `DT` must be small enough to properly resolve `PULSE_WIDTH` (the signal's time detail), but the total observable duration (`N × DT`) must exceed the maximum dispersion delay (`delta_t` between the lowest channel and `F_REF`), otherwise the peak falls outside the array for the most delayed channels. With the values above, `N × DT` ≈ 4.1 s, far above the maximum delay (≈ 59 ms).

The same choice sets the frequency resolution of the DFT: $\Delta f = 1/(N \, \mathrm{DT}) \approx 0.244$ Hz.

## Output format (`signal.csv`)

Long format, one row per data point:
```
channel,i,amplitude
0,0,0.000000
0,1,0.000012
...
```

## Output format (`dedispersed.csv`)

One row per time sample, summed over all channels:
```
i,amplitude
0,0.019300
1,0.039726
...
```

## Output format (`magnitudes.csv`)

```
freq,magnitudes
0.000000,69481.578125
0.244141,922.648926
...
```

## Visualization

The CSVs can be plotted in Python to observe the dispersion curve:

`plot_generated.py`:
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

`plot_dedispersed.py`:
```python
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("dedispersed.csv")

plt.plot(df["i"], df["amplitude"])
plt.xlabel("i (time)")
plt.ylabel("Amplitude")
plt.title("Dedispersed signal")
plt.show()
```

## Project steps

- [x] Simulate a noisy Gaussian pulse, single channel
- [x] Extend to multiple channels with per-channel delay following the dispersion law ($1/f^2$)
- [x] CSV export and visualization (2D / 3D waterfall)
- [x] Dedispersion: shift each channel back by its delay then sum, to recover a clean 1D time profile
- [x] Simulate a periodic pulse train (multiple pulsar rotations)
- [x] DFT on the dedispersed profile to recover the pulsar's rotation period

## TODO

- [ ] **DM scan**: dedisperse for a range of trial DM values, plot the signal-to-noise ratio of the dedispersed profile against DM, and check that the maximum falls at the true DM (56.8 pc/cm³). This is the actual principle of a pulsar search.
- [ ] **Quantify the detection**: compute the signal-to-noise ratio of the dedispersed pulse and of the DFT peak (peak height compared to the surrounding noise floor).
- [ ] **FFT**: replace the naive $O(N^2)$ DFT with a radix-2 Cooley-Tukey FFT ($O(N \log N)$) and compare computation times.
- [ ] **Real data**: apply the dedispersion to a public filterbank file of a real pulsar, to deal with non-Gaussian noise and interference.

## References

- [ATNF Pulsar Catalogue](https://www.atnf.csiro.au/research/pulsar/psrcat/) — parameters (including DM) of real pulsars
- Crab pulsar (B0531+21): DM ≈ 56.8 pc/cm³
