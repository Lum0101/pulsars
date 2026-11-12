# Pulsar Signal Dispersion & Dedispersion Simulation

A C project simulating the dispersion of a pulsar's radio emission as it travels through the interstellar medium and is observed by a multi-channel radio telescope.

The project implements:
- A multi-channel pulsar signal generator with interstellar dispersion and noise.
- A dedispersion algorithm to recover the original pulse profile.
- A Discrete Fourier Transform (DFT) to estimate the pulsar's rotation period.
- Python visualisation tools to inspect the generated signal, the dedispersed profile and its frequency spectrum.

## 1. Physical background

A pulsar emits short, periodic radio pulses over a wide range of frequencies simultaneously (broadband emission).

As the signal travels through the ionised interstellar medium, lower frequencies are delayed more than higher frequencies. This phenomenon is called **dispersion**.

The time delay relative to a reference frequency is:

$$
\Delta t(f) = K_{\mathrm{DM}}\,\mathrm{DM}
\left(\frac{1}{f^2}-\frac{1}{f_{\mathrm{ref}}^2}\right)
$$

where:
- $K_{\mathrm{DM}} = 4148.8\ \mathrm{MHz^2\,pc^{-1}\,cm^3\,s}$
- DM is the dispersion measure, in $\mathrm{pc\,cm^{-3}}$
- $f$ and $f_{\mathrm{ref}}$ are frequencies in MHz.

A radio telescope observes the frequency band through several channels. Without correction, the pulse appears progressively delayed at lower frequencies.

**Dedispersion** consists of shifting each channel back by its expected delay and summing the channels. This restores the original pulse profile.

The maximum delay across the observed frequency band is:

$$
\Delta t_{\max} =
K_{\mathrm{DM}}\,\mathrm{DM}
\left(\frac{1}{f_{\min}^2}-\frac{1}{f_{\max}^2}\right)
$$

For the default parameters, this delay is approximately 59 ms.

## 2. Project structure

```text
pulsars/
│
├── config.h
│
├── generate.c
├── dedispersion.c
├── dft.c
│
├── plot/
│   ├── plot_generated.py
│   ├── plot_dedispersed.py
│   └── plot_magnitudes.py
│
├── result/
│
├── signal.csv
├── dedispersed.csv
├── magnitudes.csv
│
└── README.md
```

### C programs

| File | Description |
|---|---|
| `generate.c` | Generates the dispersed multi-channel pulsar signal and writes `signal.csv`. |
| `dedispersion.c` | Applies dedispersion and writes the recovered signal to `dedispersed.csv`. |
| `dft.c` | Computes the DFT of the dedispersed signal and writes `magnitudes.csv`. |
| `config.h` | Defines the physical and numerical parameters shared by all C programs. |

### Python visualisation scripts

| Script | Description |
|---|---|
| `plot/plot_generated.py` | Displays the generated multi-channel signal as a 2D time-frequency image. |
| `plot/plot_dedispersed.py` | Displays the recovered dedispersed pulse profile as a function of time. |
| `plot/plot_magnitudes.py` | Displays the Fourier magnitude spectrum on a logarithmic magnitude scale and saves the figure in `result/`. |

## 3. Physical and numerical parameters

All simulation parameters are defined in `config.h`.

| Parameter | Description | Default value |
|---|---|---|
| `DT` | Time resolution per sample (s) | $10^{-3}$ |
| `N` | Number of time samples per channel | 4096 |
| `N_CHANNEL` | Number of frequency channels | 64 |
| `F_MIN` | Lowest observed frequency (MHz) | 1200 |
| `F_MAX` | Highest observed frequency (MHz) | 1500 |
| `F_REF` | Reference frequency (MHz) | `F_MAX` |
| `DM` | Dispersion measure ($\mathrm{pc\,cm^{-3}}$) | 56.8 |
| `K_DM` | Dispersion constant | 4148.8 |
| `FIRST_PULSE_POS` | Reference-channel pulse position (sample index) | 5 |
| `PULSE_WIDTH` | Gaussian pulse width (samples) | 5 |
| `PERIOD` | Pulsar rotation period (s) | 0.033 |
| `PERIOD_INDEX` | Rotation period in samples | Computed (33) |
| `NOISE_AMPLITUDE` | Peak-to-peak background noise amplitude | 0.1 |
| `MAX_DELAY` | Maximum dispersion delay (samples) | Computed (59) |
| `N_UTIL` | Number of usable samples after dedispersion | Computed (4037) |

The default dispersion measure corresponds approximately to that of the Crab pulsar (B0531+21).

### Time resolution and observation duration

The time resolution must be sufficiently small to resolve the pulse width.

The total observation duration is:

$$
T_{\mathrm{obs}} = N \times DT
$$

For the default parameters:

$$
T_{\mathrm{obs}} \approx 4.096\ \mathrm{s}
$$

This is considerably longer than the maximum dispersion delay of approximately 59 ms.

After dedispersion, the usable observation duration is:

$$
T_{\mathrm{util}} = N_{\mathrm{UTIL}} \times DT
\approx 4.037\ \mathrm{s}
$$

The corresponding DFT frequency resolution is:

$$
\Delta f = \frac{1}{N_{\mathrm{UTIL}}DT}
\approx 0.248\ \mathrm{Hz}
$$

## 4. Compilation

The project requires a C compiler supporting the C17 standard and the mathematical library.

For example, using GCC:

```bash
gcc -Wall -Wextra -Werror -std=c17 -g \
    -fsanitize=address,undefined \
    -o generate generate.c -lm

gcc -Wall -Wextra -Werror -std=c17 -g \
    -fsanitize=address,undefined \
    -o dedispersion dedispersion.c -lm

gcc -Wall -Wextra -Werror -std=c17 -g \
    -fsanitize=address,undefined \
    -o dft dft.c -lm
```

The `-lm` option links the mathematical library and must appear after the source file.

All three programs include `config.h`.

**Important:** if a parameter is modified in `config.h`, all three programs must be recompiled to ensure that they use consistent values.

## 5. Execution

Run all commands from the project root directory.

The programs must be executed in the following order because each step uses the output of the previous one.

### Step 1: Generate the dispersed signal

```bash
./generate
```

This program:
- Generates a periodic train of Gaussian pulses.
- Distributes the signal across multiple frequency channels.
- Applies a frequency-dependent delay according to the dispersion law.
- Adds independent uniform background noise.
- Writes the resulting dynamic spectrum to `signal.csv`.

### Step 2: Dedisperse the signal

```bash
./dedispersion
```

This program:
- Reads `signal.csv`.
- Recomputes the channel delays using the physical parameters.
- Shifts each channel back to its expected arrival time.
- Sums the channels to recover the pulse profile.
- Writes the result to `dedispersed.csv`.

The delays are recomputed directly from `DM`, `K_DM` and the channel frequencies. No intermediate delay file is required.

### Step 3: Compute the Fourier spectrum

```bash
./dft
```

This program:
- Reads `dedispersed.csv`.
- Removes the mean value of the signal.
- Applies a Hann window.
- Computes the Discrete Fourier Transform.
- Writes the frequency spectrum to `magnitudes.csv`.

### Step 4: Visualise the results

Install the Python dependencies:

```bash
python -m pip install numpy pandas matplotlib
```

Run the visualisation scripts from the project root:

```bash
python plot/plot_generated.py
python plot/plot_dedispersed.py
python plot/plot_magnitudes.py
```

The scripts use the CSV files generated by the C programs.

The Fourier spectrum visualisation saves its figure in:

```text
result/dft_spectrum.png
```

## 6. Edge effects and spectral leakage

Two numerical effects must be considered when computing the Fourier spectrum.

### Edge effects

During dedispersion, each channel is shifted by its corresponding delay.

For a time sample $t$, the algorithm reads the original channel at index:

$$
t + \mathrm{delay}
$$

Consequently, the last samples of the original observation do not contain valid contributions from every channel.

Keeping these samples would introduce an artificial decrease in amplitude at the end of the dedispersed profile.

To avoid this problem, the output is truncated to:

$$
N_{\mathrm{UTIL}} = N - \mathrm{MAX\_DELAY}
$$

Only samples for which all channels contain valid data are retained.

The DFT must therefore consistently use `N_UTIL` for:
- The input signal length.
- The mean calculation.
- The phase calculation.
- The frequency axis.

Using `N` instead would introduce an artificial discontinuity in the signal.

### Spectral leakage

The observation does not contain an integer number of pulsar rotations:

$$
\frac{N_{\mathrm{UTIL}}}{P_{\mathrm{index}}}
\approx 122.3
$$

A rectangular observation window would therefore produce spectral leakage, spreading energy into neighbouring frequency bins.

To reduce this effect, a Hann window is applied after subtracting the mean:

$$
w[n] =
\frac{1}{2}
\left(1-\cos\left(\frac{2\pi n}{N_{\mathrm{UTIL}}-1}\right)\right)
$$

The windowed signal is:

$$
x'[n] = (x[n]-\bar{x})w[n]
$$

This reduces spectral leakage but also broadens the main spectral peak and reduces its amplitude.

A small residual component at 0 Hz may remain because the windowed signal does not necessarily sum to exactly zero.

## 7. Output files

### `signal.csv`

Contains the generated multi-channel signal in long format, with one row per channel and time sample.

```csv
channel,i,amplitude
0,0,0.000000
0,1,0.000012
...
```

### `dedispersed.csv`

Contains the dedispersed signal, with one row per usable time sample.

```csv
i,amplitude
0,0.019300
1,0.039726
...
```

The file contains `N_UTIL` samples.

### `magnitudes.csv`

Contains the Fourier magnitude spectrum.

```csv
freq,magnitudes
0.000000,...
0.247708,...
...
```

The frequency values are expressed in Hz.

## 8. Expected results

The simulation should produce the following results:

- **Dispersed signal:** the pulses appear progressively delayed at lower frequencies, producing a characteristic dispersion sweep.
- **Dedispersed signal:** the individual channel contributions add coherently, producing a much sharper pulse profile.
- **Amplitude:** the dedispersed pulse can reach an amplitude close to `N_CHANNEL` (64), since each channel contributes a pulse with an amplitude of 1.
- **Fourier spectrum:** the fundamental frequency should appear around 30 Hz, corresponding to a pulsar rotation period of approximately 33 ms.
- **Harmonics:** additional peaks should appear around 60, 90, 120 and 150 Hz.

These values correspond to the default simulation parameters.

## 9. Current implementation status

The project currently implements:

- [x] Gaussian pulse generation.
- [x] Multi-channel signal generation.
- [x] Frequency-dependent dispersion following the $1/f^2$ law.
- [x] Background noise generation.
- [x] CSV export.
- [x] Signal visualisation.
- [x] Dedispersion by shifting and summing frequency channels.
- [x] Periodic pulse train simulation.
- [x] DFT computation on the dedispersed profile.
- [x] Mean subtraction and Hann windowing.
- [x] Automatic computation of `MAX_DELAY` and `N_UTIL`.
- [x] Fourier spectrum visualisation.
- [x] Evaluate the periodic pulse profile for time ahead of mesure, avoiding empty regions at the edge of dynamic spectrum. 

## 10. Future improvements

Potential developments include:

- [ ] **Dispersion measure search:** dedisperse the signal over a range of trial DM values and plot the signal-to-noise ratio as a function of DM. The maximum should occur near the true DM.
- [ ] **Detection significance:** quantify the signal-to-noise ratio of the recovered pulse and the fundamental Fourier peak.
- [ ] **Fast Fourier Transform:** replace the direct $O(N^2)$ DFT with an $O(N\log N)$ FFT algorithm and compare execution times.
- [ ] **Real pulsar data:** apply the dedispersion algorithm to public filterbank data from an observed pulsar.
- [ ] **Weighted mean subtraction:** investigate subtracting the Hann-weighted mean to reduce the residual DC component.

## 11. References

- [ATNF Pulsar Catalogue](https://www.atnf.csiro.au/research/pulsar/psrcat/) — pulsar parameters, including dispersion measures.
- Crab pulsar (B0531+21) — reference dispersion measure of approximately $56.8\ \mathrm{pc\,cm^{-3}}$.

---

**Project objective:** simulate the propagation of a pulsar radio signal through the interstellar medium, recover its original pulse profile using dedispersion, and estimate its rotation period through Fourier analysis.
