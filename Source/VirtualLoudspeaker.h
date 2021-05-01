/*
  ==============================================================================

    VirtualLoudspeaker.h
    Created: 22 Mar 2021 12:19:19am
    Author:  wolfe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class VirtualLoudspeaker
{
public:
    VirtualLoudspeaker();
    // Init, weighting, and scaling functions used by the processor.
    void initializeLoudspeaker(int speakAngle, int speakWindow, int posOffset, int negOffset, bool isHor);
    float scaleRangeOfFloatingPointNumbers(float input, float inputStart, float inputEnd, float outputStart, float outputEnd);
    void calculateGainWeight(int degreeValue);
    // Function to calculate and apply spread value
    float calculateSpread(float spreadValue, float inGain, bool increment);
    // Public members
    int speakerAngle;
    float previousGain;
    float currentGain;
    bool isHorizontal;
    // Convolution init, loading, and processing functions
    void loadHRIR(juce::dsp::ProcessSpec specs);
    juce::dsp::Convolution convolver;
    void processConvolution(juce::dsp::ProcessContextNonReplacing<float> context);

private:
    // Init members
    int speakerWindow;
    int positiveOffset;
    int negativeOffset;
    // Scaling and calculating functions
    float scaleRangeOfNumbers(int input, int inputStart, int inputEnd, float outputStart, float outputEnd);
    float calculateConstantPower(int incomingValue, int value1, int value2);
    void calculateNormalPanningWindow(int degreeValue);
    // Private members
    float scaledSpreadValue;
};
