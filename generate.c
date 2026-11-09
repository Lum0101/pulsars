#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"

int main(void)
{
    int ret = 1;
    FILE *f = NULL;
    FILE *decays_file = NULL;

    float **signal = calloc(N_CHANNEL, sizeof(float *));   // N_CHANNEL pointers, one per channel
    float *decays  = calloc(N_CHANNEL, sizeof(float));     // Delay of each channel
    if (!signal || !decays) goto cleanup;

    // Allocate the N_CHANNEL signal arrays
    for (int i = 0; i < N_CHANNEL; i++) {
        signal[i] = calloc(N, sizeof(float));
        if (!signal[i]) goto cleanup;
    }

    // Delay of the lowest frequency: same for every channel, so computed once
    const int max_decay_index =
        (int)(K_DM * DM * (1.0f / (F_MIN * F_MIN) - 1.0f / (F_REF * F_REF)) / DT);

    for (int i = 0; i < N_CHANNEL; i++) {
        // Channel frequency (linear distribution over the span)
        float f_c = F_MIN + i * (F_MAX - F_MIN) / (N_CHANNEL - 1);

        // Dispersion delay relative to the reference frequency
        float delta_t = K_DM * DM * (1.0f / (f_c * f_c) - 1.0f / (F_REF * F_REF));
        decays[i] = delta_t;
        int decay_index = (int)(delta_t / DT);

        // Gaussian pulses, as long as the most delayed channel still fits in the array
        for (int p = 0; FIRST_PULSE_POS + max_decay_index + p * PERIOD_INDEX < N; p++) {
            int pulse_center = FIRST_PULSE_POS + decay_index + p * PERIOD_INDEX;
            for (int c = 0; c < N; c++) {
                float x = (c - pulse_center) / (float)PULSE_WIDTH;
                signal[i][c] += expf(-x * x);
            }
        }

        // Uniform noise in [-0.05, 0.05]
        for (int c = 0; c < N; c++) {
            signal[i][c] += 0.1f * ((float)rand() / RAND_MAX - 0.5f);
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

    // Write the delays
    decays_file = fopen("decays.csv", "w");
    if (!decays_file) goto cleanup;
    fprintf(decays_file, "channel,decay\n");
    for (int i = 0; i < N_CHANNEL; i++) {
        fprintf(decays_file, "%d,%f\n", i, decays[i]);
    }

    ret = 0;

cleanup:
    if (f) fclose(f);
    if (decays_file) fclose(decays_file);
    if (signal) {
        for (int i = 0; i < N_CHANNEL; i++) free(signal[i]);   // free(NULL) is safe
        free(signal);
    }
    free(decays);
    return ret;
}