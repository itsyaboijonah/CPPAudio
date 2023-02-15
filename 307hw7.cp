#include "ADSR.h"
#include "SineWave.h"
#include "RtWvOut.h"
#include <iostream>
#include <cmath>

using namespace stk;

int main(int argc, char** argv){

    // Declares variables
    double fundamental, carrierFrequency, modulatorFrequency, duration;
    int minIntensity = 2, maxIntensity = 4;

    // Checks number of arguments and provides usage statement
    if (argc != 1){
        
        if ((argc - 1) != 2){
            std::cout << "USAGE: Please input a fundamental frequency in Hertz (double) and a duration in seconds (double)." << std::endl;
            exit(0);
        }
        
        else{
            
            fundamental = atof(argv[1]);
            carrierFrequency = 3 * fundamental;
            modulatorFrequency = 2 * fundamental;
            duration = atof(argv[2]);
            
        }
    }
    
    // Default case
    else{
        
        fundamental = 300;
        carrierFrequency = 3 * fundamental;
        modulatorFrequency = 2 * fundamental;
        duration = atof(argv[2]);
        
    }
    
    int numSamples = floor(duration * 44100);
    
    // Set the global sample rate before creating class instances.
    Stk::setSampleRate( 44100.0 );
    
    ADSR f1;
    ADSR f2;
    
    // Create looping wavetables with sine waveforms.
    SineWave carrier;
    SineWave modulator;
    
    // Create a one-channel realtime output object.
    RtWvOut output;
    output.start();
    
    // Set our FM parameters.
    modulator.setFrequency(modulatorFrequency);
    
    f1.setAttackTime(0.1);
    f1.setDecayTime(0.1);
    f1.setReleaseTime(0.3);
    f2.setAttackTime(0.001);
    f2.setDecayTime(0.1);
    f1.keyOn();
    f2.keyOn();
    
    // Calculates release count
    long releaseCount = (long) (Stk::sampleRate() * fabs(duration - 0.1));
    
    // Start the runtime loop.
    long counter = 0;
    while ( f1.getState() != ADSR::IDLE ) {
        
        // Do FM calculations.
        carrier.setFrequency( ((f2.lastOut() * modulatorFrequency * (maxIntensity - minIntensity)) + modulatorFrequency * minIntensity) * modulator.tick() + carrierFrequency );
        output.tick( f1.lastOut() * carrier.tick() );
        
        // Update the envelopes and check for release time.
        f1.tick();
        f2.tick();
        if ( counter++ == releaseCount )
            f1.keyOff();
            f2.keyOff();
        }
    
    return 0;
}
