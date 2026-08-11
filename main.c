#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define N 1000		//Number of computed samples
#define PULSE_POS 500	//Pic position
#define PULSE_WIDTH 5	//Pic width


int main(void){
	float *signal = malloc(N * sizeof(float));		//Dynamic array of N floats
	if (!signal) return 1;					//In case the array wasn't created
	
	for (int i=0; i<N; i++){
		//Gaussian impule f(x)=exp(-((x-center_pos)/width)^2)
		float x = (i-PULSE_POS)/ (float)PULSE_WIDTH;
		signal[i] = expf(-x*x);
		signal[i] += 0.1f * ((float)rand() / RAND_MAX - 0.5f);		//Add noise
	}

	//Data writing
	FILE *f = fopen("signal.csv", "w");
	for (int i=0; i<N; i++){
		fprintf(f, "%d,%f\n", i, signal[i]);
	}
	fclose(f);						//Close file comunication

	free(signal);						//Release/errase the dynamic array memory
	return 0;
}
