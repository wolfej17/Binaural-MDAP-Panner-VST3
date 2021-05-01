/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "VirtualLoudspeaker.h"
#include "CrossoverFilter.h"
//==============================================================================
/**
*/
// TODO: Constants. 
class MDAPPannerAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    
    MDAPPannerAudioProcessor();
    ~MDAPPannerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // Variables to store azimuth, elevation, spread values, and output gain from GUI knobs
    int azimuthValue;
    int elevationValue;
    float horizontalSpreadValue;
    float verticalSpreadValue;
    float finalOutputGain;
    float previousFinalOutputGain;
    // Public variables to hold data about speaker configuration. Used to set the azimuth knob value in PluginEditor which is dependant on the elevation knob value
    int verticalOffset;
    int verticalSpacing;
    float initSampleRate;
    // Crossover Filter
    CrossoverFilter crossoverFilter;

private:
    //==============================================================================
    // Random value for white noise generation
    juce::Random random;
    // Variables to hold data about speaker configuration
    int numInlineSpeakers;
    int totalNumberSpeakers;
    int horizontalOffset;
    int horizontalSpacing;
    int horizontalCounter;
    int verticalCounter;
    // Initialize convolver with nothing. Correct assignments are decided later in preparetoplay. 
    VirtualLoudspeaker virtualLoudspeakers [6] {};
    // Create processing and pre-output buffers
    juce::AudioBuffer<float> processBuffer;
    juce::AudioBuffer<float> finalPannedBuffer;
    juce::AudioBuffer<float> finalUnpannedBuffer;
    // Define gain and previous gain (for smoothing)
    float gain;
    float previousGain;
    float scaledSpreadValue;
    float maximumElevationGain;
    float decrementValue;
    float previousCrossoverFrequency;
    float crossoverFrequency;
    // Ensure value is never null
    float scaledElevationGain;
    // Coefficient to normalize
    float normalizationCoefficient;
    std::size_t index = 0;
    // Value tree state
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* azimuthParameter  = nullptr;
    std::atomic<float>* elevationParameter = nullptr;
    std::atomic<float>* horizontalSpreadParameter = nullptr;
    std::atomic<float>* verticalSpreadParameter = nullptr;
    std::atomic<float>* crossoverParameter = nullptr;
    std::atomic<float>* gainParameter = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MDAPPannerAudioProcessor)
};
