#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include "config.h"

#define NB_K (N_UTIL / 2 + 1)                                           // Only half the spectrum needs to be computed (real signal)

#ifndef PI
#define PI 3.14159265358979323846
#endif

// The radix-2 FFT divides the signal in two halves recursively,
// so N_UTIL has to be a power of 2. Checked at compile time:
// no runtime error path, hence no way to leak memory because of it.
_Static_assert(N_UTIL >= 2 && (N_UTIL & (N_UTIL - 1)) == 0,
               "N_UTIL must be a power of 2");

int main(void)
{
    int ret = 1;                                                        // Return variable
    FILE *f = NULL;                                                     // File variable for input signal
    FILE *magnitudes_file = NULL;                                       // File variable for output magnitudes

    float *dedispersed = calloc(N_UTIL, sizeof(float));                 // Input signal
    float *magnitudes  = calloc(NB_K, sizeof(float));                   // Output magnitudes

    double *w = calloc(N_UTIL, sizeof(double));                         // Hann weights
    double complex *x = calloc(N_UTIL, sizeof(double complex));         // Windowed signal, then spectrum (in place)
    double complex *twiddle = malloc((N_UTIL / 2) * sizeof(*twiddle));  // FFT twiddle factors

    // Other variables (declared here so that no goto jumps over a declaration)
    char header[100];
    int idx;
    int n_read = 0;
    int close_err;
    float amplitude;
    double hann_mean;
    double data_hann_product = 0.0;
    double hann_sum          = 0.0;

    if (!dedispersed || !magnitudes || !w || !x || !twiddle) {
        fprintf(stderr, "Error: out of memory\n");
        goto cleanup;
    }

    // Load dedispersed.csv file
    f = fopen("dedispersed.csv", "r");
    if (!f) {
        perror("dedispersed.csv");
        goto cleanup;
    }

    // Fetch the header
    if (!fgets(header, sizeof(header), f)) {
        fprintf(stderr, "Error: dedispersed.csv is empty\n");
        goto cleanup;
    }

    // Read the data
    while (fscanf(f, "%d,%f", &idx, &amplitude) == 2) {
        if (idx >= 0 && idx < N_UTIL) {
            dedispersed[idx] = amplitude;
            n_read++;
        }
    }

    // Close the file
    fclose(f);
    f = NULL;

    if (n_read == 0) {
        fprintf(stderr, "Error: no valid sample found in dedispersed.csv\n");
        goto cleanup;
    }
    if (n_read < N_UTIL)
        fprintf(stderr, "Warning: only %d/%d samples read, the rest is set to 0\n",
                n_read, N_UTIL);

    // Compute the Hann weights (once and for all)
    for (int n = 0; n < N_UTIL; n++)
        w[n] = 0.5 * (1.0 - cos(2 * PI * n / (N_UTIL - 1)));

    // Compute the Hann-weighted mean
    for (int n = 0; n < N_UTIL; n++) {
        data_hann_product += dedispersed[n] * w[n];
        hann_sum += w[n];
    }
    hann_mean = data_hann_product / hann_sum;

    // Compute the windowed and Hann-mean-subtracted signal (once and for all)
    for (int n = 0; n < N_UTIL; n++)
        x[n] = (dedispersed[n] - hann_mean) * w[n];

    // FFT ---------------------------------------------------------------------

    // Twiddle factors: exp(-2*pi*i*k/N), k = 0 ... N/2 - 1
    for (int k = 0; k < N_UTIL / 2; k++)
        twiddle[k] = cexp(-2.0 * I * PI * (double)k / (double)N_UTIL);

    // Bit-reversal
    // Because of the successive divisions in pairs/odds, the samples end up
    // in a different order. The new order is given by reversing the bits
    // of the index.
    for (int i = 1, j = 0; i < N_UTIL; i++) {
        int bit = N_UTIL >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j) {
            double complex t = x[i];
            x[i] = x[j];
            x[j] = t;
        }
    }

    // Butterflies: len = 2, 4, 8, ..., N_UTIL
    for (int len = 2; len <= N_UTIL; len <<= 1) {
        int half   = len / 2;
        int stride = N_UTIL / len;                                      // W_len^k = W_N^(k*stride)
        for (int i = 0; i < N_UTIL; i += len) {
            for (int k = 0; k < half; k++) {
                double complex u = x[i + k];
                double complex v = x[i + k + half] * twiddle[k * stride];
                x[i + k]        = u + v;
                x[i + k + half] = u - v;
            }
        }
    }

    // Magnitudes (only the first N/2 + 1 bins are meaningful for a real signal)
    for (int k = 0; k < NB_K; k++)
        magnitudes[k] = (float)cabs(x[k]);

    // ------------------------------------------------------------------------

    // Open the output file
    magnitudes_file = fopen("magnitudes.csv", "w");
    if (!magnitudes_file) {
        perror("magnitudes.csv");
        goto cleanup;
    }

    // Write the header
    fprintf(magnitudes_file, "freq,magnitudes\n");

    // Write the data
    for (int k = 0; k < NB_K; k++) {
        double freq = (double)k / ((double)N_UTIL * DT);
        fprintf(magnitudes_file, "%f,%f\n", freq, magnitudes[k]);
    }

    // Close file (fclose also reports delayed write errors, e.g. disk full)
    close_err = fclose(magnitudes_file);
    magnitudes_file = NULL;
    if (close_err != 0) {
        perror("magnitudes.csv");
        goto cleanup;
    }

    ret = 0;

cleanup:
    if (f) fclose(f);
    if (magnitudes_file) fclose(magnitudes_file);

    free(dedispersed);
    free(magnitudes);
    free(w);
    free(x);
    free(twiddle);

    return ret;
}