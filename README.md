# Pulsar signal dispersion & dedispersion simulation

A C project simulating a pulsar's radio emission as it would be captured by a multi-channel radio telescope, accounting for **interstellar dispersion**, then implementing the **dedispersion** algorithm to recover the original signal and a **DFT** to recover the pulsar's rotation period.

## Physical background

A pulsar emits short, periodic radio pulses across a wide range of frequencies simultaneously (broadband emission). As the signal travels through the interstellar medium (ionized gas), it gets delayed differently depending on frequency: lower frequencies arrive later than higher ones, following a $1/f^2$ law. This is **dispersion**:

$$\Delta t(f) = K_{\mathrm{DM}}  \mathrm{DM} \left( \frac{1}{f^2} - \frac{1}{f_{\mathrm{ref}}^2} \right)$$

with $K_{\mathrm{DM}} = 4148.8\ \mathrm{MHz^2\ pc^{-1}cm^3\ s}$, $\mathrm{DM}$ in $\mathrm{pc\ cm^{-3}}$ and $f$ in MHz.

A telescope observes this band split into several frequency channels. Without correction, the pulse appears smeared out in time across all channels (a parabolic "sweep"), buried in noise on each individual channel. **Dedispersion** consists of shifting each channel back by its expected delay (computed from the pulsar's dispersion measure, DM) and then summing them, making a sharp peak reappear.

The largest delay is the one between the lowest and the highest frequency of the band:

$$\Delta t_{\max} = K_{\mathrm{DM}}  \mathrm{DM} \left( \frac{1}{f_{\min}^2} - \frac{1}{f_{\max}^2} \right) \approx 59\ \mathrm{ms}$$

## Project files

- [`generate.c`](generate.c) - Generates the simulated signal: a 2D array `signal[channel][i]` containing a periodic train of Gaussian pulses per channel, shifted in time according to the dispersion law, plus a uniform random background noise (drawn independently for each sample and each channel, amplitude set by `NOISE_AMPLITUDE`). Writes the signal to `signal.csv`.

- [`dedispersion.c`](dedispersion.c) - Dedisperses the previously generated signal: for every time sample, sums the amplitude of all channels after shifting each one back by its delay. The delays are not stored anywhere: they are recomputed on the fly from `DM`, `K_DM` and the channel frequencies (same formula and same integer truncation as in [`generate.c`](generate.c)). A delayed channel is read at index `t + delay`, so the last samples would lack the contribution of the most delayed channels and the amplitude would drop there (edge effect). To avoid it, the output is **truncated to `N_UTIL = N - MAX_DELAY` samples**, the only ones for which every channel has data. Each channel is stored on the full `N` samples; only the dedispersed profile has `N_UTIL` samples. Writes the result to `dedispersed.csv`.

- [`dft.c`](dft.c) - Computes the Discrete Fourier Transform of the dedispersed profile (`N_UTIL` samples) with a naive direct approach ($O(N^2)$) to find the dominant frequency. The mean is subtracted first, then a **Hann window** is applied to reduce spectral leakage. Writes the spectrum to `magnitudes.csv`.

- [`config.h`](config.h) - Contains the project's constants, including the derived `MAX_DELAY` and `N_UTIL`.

- [`plot_generated.py`](plot/plot_generated.py) - Visualises the generated signal: amplitude for each channel with respect to time.

- [`plot_dedispersed.py`](plot/plot_dedispersed.py) - Visualises the dedispersed signal: amplitude with respect to time.

- [`plot_magnitudes.py`](plot/plot_magnitudes.py) - Visualises the Fourier spectrum (log scale on the magnitude axis) and saves it in `result/dft_spectrum.png`.

## Build

```bash
gcc -Wall -Wextra -Werror -std=c17 -g -fsanitize=address,undefined -o file file.c -lm
./file
```

(`-lm` goes after the source file so that the linker resolves the math library correctly.)

## Run order

Each step reads the output of the previous one, so run the programs from the same folder, in this order:

1. `generate` → writes `signal.csv`
2. `dedispersion` → reads `signal.csv`, writes `dedispersed.csv`
3. `dft` → reads `dedispersed.csv`, writes `magnitudes.csv`
4. the plots, each one as soon as its CSV exists

All programs share [`config.h`](config.h): recompile all of them after changing a constant, otherwise they may no longer agree (for instance on `N` or `N_UTIL`).

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
| `PERIOD_INDEX` | Period expressed in number of time samples (`PERIOD/DT`) | computed (33) |
| `NOISE_AMPLITUDE` | Peak-to-peak amplitude of the uniform background noise added to each sample (pulse amplitude = 1) | 0.1 |
| `MAX_DELAY` | Maximum dispersion delay across the band, in samples (rounded up) | computed (59) |
| `N_UTIL` | Number of usable samples after dedispersion (`N - MAX_DELAY`) | computed (4037) |

**`DT` / `N` trade-off**: `DT` must be small enough to properly resolve `PULSE_WIDTH` (the signal's time detail), but the total observable duration (`N × DT`) must exceed the maximum dispersion delay (`delta_t` between the lowest channel and `F_REF`), otherwise the peak falls outside the array for the most delayed channels. With the values above, `N × DT` ≈ 4.1 s, far above the maximum delay (≈ 59 ms). After truncation, the usable duration is `N_UTIL × DT` ≈ 4.04 s.

The usable duration sets the frequency resolution of the DFT: $\Delta f = 1/(N_{\mathrm{util}} \ \mathrm{DT}) \approx 0.248$ Hz.

## Edge effect and spectral leakage

Two effects can pollute the spectrum, and both are handled:

- **Edge effect**: in the last `MAX_DELAY` samples, the most delayed channels have no data left, so the dedispersed amplitude collapses. This discontinuity spreads energy over the whole spectrum. The profile is therefore truncated to `N_UTIL` samples, and the DFT must use `N_UTIL` (not `N`) everywhere: array sizes, mean, phase angle and frequency axis. Using `N` would silently zero-pad the series and reintroduce a step at its end.
- **Spectral leakage**: the series does not contain an integer number of periods ($N_{\mathrm{util}}/P \approx 122.3$), and a rectangular window creates side lobes around each peak. A Hann window, applied **after** subtracting the mean, brings the signal smoothly to zero at both ends:

$$w[n] = \frac{1}{2}\left(1 - \cos\frac{2\pi n}{N_{\mathrm{util}} - 1}\right), \qquad x'[n] = \left(x[n] - \bar{x}\right) w[n]$$

The price is a main peak about twice as wide and an amplitude divided by two on average. A small residual peak at 0 Hz remains, because the windowed sum no longer cancels exactly.

## Results

- The dedispersed pulses reach an amplitude of about 64 = `N_CHANNEL`: all channels add up coherently, each one having an amplitude of 1.
- The spectrum shows the fundamental at about 30 Hz, i.e. a period of about 33 ms, followed by its harmonics (60, 90, 120 and 150 Hz) above the noise floor.

## Output format (`signal.csv`)

Long format, one row per data point:
```
channel,i,amplitude
0,0,0.000000
0,1,0.000012
...
```

## Output format (`dedispersed.csv`)

One row per usable time sample (`N_UTIL` rows), summed over all channels:
```
i,amplitude
0,0.019300
1,0.039726
...
```

## Output format (`magnitudes.csv`)

One row per frequency bin, from 0 up to half the sampling frequency (`N_UTIL / 2` rows):
```
freq,magnitudes
0.000000,...
0.247708,...
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
- [x] Remove the intermediate delays file: the delays are recomputed from the DM in `dedispersion.c`
- [x] Spectrum plotted in log scale and saved in `result/`
- [x] Truncate the profile to `N - MAX_DELAY` samples (`N_UTIL`) before the DFT, and apply a Hann window
- [x] Compute `MAX_DELAY` and `N_UTIL` automatically from the physical parameters in `config.h`

## TODO

- [ ] **DM scan**: dedisperse for a range of trial DM values, plot the signal-to-noise ratio of the dedispersed profile against DM, and check that the maximum falls at the true DM (56.8 pc/cm³). This is the actual principle of a pulsar search.
- [ ] **Quantify the detection**: compute the signal-to-noise ratio of the dedispersed pulse and of the DFT peak (peak height compared to the surrounding noise floor).
- [ ] **FFT**: replace the naive $O(N^2)$ DFT with a radix-2 Cooley-Tukey FFT ($O(N \log N)$) and compare computation times.
- [ ] **Real data**: apply the dedispersion to a public filterbank file of a real pulsar, to deal with non-Gaussian noise and interference.
- [ ] *(optional)* **More realistic generation**: evaluate the periodic pulse profile at $t - \Delta t_c$ in each channel, so that every channel is filled over the whole observation (no empty zone at the edges of the dynamic spectrum). The truncation in `dedispersion.c` stays necessary.
- [ ] *(optional)* **Weighted mean**: subtract the Hann-weighted mean instead of the plain mean to cancel the residual peak at 0 Hz.

## References

- [ATNF Pulsar Catalogue](https://www.atnf.csiro.au/research/pulsar/psrcat/) — parameters (including DM) of real pulsars
- Crab pulsar (B0531+21): DM ≈ 56.8 pc/cm³