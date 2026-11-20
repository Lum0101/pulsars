#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "config.h"

#define M (N_UTIL / 2)
#define NB_K (M + 1)
#define N_STAGES 5
#define N_CAND 8
#define SEP 3 // two bins closer than SEP bins are the same one
#define W  500
#define N_FOLD 10
#define F_SEARCH_MIN 1.0
#define T_OBS ((double)N_UTIL * DT)

// Struct for Candidates 
typedef struct {int k; int h; double score; } Cand;

static const int STAGES[N_STAGES] = {1, 2, 4, 8, 16};

static int cmp_double(const void *a, const void *b){
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}

static int cmp_cand(const void *a, const void *b){
    double x = ((const Cand *)a) -> score;
    double y = ((const Cand *)b) -> score;
    return (x<y)-(x>y);
}

    

int main(void)
{
    int     ret = 1;
    int     n_read = 0;
    float   freq;
    float   magnitude;

    char header[100];
    
    FILE    *magnitudes_file = NULL;
    
    float   *magnitudes = calloc(NB_K, sizeof(float));
    double  *pw         = calloc(NB_K, sizeof(double));      // Signal power
    double  *score      = calloc(NB_K, sizeof(double));

    if (!magnitudes || !pw){
        fprintf(stderr, "Error: out of memory\n");
        goto cleanup;
    }
    
    // Open magnitudes file
    magnitudes_file = fopen("magnitudes.csv", "r");
    if (!magnitudes_file){
        perror("magnitudes.csv");
        goto cleanup;
    }

    // Skip header
    if (!fgets(header, sizeof(header), magnitudes_file)){
        fprintf(stderr, "Error: magnitudes.csv is empty\n");
        goto cleanup;
    }

    // Read the data
    while (fscanf(magnitudes_file, "%f,%f", &freq, &magnitude) == 2){
        magnitudes[n_read] = magnitude;
        pw[n_read]         = magnitude * magnitude;
        n_read++;
    }

    // Close the file
    fclose(magnitudes_file);
    magnitudes_file = NULL;

    if (n_read == 0){
        fprintf(stderr, "Error: no valid sample found in magnitudes.csv\n");
        goto cleanup;
    }

    if (n_read < NB_K){
        fprintf(stderr, "Warning: only %d/%d samples read, the rest is set to 0\n", n_read, NB_K);
    }


    // Noise normalisation ----------------------------------------------------
    // Devides the magnitudes into blocs of 500 bins, computes the mean for 
    // each bloc and devides each values in the bloc by the mean
    double tmp[W];

    int k = 0;
    while (k<NB_K){
        int len = (k + W <= NB_K) ? W : NB_K - k;

        memcpy(tmp, pw + k, len * sizeof(double));
        qsort(tmp, len, sizeof(double), cmp_double);
        double median = tmp[len / 2];

        double mean = median / log(2.0);
        for (int i = k; i < k + len; i++){
            pw[i] /= mean;
        }

        k += W;
    }
    pw[0] = 0;
    
    // Harmonic summing -------------------------------------------------------
    Cand all[N_STAGES * N_CAND];
    Cand sel[N_FOLD];
    int n_sel = 0; int n_all = 0;

    for (int s = 0; s < N_STAGES; s ++){
        int h = STAGES[s];
        int kmax = NB_K / h;                // so that k*h stays in range
        int kmin = (int)ceil(F_SEARCH_MIN * T_OBS);
        if (kmin < 1) kmin = 1;
        
        // 1. Score candidates
        for (int k = kmin; k < kmax; k++){
            double S = 0.0;
            for (int j = 1 ; j <= h; j++)
                S += pw[j*k];
            score[k] = (S-h) / sqrt((double)h);
        }

        // 2. Keep only N_CAND candidates
        for (int c = 0; c < N_CAND; c++){
            int best = kmin;
            for (int k = kmin; k < kmax; k++)
                if (score[k] > score[best]) best = k;

            all[n_all++] = (Cand){best, h, score[best]};

            // Erase best's peak and neighbours (bescause the magnitudes peaks
            // are not only on one beam (Gaussian))
            // Only SEP neighbours will be deleted on each side

            for (int k = best-SEP; k <= best+SEP; k++)
                if (k >= kmin && k < kmax) score[k] = -1e30;     // Extremely low value to avoid 0
        }
    }

    // 3. Sort candidates by score, best first
    qsort(all, n_all, sizeof(Cand), cmp_cand);
    
    // Remove duplicates
    for (int i = 0; i < n_all && n_sel < N_FOLD; i++){
        int dup = 0;
        for (int j = 0; j < n_sel; j++)
            if (abs(sel[j].k - all[i].k)  <= SEP) { dup = 1; break;}
        if (!dup) sel[n_sel++] = all[i];
    }
    
    for (int i = 0; i < N_FOLD; i++){
        printf("h=%d, k=%d, score=%.2f\n", sel[i].h, sel[i].k, sel[i].score);
    }
    ret = 0;

cleanup:
    if (magnitudes_file) fclose(magnitudes_file);

    free(magnitudes);
    free(pw);
    free(score);

    return ret;
}
