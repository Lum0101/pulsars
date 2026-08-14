#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"


int main(void){
	float **signal = calloc(N_CHANNEL,  sizeof(float*));		//Dynamic array of N_CHANNEL pointer, pointing to N_CHANNEL arrays
	float *decays  = malloc(N_CHANNEL * sizeof(float));		//Dynamic array to store the channel's decays 
	if (!signal) return 1;						//In case the array wasn't created
	if (!decays) return 1;


	//Create the N_CHANNEL arrays
	for (int i=0; i<N_CHANNEL; i++){
		signal[i] = calloc(N, sizeof(float));			//Assigning the pointer returned by malloc to the array
		if (!signal[i]) return 1;				//In case the array wasn't created
	}

	for (int i=0; i<N_CHANNEL; i++){
		//Frequency of the channel
		float f_c = F_MIN + i*(F_MAX-F_MIN)/(N_CHANNEL-1);			        //Linear repartition of the frequency span
		float delta_t = K_DM * DM * (1.0f/(f_c*f_c) - 1.0f/(F_REF*F_REF));	//Signal decay in regard to frequency
		decays[i] = delta_t;							                    //Store the decay in decays array					
        int decay_index = (int)(delta_t / DT);   				            //Signal to index decay
        
        int max_decay_index = (int)(K_DM * DM * (1.0f/(F_MIN*F_MIN) - 1.0f/(F_REF*F_REF)) / DT);

        for (int p=0; (FIRST_PULSE_POS + max_decay_index +p*PERIOD_INDEX)<N; p++){
            int pulse_center = FIRST_PULSE_POS + decay_index + p*PERIOD_INDEX;
            for (int c=0; c<N; c++){
                float x = (c-pulse_center)/(float)PULSE_WIDTH;
                signal[i][c] += exp(-x*x);
            }
        }
        for (int c=0; c<N; c++){
            signal[i][c] += 0.1f * ((float)rand() / RAND_MAX - 0.5f);
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
