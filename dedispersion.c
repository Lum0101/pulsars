#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define DT 1e-4f	//Division temporelle -> 1 pas = 1e-6s
#define N 2000		//Number of computed samples
#define N_CHANNEL 64	//Number of channels
#define F_MIN 1200	//Min frequency mesured (MHz)
#define F_MAX 1500	//Max frequency mesured (MHz)
#define K_DM 4148.8f	//MHz^2pc^{-1}cm^3s -- Physical constant
//DM : constant depending on the pulsar. See ATNF catalog for the constants list.
//Crabe Pulsar (B0531+21) : DM = 56.8 pc/cm^3
#define DM 56.8f

int main(void){
	float **signal = malloc(N_CHANNEL * sizeof(float*));
	for (int i = 0; i < N_CHANNEL; i++) {
    		signal[i] = malloc(N * sizeof(float));
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
