/*
  ==============================================================================

    CrossoverFilter.h
    Created: 17 Apr 2021 6:24:29pm
    Author:  Jacob Wolfe

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once
class CrossoverFilter
{
    
public:
    CrossoverFilter();
    void setFilterParameters(double crossoverFrequency, double sampleRate);
    void reset();
    void processBuffers(float* unprocessedSamplesL, float* processedSamplesL, float* unprocessedSamplesR, float* processedSamplesR, int numSamples);
    
private:
    // TODO: Yes, this can be simplified using processor duplicator.
    juce::IIRFilter lowPassL;
    juce::IIRFilter lowPassR;
    juce::IIRFilter highPassL;
    juce::IIRFilter highPassR;
};
