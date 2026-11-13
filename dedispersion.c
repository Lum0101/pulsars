
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"

int main(void)
{
    int ret = 1;
    FILE *data_file = NULL;
    FILE *dedispersed_file = NULL;

    float **signal = calloc(N_CHANNEL, sizeof(float *));
    float *dedispersed = calloc(N_UTIL, sizeof(float));

    if (!signal || !dedispersed){
        fprintf(stderr, "Error: out of memory\n");
        goto cleanup;
    }

    for (int c = 0; c < N_CHANNEL; c++) {
        signal[c] = calloc(N, sizeof(float));
        if (!signal[c]){
            fprintf(stderr, "Error: out of memory\n");
            goto cleanup;
        }
    }

    // Read the signal
    data_file = fopen("signal.csv", "r");
    if (!data_file){
        perror("signal.csv");
        goto cleanup;
    }

    char header[100];
    int channel, i;
    float amplitude;

    if (!fgets(header, sizeof(header), data_file)){
        fprintf(stderr, "Error: signal.csv is empty\n");
        goto cleanup;
    }

    while (fscanf(data_file, "%d,%d,%f",
                  &channel, &i, &amplitude) == 3) {

        if (channel >= 0 && channel < N_CHANNEL &&
            i >= 0 && i < N) {
            signal[channel][i] = amplitude;
        }
    }

    fclose(data_file);
    data_file = NULL;

    int shift[N_CHANNEL];
    for (int c = 0; c < N_CHANNEL; c++){
        float f_c = F_MIN + c * (F_MAX - F_MIN) / (N_CHANNEL - 1);
        float delta_t_c = K_DM * DM * (1.0f / (f_c * f_c) - 1.0f / (F_REF * F_REF));
        shift[c] = (int)(delta_t_c / DT);
    }

    // Dedisperse
    for (int t = MIN_UTIL; t < MAX_UTIL; t++) {
        float sum = 0.0f;
        for (int c = 0; c < N_CHANNEL; c++){
            int idx = t + shift[c];
            if (idx >= 0 && idx < N)
                sum += signal[c][idx];
        }

        dedispersed[t - MIN_UTIL] = sum;
    }

    // Write the dedispersed signal
    dedispersed_file = fopen("dedispersed.csv", "w");
    if (!dedispersed_file){
        fprintf(stderr, "Error: dedispersed.csv is empty");
    }

    fprintf(dedispersed_file, "i,amplitude\n");

    for (int t = MIN_UTIL; t < MAX_UTIL; t++) {
        fprintf(dedispersed_file, "%d,%f\n",
                t - MIN_UTIL, dedispersed[t - MIN_UTIL]);
    }

    ret = 0;

cleanup:
    if (data_file) fclose(data_file);
    if (dedispersed_file) fclose(dedispersed_file);

    if (signal) {
        for (int c = 0; c < N_CHANNEL; c++)
            free(signal[c]);
        free(signal);
    }

    free(dedispersed);

    return ret;
}
