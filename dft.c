//---------------------------------------------------------------------------//
//                                                                           //
//  Provides a Discreate Fourier Transform implementation. This code uses    //
//  the linear/naive implementation in O(n) however a quasilinear            //
//  O(nlog(n)) exists : the FFT (Fas Fourier Transform).                     //
//                                                                           //
//---------------------------------------------------------------------------//


#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define N 4096
#define M_PI 3.1415
#define Nb_k (N/2)                  //Since it will be symetric


int main(void){
    //Variables
    float *dedispersed = malloc(N*sizeof(float));
    float *magnitudes  = malloc(Nb_k*sizeof(float)); 
    if (!dedispersed) return 1;
    if (!magnitudes) return 1;

    //Read data
    FILE *f = fopen("dedispersed.csv", "r");
    if (!f) return 1;

    char header[100];
    fgets(header, sizeof(header), f);                   //ignore the header

    int i;
    float amplitude;
    while (fscanf(f, "%d,%f", &i, &amplitude) == 2 && i<=Nb_k) {
        dedispersed[i] = amplitude;
    }
    fclose(f);
    
    //Compute Re(X[k]) and Im(Im[k])
    for (int k=0; k<Nb_k; k++){
        float re = 0.0;
        float im = 0.0;

        for (int n=0; n<N; n++){
            float angle = -2.0f * M_PI * k * n / N;
            re += dedispersed[n] * cos(angle);
            im += dedispersed[n] * sin(angle);
        }
        magnitudes[k] = sqrtf(re*re + im*im);
    }

    //Write files
    FILE *magnitudes_file = fopen("magnitudes.csv", "w");
    fprintf(magnitudes_file, "k,magnitudes\n");
    fprintf(magnitudes_file, "%f,%f\n", freq, magnitudes[k]);
	for (int i=0; i< Nb_k; i++){
        float freq = (float)k/(N*DT)
		fprintf(magnitudes_file, "%f,%f\n", freq, magnitudes[i]);
	}
	fclose(magnitudes_file);
    
    //Free spaces taken by malloc
    free(dedispersed);
    free(magnitudes);
    
    return 0;
}
