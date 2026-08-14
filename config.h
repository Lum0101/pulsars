//---------------------------------------------------------------------------//
//  The frequency band used is the L-BAND, used in labs.
//  K_DM is a physical constant on MHz^2.cm^3s/pc haveing a role in 
//  diffusion.
//  DM is a physical constant depending on the pulsar. Values are aviable 
//  on the ATNF catalog.
//  For this project I use the "Crab" pulsar (B0531+21)
//---------------------------------------------------------------------------//


#ifndef CONFIG_H
#define CONFIG_H

#define DT              1e-3f               //Time division
#define N               4096                //Number of computed samples
#define FIRST_PULSE_POS 5                   //First pic position
#define PULSE_WIDTH     5                   //Pic width
#define N_CHANNEL       64                  //Number of channels
#define F_MIN           1200                //Min frequency of band
#define F_MAX           1500                //Max frequency of band
#define F_REF           F_MAX               //Reference frequency
#define K_DM            4148.8f             //Physical constant -- see above
#define DM              56.8f               //Physical constant -- see above
#define PERIOD          0.033f              //Period of the pulusar
#define PERIOD_INDEX    (int)(PERIOD/DT)    //Period inn time division

#endif
