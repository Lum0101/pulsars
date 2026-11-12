
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"

int main(void)
{
    int ret = 1;
    FILE *f = NULL;

    float **signal = calloc(N_CHANNEL, sizeof(float *));
    if (!signal) goto cleanup;

    for (int i = 0; i < N_CHANNEL; i++) {
        signal[i] = calloc(N, sizeof(float));
        if (!signal[i]) goto cleanup;
    }

    for (int i = 0; i < N_CHANNEL; i++) {
        // Channel frequency
        float f_c = F_MIN + i * (F_MAX - F_MIN) / (N_CHANNEL - 1);

        // Dispersion delay relative to reference frequency
        float delta_t = K_DM * DM *
            (1.0f / (f_c * f_c) - 1.0f / (F_REF * F_REF));

        int decay_index = (int)(delta_t / DT);

        // Generate pulses emitted before the observation
        int pre_margin = (int)ceilf((MAX_DELAY + FIRST_PULSE_POS) / (float)PERIOD_INDEX);

        for (int p = -pre_margin;
             FIRST_PULSE_POS + p * PERIOD_INDEX < N;
             p++) {

            int pulse_center = FIRST_PULSE_POS
                             + decay_index
                             + p * PERIOD_INDEX;

            for (int c = 0; c < N; c++) {
                float x = (c - pulse_center) / (float)PULSE_WIDTH;
                signal[i][c] += expf(-x * x);
            }
        }

        // Add uniform noise in [-0.05, 0.05]
        for (int c = 0; c < N; c++) {
            signal[i][c] += NOISE_AMPLITUDE *
                ((float)rand() / RAND_MAX - 0.5f);
        }
    }

    // Write the signal
    f = fopen("signal.csv", "w");
    if (!f) goto cleanup;

    fprintf(f, "channel,i,amplitude\n");

    for (int c = 0; c < N_CHANNEL; c++) {
        for (int i = 0; i < N; i++) {
            fprintf(f, "%d,%d,%f\n", c, i, signal[c][i]);
        }
    }

    ret = 0;

cleanup:
    if (f) fclose(f);

    if (signal) {
        for (int i = 0; i < N_CHANNEL; i++)
            free(signal[i]);
        free(signal);
    }

    return ret;
}
