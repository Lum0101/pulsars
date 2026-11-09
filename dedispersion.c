#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"

int main(void)
{
    int ret = 1;
    FILE *data_file = NULL;
    FILE *decays_file = NULL;
    FILE *dedispersed_file = NULL;

    float **signal     = calloc(N_CHANNEL, sizeof(float *));
    float *decays      = calloc(N_CHANNEL, sizeof(float));
    float *dedispersed = calloc(N, sizeof(float));
    if (!signal || !decays || !dedispersed) goto cleanup;

    for (int c = 0; c < N_CHANNEL; c++) {
        signal[c] = calloc(N, sizeof(float));
        if (!signal[c]) goto cleanup;
    }

    char header[100];
    int channel, i;
    float amplitude, decay;

    // Read the signal
    data_file = fopen("signal.csv", "r");
    if (!data_file) goto cleanup;

    if (!fgets(header, sizeof(header), data_file)) goto cleanup;    // Skip the header
    while (fscanf(data_file, "%d,%d,%f", &channel, &i, &amplitude) == 3) {
        if (channel >= 0 && channel < N_CHANNEL && i >= 0 && i < N)
            signal[channel][i] = amplitude;
    }
    fclose(data_file);
    data_file = NULL;

    // Read the decays
    decays_file = fopen("decays.csv", "r");
    if (!decays_file) goto cleanup;

    if (!fgets(header, sizeof(header), decays_file)) goto cleanup;  // Skip the header
    while (fscanf(decays_file, "%d,%f", &channel, &decay) == 2) {
        if (channel >= 0 && channel < N_CHANNEL)
            decays[channel] = decay;
    }
    fclose(decays_file);
    decays_file = NULL;

    // Dedisperse: sum all channels, each shifted by its own delay
    for (int t = 0; t < N; t++) {
        float sum = 0.0f;
        for (int c = 0; c < N_CHANNEL; c++) {
            int idx = t - (int)(decays[c] / DT);
            if (idx >= 0 && idx < N) sum += signal[c][idx];
        }
        dedispersed[t] = sum;
    }

    // Write the result
    dedispersed_file = fopen("dedispersed.csv", "w");
    if (!dedispersed_file) goto cleanup;

    fprintf(dedispersed_file, "i,amplitude\n");
    for (int t = 0; t < N; t++) {
        fprintf(dedispersed_file, "%d,%f\n", t, dedispersed[t]);
    }

    ret = 0;

cleanup:
    if (data_file) fclose(data_file);
    if (decays_file) fclose(decays_file);
    if (dedispersed_file) fclose(dedispersed_file);
    if (signal) {
        for (int c = 0; c < N_CHANNEL; c++) free(signal[c]);        // free(NULL) is safe
        free(signal);
    }
    free(decays);
    free(dedispersed);
    return ret;
}