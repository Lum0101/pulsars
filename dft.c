
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

    double *x = calloc(N_UTIL, sizeof(double));
    double *cos_t = calloc(N_UTIL, sizeof(double));
    double *sin_t = calloc(N_UTIL, sizeof(double));

    if (!dedispersed || !magnitudes || !x || !cos_t || !sin_t){
        fprintf(stderr, "Eror: out of memory\n");
        goto cleanup;
    }

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

    // Preapare variable so they are not uselessly re-computed
    if (!x || !cos_t || !sin_t){
        fprintf(stderr, "Error: out of memory\n");
        goto cleanup;
    }

    // Windowed, mean-substracted signal (computed once)
    for (int n = 0; n < N_UTIL; n++){
        double w = 0.5 * (1.0 - cos(2.0 * PI * n / (N_UTIL - 1)));
        x[n] = (dedispersed[n] - mean) * w;
    }

    // Twidde factors : exp(-2*pi*i*m/N_UTIL) for m = 0 ... N_UTIL -1
    for (int m = 0; m < N_UTIL; m++){
        double angle = -2.0 * PI * m / N_UTIL;
        cos_t[m] = cos(angle);
        sin_t[m] = sin(angle);
    }

    // Compute the DFT with a Hann window
    for (int k = 0; k < NB_K; k++) {
        double re = 0.0, im = 0.0;
        int m = 0;

        for (int n = 0; n < N_UTIL; n++) {
            re += x[n] * cos_t[m];
            im += x[n] * sin_t[m];
            m += k;
            if (m >= N_UTIL) m -= N_UTIL;
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
    free(x);
    free(cos_t);
    free(sin_t);

    return ret;
}
