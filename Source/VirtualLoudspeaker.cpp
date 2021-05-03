/*
  ==============================================================================

    VirtualLoudspeaker.cpp
    Created: 22 Mar 2021 12:19:19am
    Author:  wolfe

  ==============================================================================
*/

#include "VirtualLoudspeaker.h"
#include <BinaryData.h>
#include <math.h>

VirtualLoudspeaker::VirtualLoudspeaker()
{
}

void VirtualLoudspeaker::initializeLoudspeaker(int speakAngle, int speakWindow, int posOffset, int negOffset, bool isHor)
{
    speakerAngle = speakAngle;
    speakerWindow = speakWindow;
    positiveOffset = posOffset;
    negativeOffset = negOffset;
    isHorizontal = isHor;
}

void VirtualLoudspeaker::loadHRIR(juce::dsp::ProcessSpec specs)
{
    if (isHorizontal)
    {
        if (speakerAngle == 45)
        {
            convolver.loadImpulseResponse(BinaryData::azi45el0_wav, BinaryData::azi45el0_wavSize, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        }
        else if (speakerAngle == 135)
        {
            convolver.loadImpulseResponse(BinaryData::azi135el0_wav, BinaryData::azi135el0_wavSize, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        }
        else if (speakerAngle == 225)
        {
            convolver.loadImpulseResponse(BinaryData::azi225el0_wav, BinaryData::azi225el0_wavSize, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        }
        else if (speakerAngle == 315)
        {
            convolver.loadImpulseResponse(BinaryData::azi315el0_wav, BinaryData::azi315el0_wavSize, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        }
           
    }
    else
    {
        if (speakerAngle == 90)
        {
            convolver.loadImpulseResponse(BinaryData::azi0el90_wav, BinaryData::azi0el90_wavSize, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        }
        else if (speakerAngle == 270)
        {
            convolver.loadImpulseResponse(BinaryData::azi0el270_wav, BinaryData::azi0el270_wavSize, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        }
    }
    convolver.prepare(specs);
}

void VirtualLoudspeaker::processConvolution(juce::dsp::ProcessContextNonReplacing<float> context)
{
    convolver.process(context);
}

// TODO: Implement method for time delays between right and left ears
void VirtualLoudspeaker::calculateGainWeight(int degreeValue)
{
    // Calculate VBAP gain for offset configurations
    // Calculate gain for speaker1 of pairwise speakers
    if (speakerAngle == 360 - negativeOffset)
    {
        if (degreeValue >= speakerAngle)
        {
            currentGain = calculateConstantPower(degreeValue, speakerAngle, speakerAngle + speakerWindow);
        }
        else if (degreeValue <= positiveOffset && degreeValue >= 0.0f)
        {
            currentGain = calculateConstantPower(degreeValue, positiveOffset - speakerWindow, positiveOffset);
        }
        else
        {
            calculateNormalPanningWindow(degreeValue);
        }
    }
    // Calculate gain for speaker2 of pairwise speakers
    else if (speakerAngle == positiveOffset)
    {
        if (degreeValue <= positiveOffset && degreeValue >= 0.0f)
        {
            currentGain = calculateConstantPower(degreeValue, positiveOffset, positiveOffset - speakerWindow);
        }
        else if (degreeValue >= (360 - negativeOffset) && degreeValue <= 360.0f)
        {
            currentGain = calculateConstantPower(degreeValue, 360 + positiveOffset, (360 - negativeOffset));
        }
        else
        {
            calculateNormalPanningWindow(degreeValue);
        }
    }
    // Calculate normal panning windows
    else
    {
        calculateNormalPanningWindow(degreeValue);
    }
}
    
void VirtualLoudspeaker::calculateNormalPanningWindow(int degreeValue)
{
    // Calculate normal panning windows
    // Calculate gain for speaker1 of pairwise speakers
    if (degreeValue >= speakerAngle && degreeValue < speakerAngle + speakerWindow)
    {
        currentGain = calculateConstantPower(degreeValue, speakerAngle, speakerAngle + speakerWindow);
    }
    // Calculate gain for speaker2 of pairwise speakers
    else if (degreeValue <= speakerAngle && degreeValue > abs(speakerAngle - speakerWindow))
    {
        currentGain = calculateConstantPower(degreeValue, speakerAngle, speakerAngle - speakerWindow);
    }
    else
    {
        currentGain = 0.0f;
    }
}

float VirtualLoudspeaker::calculateSpread(float spreadValue, float inGain, bool increment)
{
    if (increment)
    {
        scaledSpreadValue = scaleRangeOfFloatingPointNumbers(spreadValue, 0.0f, 1.0f, 0.0f, 1.0f - inGain);
        currentGain += scaledSpreadValue;
    }
    else
    {
        scaledSpreadValue = scaleRangeOfFloatingPointNumbers(spreadValue, 0.0f, 1.0f, 0.0f, inGain);
        currentGain -= scaledSpreadValue;
    }
    return currentGain;
}

float VirtualLoudspeaker::scaleRangeOfFloatingPointNumbers(float input, float inputStart, float inputEnd, float outputStart, float outputEnd)
{
    return outputStart + ((outputEnd - outputStart) / (inputEnd - inputStart)) * (input - inputStart);
}

float VirtualLoudspeaker::scaleRangeOfNumbers(int input, int inputStart, int inputEnd, float outputStart, float outputEnd)
{
    return outputStart + ((outputEnd - outputStart) / (inputEnd - inputStart)) * (input - inputStart);
}

float VirtualLoudspeaker::calculateConstantPower(int incomingValue, int value1, int value2)
{
     constexpr double pi = 3.14159265358979323846;
     return sin(scaleRangeOfFloatingPointNumbers(incomingValue, value1, value2,  pi * 0.5f, pi));
}
