
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"

#define NB_K (N_UTIL / 2 + 1)                                           // Only half the spectrum needs to be computed
#define PI   3.14159265358979323846

int main(void)
{
    int ret = 1;                                                        // Return variable
    FILE *f = NULL;                                                     // File variable for input signal
    FILE *magnitudes_file = NULL;                                       // File variable for output magnitudes

    float *dedispersed = calloc(N_UTIL, sizeof(float));                 // Input signal             
    float *magnitudes  = calloc(NB_K, sizeof(float));                   // Output magnitudes

    double *x     = calloc(N_UTIL, sizeof(double));                     // Signal after being treated (windowed, substracted the hann-mean)
    double *cos_t = calloc(N_UTIL, sizeof(double));                     // DFT real factor
    double *sin_t = calloc(N_UTIL, sizeof(double));                     // DFT imaginary factor
    double *w     = calloc(N_UTIL, sizeof(double));                     // Hann-weights
   
    if (!dedispersed || !magnitudes || !x || !cos_t || !sin_t || !w){
        fprintf(stderr, "Eror: out of memory\n");
        goto cleanup;
    }

    // Load dedispersed.csv file
    f = fopen("dedispersed.csv", "r");
    if (!f){
       perror("dedispersed.csv");
       goto cleanup;
    };
    
    // Fetch the header
    char header[100];
    if (!fgets(header, sizeof(header), f)){
        fprintf(stderr, "Error: dedispersed.csv is empty");
        goto cleanup;
    }
    
    // Read the data
    int i;
    float amplitude;
    while (fscanf(f, "%d,%f", &i, &amplitude) == 2) {
        if (i >= 0 && i < N_UTIL)
            dedispersed[i] = amplitude;
    }
    
    // Close the file
    fclose(f);
    f = NULL;

    // Compute the Hann weights (once and for all)
    for (int n=0; n < N_UTIL; n++){
        w[n] = 0.5 * (1.0 - cos(2 * PI * n / (N_UTIL - 1)));
    }

    // Compute the Hann-weighted mean
    double hann_mean         = 0.0;
    double data_hann_product = 0.0;
    double hann_sum          = 0.0;

    for (int n = 0; n < N_UTIL; n++){
        data_hann_product += dedispersed[n] * w[n];
        hann_sum += w[n];
    }
    hann_mean = data_hann_product / hann_sum;

    // Compute the windowed and Hann-mean-substracted signal (once and for all)
    for (int n = 0; n < N_UTIL; n++){
        x[n] = (dedispersed[n] - hann_mean) * w[n];
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

    // Open the outpur file
    magnitudes_file = fopen("magnitudes.csv", "w");
    if (!magnitudes_file){
        perror("magnitudes.csv");
        goto cleanup;
    }

    // Write the header
    fprintf(magnitudes_file, "freq,magnitudes\n");
    
    // Write the data
    for (int k = 0; k < NB_K; k++) {
        float freq = (float)k / (N_UTIL * DT);
        fprintf(magnitudes_file, "%f,%f\n",
                freq, magnitudes[k]);
    }
    
    // Close file
    fclose(magnitudes_file);
    magnitudes_file = NULL;

    ret = 0;

cleanup:
    if (f) fclose(f);
    if (magnitudes_file) fclose(magnitudes_file);

    free(dedispersed);
    free(magnitudes);
    free(x);
    free(cos_t);
    free(sin_t);
    free(w);

    return ret;
}
