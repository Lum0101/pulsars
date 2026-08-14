#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"

int main(void){
	float **signal = calloc(N_CHANNEL, sizeof(float*));
	for (int i = 0; i < N_CHANNEL; i++) {
    		signal[i] = calloc(N, sizeof(float));
    		if (!signal[i]) return 1;
	}

	float *decays = malloc(N_CHANNEL * sizeof(float));

	float *dedispersed = malloc(N * sizeof(float));
	
	
	//Read signal data
	FILE *data_file = fopen("signal.csv", "r");
	if (!data_file) return 1; 

	//To ignore the header
	char header[100];
	fgets(header, sizeof(header), data_file);
	

	int channel, i;
	float amplitude;

	while (fscanf(data_file, "%d,%d,%f", &channel, &i, &amplitude) == 3) {
    		signal[channel][i] = amplitude;
	}
	fclose(data_file);

	//Read decays data
	FILE *decays_file = fopen("decays.csv", "r");
	if (!decays_file) return 1;

	//To ignore the header
	fgets(header, sizeof(header), decays_file);

	float decay;

	while (fscanf(decays_file, "%d,%f", &channel, &decay) == 2){
		decays[channel] = decay;
	}
	fclose(decays_file);

	

	//Dedisperse data
	for (int i=0; i<N; i++){
		float sum_i = 0.0;
		for (int c=0; c<N_CHANNEL; c++){
			int idx = i - (int)(decays[c]/DT);
			if (idx >=0 && idx < N){
				sum_i += signal[c][idx];
			}
		}
		dedispersed[i] = sum_i;
	}


	//Write data
	FILE *dedispersed_file = fopen("dedispersed.csv", "w");
	if (!dedispersed_file) return 0;
	fprintf(dedispersed_file, "i,amplitude\n");
	for (int i=0; i<N; i++){
		fprintf(dedispersed_file, "%d,%f\n", i, dedispersed[i]);
	}
	fclose(dedispersed_file);

	
		
	//Free memory
	for (int i=0; i<N_CHANNEL; i++){
		free(signal[i]);
	}
	free(signal);
	free(decays);
	free(dedispersed);

	return 0;
}
