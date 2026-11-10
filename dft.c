//---------------------------------------------------------------------------//
//                                                                           //
//  Provides a Discrete Fourier Transform implementation. This code uses     //
//  the naive implementation in O(n²), however a quasilinear O(n log(n))     //
//  algorithm exists: the FFT (Fast Fourier Transform).                      //
//                                                                           //
//---------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"

#define Nb_k (N / 2)                // Only half the spectrum: it is symmetrical for a real signal
#define PI   3.14159265358979323846

int main(void)
{
    int ret = 1;
    FILE *f = NULL;
    FILE *magnitudes_file = NULL;

    float *dedispersed = calloc(N, sizeof(float));
    float *magnitudes  = calloc(Nb_k, sizeof(float));
    if (!dedispersed || !magnitudes) goto cleanup;

    // Read the data
    f = fopen("dedispersed.csv", "r");
    if (!f) goto cleanup;

    char header[100];
    if (!fgets(header, sizeof(header), f)) goto cleanup;    // Skip the header

    int i;
    float amplitude;
    while (fscanf(f, "%d,%f", &i, &amplitude) == 2) {
        if (i >= 0 && i < N) dedispersed[i] = amplitude;    // Guard against out-of-range indices
    }
    fclose(f);
    f = NULL;
    
    // Compute the average of the signal
    float mean = 0;
    for (int k = 0; k < N; k++){
        mean += dedispersed[k];
    }
    mean = mean / N;

    // Compute |X[k]| from Re(X[k]) and Im(X[k])
    for (int k = 0; k < Nb_k; k++) {
        float re = 0.0f;
        float im = 0.0f;

        for (int n = 0; n < N; n++) {
            double angle = -2.0 * PI * k * n / N;
            re += (dedispersed[n] - mean) * cos(angle);
            im += (dedispersed[n] - mean) * sin(angle);
        }
        magnitudes[k] = sqrtf(re * re + im * im);
    }

    // Write the spectrum
    magnitudes_file = fopen("magnitudes.csv", "w");
    if (!magnitudes_file) goto cleanup;

    fprintf(magnitudes_file, "freq,magnitudes\n");
    for (int k = 0; k < Nb_k; k++) {
        float freq = (float)k / (N * DT);
        fprintf(magnitudes_file, "%f,%f\n", freq, magnitudes[k]);
    }

    ret = 0;

cleanup:
    if (f) fclose(f);
    if (magnitudes_file) fclose(magnitudes_file);
    free(dedispersed);              // free(NULL) is safe
    free(magnitudes);
    return ret;
}
