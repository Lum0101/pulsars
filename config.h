//---------------------------------------------------------------------------//
//  Simulation parameters.
//
//  The frequency band used is the L-band, commonly used in labs.
//  K_DM is the dispersion constant, in MHz^2.cm^3.s/pc. It links the delay
//  of a pulse to its frequency.
//  DM is the dispersion measure of the pulsar, in pc/cm^3. Values are
//  available in the ATNF catalogue.
//  This project uses the Crab pulsar (B0531+21).
//---------------------------------------------------------------------------//

#include <math.h>

#ifndef CONFIG_H
#define CONFIG_H

#define DT              1e-3f                           // Time step (s)
#define N               4096                            // Number of samples
#define FIRST_PULSE_POS 5                               // Position of the first pulse (samples)
#define PULSE_WIDTH     5                               // Pulse width (samples)
#define N_CHANNEL       64                              // Number of frequency channels
#define F_MIN           1200.0f                         // Lower band edge (MHz)
#define F_MAX           1500.0f                         // Upper band edge (MHz)
#define F_REF           F_MAX                           // Reference frequency (MHz)
#define K_DM            4148.8f                         // Dispersion constant (see above)
#define DM              56.8f                           // Dispersion measure (see above)
#define PERIOD          0.033f                          // Pulsar period (s)
#define PERIOD_INDEX    ((int)(PERIOD / DT + 0.5f))     // Period in samples (rounded)
#define NOISE_AMPLITUDE 0.1f                            // Peak-to-peak amplitude of the background noise
#define DELAYS_SPAN_S   (K_DM * DM * (1.0f / \
            (F_MIN * F_MIN) - 1.0f / (F_MAX * F_MAX)))  // Total dispersion delay across the band (sec)
#define MAX_DELAY       ((int)ceilf(DELAY_SPAN_S / DT)) //Total delay in samples, rounded up
#define N_UTIL          (N-MAX_DELAY)                   //Number of usable saples
#endif
