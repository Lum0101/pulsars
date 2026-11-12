
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"

#define NB_K (N_UTIL / 2 + 1)
#define PI   3.14159265358979323846

int main(void)
{
    int ret = 1;
    FILE *f = NULL;
    FILE *magnitudes_file = NULL;

    float *dedispersed = calloc(N_UTIL, sizeof(float));
    float *magnitudes = calloc(NB_K, sizeof(float));

    if (!dedispersed || !magnitudes) goto cleanup;

    // Read the dedispersed signal
    f = fopen("dedispersed.csv", "r");
    if (!f) goto cleanup;

    char header[100];
    if (!fgets(header, sizeof(header), f))
        goto cleanup;

    int i;
    float amplitude;

    while (fscanf(f, "%d,%f", &i, &amplitude) == 2) {
        if (i >= 0 && i < N_UTIL)
            dedispersed[i] = amplitude;
    }

    fclose(f);
    f = NULL;

    // Compute the mean
    double mean = 0.0;

    for (int n = 0; n < N_UTIL; n++)
        mean += dedispersed[n];

    mean /= N_UTIL;

    // Compute the DFT with a Hann window
    for (int k = 0; k < NB_K; k++) {
        double re = 0.0;
        double im = 0.0;

        for (int n = 0; n < N_UTIL; n++) {
            float w = 0.5f * (
                1.0f - cosf(2.0f * PI * n / (N_UTIL - 1))
            );

            double angle = -2.0 * PI * k * n / N_UTIL;

            double x = (dedispersed[n] - mean) * w;

            re += x * cos(angle);
            im += x * sin(angle);
        }

        magnitudes[k] = (float)sqrt(re * re + im * im);
    }

    // Write the spectrum
    magnitudes_file = fopen("magnitudes.csv", "w");
    if (!magnitudes_file) goto cleanup;

    fprintf(magnitudes_file, "freq,magnitudes\n");

    for (int k = 0; k < NB_K; k++) {
        float freq = (float)k / (N_UTIL * DT);
        fprintf(magnitudes_file, "%f,%f\n",
                freq, magnitudes[k]);
    }

    ret = 0;

cleanup:
    if (f) fclose(f);
    if (magnitudes_file) fclose(magnitudes_file);

    free(dedispersed);
    free(magnitudes);

    return ret;
}
