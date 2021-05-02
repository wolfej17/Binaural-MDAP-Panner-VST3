/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#define _USE_MATH_DEFINES
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>



//==============================================================================
MDAPPannerAudioProcessor::MDAPPannerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                       parameters (*this, nullptr, juce::Identifier ("MDAPPannerValueTree"),
                       {
                           std::make_unique<juce::AudioParameterInt> ("azimuth", "Azimuth", 0, 360, 0),
                           std::make_unique<juce::AudioParameterInt> ("elevation", "Elevation", 0, 360, 0),
                           std::make_unique<juce::AudioParameterFloat> ("horizontalSpread", "HorizontalSpread", 0.0f, 1.0f, 0.0f),
                           std::make_unique<juce::AudioParameterFloat> ("verticalSpread", "VerticalSpread", 0.0f, 1.0f, 0.0f),
                           std::make_unique<juce::AudioParameterInt> ("thisGain", "ThisGain", -96, 6, 0),
                           std::make_unique<juce::AudioParameterInt> ("crossover", "Crossover", 20.0f, 1000.0f, 150.0f)
                       })
#endif
{
    azimuthParameter = parameters.getRawParameterValue ("azimuth");
    elevationParameter = parameters.getRawParameterValue ("elevation");
    horizontalSpreadParameter = parameters.getRawParameterValue ("horizontalSpread");
    verticalSpreadParameter = parameters.getRawParameterValue ("verticalSpread");
    gainParameter = parameters.getRawParameterValue ("thisGain");
    crossoverParameter = parameters.getRawParameterValue ("crossover");
}

MDAPPannerAudioProcessor::~MDAPPannerAudioProcessor()
{
}

//==============================================================================
const juce::String MDAPPannerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MDAPPannerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MDAPPannerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MDAPPannerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MDAPPannerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MDAPPannerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MDAPPannerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MDAPPannerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MDAPPannerAudioProcessor::getProgramName (int index)
{
    return {};
}

void MDAPPannerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MDAPPannerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{    // Init data for gain weighting.
    previousFinalOutputGain = 1.0f;
    finalOutputGain = 1.0f;
    normalizationCoefficient = 3.0f;
    azimuthValue = 0.0f;
    elevationValue = 0.0f;
    horizontalSpreadValue = 0.0f;
    verticalSpreadValue = 0.0f;
    gain = 0.0f;
    maximumElevationGain = 0.0f;
    scaledElevationGain = 0.0f;
    crossoverFilter.setFilterParameters(150.0f, sampleRate);
    juce::dsp::ProcessSpec specs;
    specs.sampleRate = sampleRate;
    specs.maximumBlockSize = samplesPerBlock;
    specs.numChannels = getTotalNumOutputChannels();
    initSampleRate = sampleRate;
    // ************** The following initializes the virtual loudspeaker array. Could be extended to user-defined input. **************
    // Set desired number of horizontal and vertical speakers
    numInlineSpeakers = 4;
    totalNumberSpeakers = 6;
    // Set the horizontal and vertical offset from 0 deg azimuth, 0 deg elevation (north point)
    horizontalOffset = 45;
    verticalOffset = 90;
    // Set the spacing between horizontal and vertical speakers
    horizontalSpacing = 90;
    verticalSpacing = 180;
    // Initialize counters
    horizontalCounter = 0;
    verticalCounter = 0;
    // Fill array with correct loudspeaker data
    for (int i = 0; i < totalNumberSpeakers; i++)
   {
       if (i < numInlineSpeakers)
       {
           virtualLoudspeakers[i].initializeLoudspeaker(horizontalOffset + horizontalCounter, 360 / numInlineSpeakers, horizontalOffset, 90 - horizontalOffset, true);
           horizontalCounter += horizontalSpacing;
       }
       else
       {
           virtualLoudspeakers[i].initializeLoudspeaker(verticalOffset + verticalCounter, 360 / numInlineSpeakers, horizontalOffset, 90 - horizontalOffset, false);
           verticalCounter += verticalSpacing;
       }
       // Initialize convolvers with IRs loaded in binary data
       virtualLoudspeakers[i].loadHRIR(specs);
       virtualLoudspeakers[i].currentGain = 0.0f;
   }
}

void MDAPPannerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MDAPPannerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MDAPPannerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // TODO: The gain weighting algorithm could be pushed to VirtualLoudspeaker class, but variables would have to be created for each instance to hold UI input. Readability vs memory size.
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    //Clear data in case outputs are larger than inputs
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
        processBuffer.clear (i, 0, processBuffer.getNumSamples());
        finalPannedBuffer.clear (i, 0, processBuffer.getNumSamples());
        finalUnpannedBuffer.clear (i, 0, processBuffer.getNumSamples());
    }
    // make copy of buffer into final process buffer and set the gain to 0
    finalPannedBuffer.makeCopyOf(buffer);
    finalUnpannedBuffer.makeCopyOf(buffer);
    processBuffer.makeCopyOf(buffer);
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        finalPannedBuffer.applyGain(channel, 0, buffer.getNumSamples(), 0.0f);
        finalUnpannedBuffer.applyGain(channel, 0, buffer.getNumSamples(), 0.0f);
    }
    // Update parameters
    azimuthValue = *azimuthParameter;
    elevationValue = *elevationParameter;
    horizontalSpreadValue = *horizontalSpreadParameter;
    verticalSpreadValue = *verticalSpreadParameter;
    crossoverFrequency = *crossoverParameter;
    // Update Crossover Filter
    if (previousCrossoverFrequency != crossoverFrequency)
    {
        crossoverFilter.setFilterParameters(crossoverFrequency, initSampleRate);
    }
    float gainParam = *gainParameter;
    finalOutputGain = juce::Decibels::decibelsToGain(gainParam);
    // Iterate through each virtual loudspeaker, calculate the MDAP weights, and apply them to the correct HRIRs based azimuth/elevation values.
    juce::dsp::AudioBlock<float> inBlock(buffer);
    for (int i = 0; i < totalNumberSpeakers; ++i)
    {
        processBuffer.makeCopyOf(buffer);
        juce::dsp::AudioBlock<float> outBlock(processBuffer);
        juce::dsp::ProcessContextNonReplacing<float> thisContext(inBlock, outBlock);
        // Will need to change the below if another configuration of virtual loudspeakers is desired. Parallel convolution is used instead of IR
        // switching to avoid crossfading and possible artifacts. See http://publications.rwth-aachen.de/record/466561/files/466561.pdf
        // TODO: Possible: According to JUCE Convolution docs, "This class provides some thread-safe functions to load impulse responses from audio files or memory on-the-fly without noticeable artefacts, performing resampling and trimming if necessary." Experiment with IR switching to simplify modification to custom loudspeaker layouts.
        // Grab previous gain of loudspeaker for smooth gain changes
        previousGain = virtualLoudspeakers[i].currentGain;
        virtualLoudspeakers[i].processConvolution(thisContext);
        // Convolve the HRTFs at respective degrees for horizontal speakers (azimuth == 0)
        if (virtualLoudspeakers[i].isHorizontal)
        {
            // TODO: We don't need to calculate azimuth, elevation, and spread for every processBlock call. This will bog down performance. Move processing only when the associated slider value has changed.
            // Calculate VBAP gain weights for the current azimuth value
            virtualLoudspeakers[i].calculateGainWeight(azimuthValue);
            // If more than two loudspeakers are active, apply panning weights
            gain = virtualLoudspeakers[i].currentGain;
            if (gain > 0.0f)
            {
                // Inner loop through vertical virtualLoudspeakers to re-calculate horizontal gain
                for (int j = numInlineSpeakers; j < totalNumberSpeakers; ++j)
                {
                    // Find the maximum elevation gain. This is the active elevation speaker we need
                    if (virtualLoudspeakers[j].currentGain > maximumElevationGain)
                    {
                        scaledElevationGain = virtualLoudspeakers[j].scaleRangeOfFloatingPointNumbers(virtualLoudspeakers[j].currentGain, 0.0f, 1.0f, 0.0f, gain);
                        maximumElevationGain = virtualLoudspeakers[j].currentGain;
                    }
                }
                virtualLoudspeakers[i].currentGain -= scaledElevationGain;
                gain -= scaledElevationGain;
                maximumElevationGain = 0.0f;
            }
            // Calculate spread value for horizontal speakers
            gain = virtualLoudspeakers[i].calculateSpread(horizontalSpreadValue, virtualLoudspeakers[i].currentGain, true);
        }
        // If virtual loudspeaker has an elevation value that is not zero
        else
        {
            // Calculate VBAP weight for elevation
            virtualLoudspeakers[i].calculateGainWeight(elevationValue);
            // Calculate spread value for elevation
            virtualLoudspeakers[i].calculateSpread(verticalSpreadValue, virtualLoudspeakers[i].currentGain, true);
            // Decrement vertical loudspeaker gains with the horizontal spread value
            gain = virtualLoudspeakers[i].calculateSpread(horizontalSpreadValue, virtualLoudspeakers[i].currentGain, false);
        }
        // Apply a gain ramp to the processBuffer (changes with every virtual loudspeaker iteration) and add the output to the pre-output buffer (finalProcessBuffer)
        // Possibly improper way of doing this, but make absolutely certain that gain will not exceed 1.0.
        if (gain > 1.0f)
        {
            gain = 1.0f;
        }
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            // This buffer is used for low frequency information (not panned).
            finalUnpannedBuffer.addFrom(channel, 0, processBuffer, channel, 0, processBuffer.getNumSamples());
            // This buffer is used to add the current convolved signal with the loudspeaker and scale respective gain weights.
            processBuffer.applyGainRamp(channel, 0, processBuffer.getNumSamples(), previousGain * normalizationCoefficient, gain * normalizationCoefficient);
            // This is buffer is used to add all convolved and panned signals to a final output buffer. Pre-output buffer.
            finalPannedBuffer.addFrom(channel, 0, processBuffer, channel, 0, processBuffer.getNumSamples());
        }
    }
    // Apply crossover filter. We don't want low frequencies to have the panning applied.
    crossoverFilter.processBuffers(finalUnpannedBuffer.getWritePointer(0), finalPannedBuffer.getWritePointer(0), finalUnpannedBuffer.getWritePointer(1), finalPannedBuffer.getWritePointer(1), buffer.getNumSamples());
    // Copy the pre-output buffer to the final output buffer
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        buffer.copyFrom(channel, 0, finalPannedBuffer, channel, 0, finalPannedBuffer.getNumSamples());
        buffer.addFrom(channel, 0, finalUnpannedBuffer, channel, 0, finalUnpannedBuffer.getNumSamples());
        buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), previousFinalOutputGain, finalOutputGain);
    }
    previousFinalOutputGain = finalOutputGain;
    previousCrossoverFrequency = *crossoverParameter;
}

//==============================================================================
bool MDAPPannerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MDAPPannerAudioProcessor::createEditor()
{
    return new MDAPPannerAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void MDAPPannerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MDAPPannerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName (parameters.state.getType()))
                parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MDAPPannerAudioProcessor();
}
