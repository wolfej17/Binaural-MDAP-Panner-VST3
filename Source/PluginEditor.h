/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SliderReverse.h"

//==============================================================================
/**
*/
// TODO: Constants. 
class MDAPPannerAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    MDAPPannerAudioProcessorEditor (MDAPPannerAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~MDAPPannerAudioProcessorEditor();
    juce::Slider gainSlider;
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    void sliderValueChanged(juce::Slider* slider) override;
    MDAPPannerAudioProcessor& audioProcessor;
    Slider_reverse azimuthKnob;
    juce::Slider elevationKnob;
    juce::Slider horizontalSpreadKnob;
    juce::Slider verticalSpreadKnob;
    juce::Slider crossoverKnob;
    juce::Label gainLabel;
    juce::Label crossoverLabel;
    juce::Label azimuthLabel;
    juce::Label elevationLabel;
    juce::Label verticalSpreadLabel;
    juce::Label horizontalSpreadLabel;
    // Head pictures
    juce::Image headFrontImage;
    juce::Image headSideImage;
    // Variable to store the unaltered azimuth value when the elevation knob value is set to change the azimuth knob value
    int previousAzimuthValue;
    bool azimuthChanged = false;
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    std::unique_ptr<SliderAttachment> azimuthAttachment;
    std::unique_ptr<SliderAttachment> elevationAttachment;
    std::unique_ptr<SliderAttachment> horiontalSpreadAttachment;
    std::unique_ptr<SliderAttachment> verticalSpreadAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<SliderAttachment> crossoverAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MDAPPannerAudioProcessorEditor)
};
