/*
  ==============================================================================

    CrossoverFilter.cpp
    Created: 17 Apr 2021 6:24:29pm
    Author:  Jacob Wolfe

  ==============================================================================
*/

#include "CrossoverFilter.h"
CrossoverFilter::CrossoverFilter()
{
    
}

void CrossoverFilter::setFilterParameters(double crossoverFrequency, double sampleRate)
{
    highPassL.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, crossoverFrequency));
    lowPassL.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, crossoverFrequency));
    highPassR.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, crossoverFrequency));
    lowPassR.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, crossoverFrequency));
}

void CrossoverFilter::reset()
{
    highPassL.reset();
    lowPassL.reset();
    highPassR.reset();
    lowPassR.reset();
}

void CrossoverFilter::processBuffers(float* unprocessedSamplesL, float* processedSamplesL, float* unprocessedSamplesR, float* processedSamplesR, int numSamples)
{
    highPassL.processSamples(processedSamplesL, numSamples);
    lowPassL.processSamples(unprocessedSamplesL, numSamples);
    highPassR.processSamples(processedSamplesR, numSamples);
    lowPassR.processSamples(unprocessedSamplesR, numSamples);
}
