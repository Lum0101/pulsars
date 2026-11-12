
#ifndef CONFIG_H
#define CONFIG_H

#include <math.h>

// Simulation parameters
#define DT              1e-3f
#define N               4096
#define FIRST_PULSE_POS 5
#define PULSE_WIDTH     5
#define N_CHANNEL       64

#define F_MIN           1200.0f
#define F_MAX           1500.0f
#define F_REF           F_MAX

#define K_DM            4148.8f
#define DM              56.8f

#define PERIOD          0.033f
#define PERIOD_INDEX    ((int)(PERIOD / DT + 0.5f))

#define NOISE_AMPLITUDE 0.1f

#define DELAY_SPAN_S   (K_DM * DM * (1.0f / \
            (F_MIN * F_MIN) - 1.0f / (F_MAX * F_MAX)))

#define MAX_DELAY      ((int)ceilf(DELAY_SPAN_S / DT))

// Useful observation window (MAX_UTIL is exclusive)
#define MIN_UTIL       0
#define MAX_UTIL       (N - MAX_DELAY)
#define N_UTIL         (MAX_UTIL - MIN_UTIL)

#endif
