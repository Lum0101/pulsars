#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define DT 1e-4f	//Division temporelle -> 1 pas = 1e-6s
#define N 2000		//Number of computed samples
#define PULSE_POS (N/2)	//Pic position
#define PULSE_WIDTH 5	//Pic width
#define N_CHANNEL 64	//Nuber of channels
#define F_MIN 1200	//Min frequency mesured (MHz)
#define F_MAX 1500	//Max frequency mesured (MHz)
#define F_REF F_MAX
#define K_DM 4148.8f	//MHz^2pc^{-1}cm^3s -- Physical constant
//DM : constant depending on the pulsar. See ATNF catalog for the constants list.
//Crabe Pulsar (B0531+21) : DM = 56.8 pc/cm^3
#define DM 56.8f


int main(void){
	float **signal = malloc(N_CHANNEL * sizeof(float*));		//Dynamic array of N_CHANNEL pointer, pointing to N_CHANNEL arrays
	float *decays  = malloc(N_CHANNEL * sizeof(float));		//Dynamic array to store the channel's decays 
	if (!signal) return 1;						//In case the array wasn't created
	if (!decays) return 1;


	//Create the N_CHANNEL arrays
	for (int i=0; i<N_CHANNEL; i++){
		signal[i] = malloc(N * sizeof(float));			//Assigning the pointer returned by malloc to the array
		if (!signal[i]) return 1;				//In case the array wasn't created
	}

	for (int i=0; i<N_CHANNEL; i++){
		//Frequency of the channel
		float f_c = F_MIN + i*(F_MAX-F_MIN)/(N_CHANNEL-1);			//Linear repartition of the frequency span
		float delta_t = K_DM * DM * (1.0f/(f_c*f_c) - 1.0f/(F_REF*F_REF));	//Signal decay in regard to frequency
		decays[i] = delta_t;							//Store the decay in decays array					
		int decay_index = (int)(delta_t / DT);   				//Signal to index decay
		for (int c=0; c<N; c++){
			//Gaussian impule f(x)=exp(-((x-center_pos)/width)^2)
			float x = (c-PULSE_POS+decay_index)/ (float)PULSE_WIDTH;
			signal[i][c] = expf(-x*x);
			signal[i][c] += 0.1f * ((float)rand() / RAND_MAX - 0.5f);		//Add noise
		}
	}

	//Data writing
	FILE *f = fopen("signal.csv", "w");
	fprintf(f, "channel,i,amplitude\n");
	for (int c = 0; c < N_CHANNEL; c++) {
    		for (int i = 0; i < N; i++) {
        		fprintf(f, "%d,%d,%f\n", c, i, signal[c][i]);
    		}	
	}
	fclose(f);						//Close file comunication
								
	FILE *decays_file = fopen("decays.csv", "w");
	fprintf(decays_file, "channel,decay\n");
	for (int i=0; i< N_CHANNEL; i++){
		fprintf(decays_file, "%d,%f\n", i, decays[i]);
	}
	fclose(decays_file);
	
	//Freeing
	for (int i=0; i<N_CHANNEL; i++){
		free(signal[i]);					//Free every sub-array memory space
	}
	free(signal);							//Free parent array memory space
	free(decays);
	
	return 0;
}
