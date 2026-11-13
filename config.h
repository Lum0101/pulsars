
#ifndef CONFIG_H
#define CONFIG_H

#include <math.h>

// Simulation parameters
#define DT              1e-3f
#define N               4096
#define FIRST_PULSE_POS 5
#define PULSE_SIGMA 3.5f    // Gaussian standard deviation (samples), FWHM ≈ 2.355 * sigma
#define N_CHANNEL       64

#define F_MIN           1200.0f
#define F_MAX           1500.0f
#define F_REF           F_MAX

#define K_DM            4148.8f
#define DM              56.8f

#define SEED            42

#define PERIOD          0.033f
#define PERIOD_INDEX    ((int)(PERIOD / DT + 0.5f))

#define NOISE_AMPLITUDE 0.1f

#define DELAY_SPAN_S   (K_DM * DM * (1.0f / \
            (F_MIN * F_MIN) - 1.0f / (F_MAX * F_MAX)))
// Total dispersion delay across the band, in samples.
// MAX_DELAY is rounded UP (ceilf), while each channel shift in generate.c and
// dedispersion.c is TRUNCATED ((int) cast). Every shift is therefore <= MAX_DELAY,
// and since t < N - MAX_DELAY, the read index t + shift never exceeds N - 1.
// This holds even if DM or the band change. Cost: at most one usable sample.
#define MAX_DELAY      ((int)ceilf(DELAY_SPAN_S / DT))

// Useful observation window (MAX_UTIL is exclusive)
#define MIN_UTIL       0
#define MAX_UTIL       (N - MAX_DELAY)
#define N_UTIL         (MAX_UTIL - MIN_UTIL)

#endif
