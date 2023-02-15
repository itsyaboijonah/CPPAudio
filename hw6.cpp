#include "SineWave.h"
#include "RtWvOut.h"
#include <ADSR.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

using namespace stk;

float getNextNote(){
    
    //Make a notes vector and assigns
    std::vector<int> notes;
    notes.push_back(440);
    notes.push_back(494);
    notes.push_back(554);
    notes.push_back(659);
    notes.push_back(740);
    
    //Returns a random note from the array
    int randNote = (rand() % 5);
    return notes[randNote];
    
}

int main(int argc, char **argv){
    
    srand(time(NULL));
    
    //Ensure user-provided arguments are correct
    if (argc != 3) {
        std::cout << "Please provide two arguments: tone duration in seconds (int), and file duration in seconds (int)" << std::endl;
        exit(0);
    }
    
    if(atof(argv[1]) > atof(argv[2])){
        std::cout << "The tone length must be shorter than file length" << std::endl;
        exit(0);
    }
    
    double toneDuration = atof(argv[1]);
    double fileDuration = atof(argv[2]);
    
    //Convert to the number of samples
    double sampleRate = 44100.0;
    long samplesPerTone = (long) (toneDuration * sampleRate);
    long samplesPerFile = (long) (fileDuration * sampleRate);
    long numNotes = (long) ceil(fileDuration / toneDuration);
    
    //Initialize Stk objects
    Stk::setSampleRate(sampleRate);
    RtWvOut dac(1);
    SineWave sine;
    ADSR envelope;
    
    //Initializes envelope
    envelope.setAllTimes((toneDuration * 0.1), (toneDuration * 0.1), (toneDuration * 0.4), (toneDuration * 0.4));
    
    long currentSample = 0;
    
    double outputNote;
    
    //Plays all the samples for each note
    for (int i = 0; i < numNotes; i++) {
        
        outputNote = getNextNote();
        
        envelope.keyOn();
        
        for (int j = 0; j < samplesPerTone; j++) {
            
            sine.setFrequency(outputNote);
            dac.tick(envelope.lastOut() * sine.tick());
            envelope.tick();
            
            if (j == (samplesPerTone / 2)){
                envelope.keyOff();
            }
        }
    }
}

