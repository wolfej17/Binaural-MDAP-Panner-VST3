/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#define _USE_MATH_DEFINES
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SliderReverse.h"
#include <cmath>
//==============================================================================
MDAPPannerAudioProcessorEditor::MDAPPannerAudioProcessorEditor (MDAPPannerAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), azimuthKnob("azimuthKnob"), elevationKnob("elevationKnob"), valueTreeState(vts)
{
    setSize(400, 310);
    //Azimuth
    azimuthAttachment.reset (new SliderAttachment (valueTreeState, "azimuth", azimuthKnob));
    azimuthKnob.setSliderStyle(juce::Slider::Rotary);
    azimuthKnob.setRange(0, 360, 1);
    azimuthKnob.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 30, 20);
    azimuthKnob.setRotaryParameters(0, 2 * M_PI, false);
    azimuthKnob.setColour(azimuthKnob.textBoxBackgroundColourId, juce::Colours::black);
    azimuthKnob.setName("Azimuth");
    azimuthKnob.setValue(0);
    azimuthKnob.addListener(this);
    addAndMakeVisible(&azimuthKnob);
    // Elevation
    elevationAttachment.reset (new SliderAttachment (valueTreeState, "elevation", elevationKnob));
    elevationKnob.setSliderStyle(juce::Slider::Rotary);
    elevationKnob.setRange(0, 360, 1);
    elevationKnob.setRotaryParameters(1.5 * M_PI, 3.5 * M_PI, false);
    elevationKnob.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 30, 20);
    elevationKnob.setColour(azimuthKnob.textBoxBackgroundColourId, juce::Colours::black);
    elevationKnob.setName("Elevation");
    elevationKnob.setValue(0);
    elevationKnob.addListener(this);
    addAndMakeVisible(&elevationKnob);
    // Crossover Knob
    crossoverAttachment.reset (new SliderAttachment (valueTreeState, "crossover", crossoverKnob));
    crossoverKnob.setSliderStyle(juce::Slider::Rotary);
    crossoverKnob.setRange(20.0f, 1000.0f, 1.0f);
    crossoverKnob.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 40, 20);
    crossoverKnob.setRotaryParameters(1.25 * M_PI, 2.75 * M_PI, true);
    crossoverKnob.setColour(azimuthKnob.textBoxBackgroundColourId, juce::Colours::black);
    crossoverKnob.setValue(150.0f);
    crossoverKnob.setSkewFactorFromMidPoint(250.0);
    crossoverKnob.setName("Crossover");
    addAndMakeVisible(&crossoverKnob);
    // Horizontal Spread Knob
    horiontalSpreadAttachment.reset (new SliderAttachment (valueTreeState, "horizontalSpread", horizontalSpreadKnob));
    horizontalSpreadKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    horizontalSpreadKnob.setRange(0, 1, 0.0001);
    horizontalSpreadKnob.setTextBoxStyle(juce::Slider::NoTextBox, true, 40, 5);
    horizontalSpreadKnob.setColour(azimuthKnob.textBoxBackgroundColourId, juce::Colours::black);
    horizontalSpreadKnob.setValue(0.0f);
    addAndMakeVisible(&horizontalSpreadKnob);
    horizontalSpreadKnob.setName("HorizontalSpread");
    // Vertical Spread Knob
    verticalSpreadAttachment.reset (new SliderAttachment (valueTreeState, "verticalSpread", verticalSpreadKnob));
    verticalSpreadKnob.setSliderStyle(juce::Slider::LinearHorizontal);
    verticalSpreadKnob.setRange(0, 1, 0.0001);
    verticalSpreadKnob.setTextBoxStyle(juce::Slider::NoTextBox, true, 40, 20);
    verticalSpreadKnob.setColour(azimuthKnob.textBoxBackgroundColourId, juce::Colours::black);
    verticalSpreadKnob.setValue(0.0f);
    addAndMakeVisible(&verticalSpreadKnob);
    verticalSpreadKnob.setName("VerticalSpread");
    // Gain Slider
    gainAttachment.reset (new SliderAttachment (valueTreeState, "thisGain", gainSlider));
    gainSlider.setSliderStyle(juce::Slider::LinearVertical);
    gainSlider.setRange(-96, 6, 1);
    gainSlider.setSkewFactorFromMidPoint(-18.0f);
    gainSlider.setColour(azimuthKnob.textBoxBackgroundColourId, juce::Colours::black);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 30, 20);
    gainSlider.setName("Gain");
    gainSlider.setValue(0);
    addAndMakeVisible(&gainSlider);
    // Gain Label
    addAndMakeVisible(&gainLabel);
    gainLabel.setFont(juce::Font (12.0f, juce::Font::bold));
    gainLabel.setText ("Gain (dB)", juce::dontSendNotification);
    gainLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    gainLabel.setJustificationType (juce::Justification::centred);
    // Crossover Label
    addAndMakeVisible(&crossoverLabel);
    crossoverLabel.setFont(juce::Font (10.0f, juce::Font::bold));
    // ...
    crossoverLabel.setText ("X-over (hz)", juce::dontSendNotification);
    crossoverLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    crossoverLabel.setJustificationType (juce::Justification::centred);
    // Azimuth Label
    addAndMakeVisible(&azimuthLabel);
    azimuthLabel.setFont(juce::Font (14.0f, juce::Font::bold));
    azimuthLabel.setText ("Azimuth (degrees)", juce::dontSendNotification);
    azimuthLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    azimuthLabel.setJustificationType (juce::Justification::centred);
    // Elevation Label
    addAndMakeVisible(&elevationLabel);
    elevationLabel.setFont(juce::Font (14.0f, juce::Font::bold));
    elevationLabel.setText ("Elevation (degrees)", juce::dontSendNotification);
    elevationLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    elevationLabel.setJustificationType (juce::Justification::centred);
    // Vertical Spread Label
    addAndMakeVisible(&verticalSpreadLabel);
    verticalSpreadLabel.setFont(juce::Font (14.0f, juce::Font::bold));
    verticalSpreadLabel.setText ("Vertical Spread (equal power)", juce::dontSendNotification);
    verticalSpreadLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    verticalSpreadLabel.setJustificationType (juce::Justification::centred);
    // Horizontal Spread Label
    addAndMakeVisible(&horizontalSpreadLabel);
    horizontalSpreadLabel.setFont(juce::Font (14.0f, juce::Font::bold));
    horizontalSpreadLabel.setText ("Horizontal Spread (equal power)", juce::dontSendNotification);
    horizontalSpreadLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    horizontalSpreadLabel.setJustificationType (juce::Justification::centred);
}

MDAPPannerAudioProcessorEditor::~MDAPPannerAudioProcessorEditor()
{
}

void MDAPPannerAudioProcessorEditor::sliderValueChanged (juce::Slider *slider)
{
    if (slider->getName() == "Azimuth" || slider->getName() == "Elevation")
    {
        // Elevation in range that does not change azimuth value ( < 90 || > 270 )
        if ((elevationKnob.getValue() < audioProcessor.verticalOffset || elevationKnob.getValue() > audioProcessor.verticalOffset + audioProcessor.verticalSpacing))
        {
            if (azimuthChanged)
            {
                // Flip azimuth value back to original
                if (azimuthKnob.getValue() <= 180)
                {
                    azimuthKnob.setValue(azimuthKnob.getValue() + audioProcessor.verticalSpacing);
                }
                else
                {
                    azimuthKnob.setValue(azimuthKnob.getValue() - audioProcessor.verticalSpacing);
                }
            }
            else
            {
                previousAzimuthValue = azimuthKnob.getValue();
            }
            azimuthChanged = false;
        }
        // Elevation in range that does change azimuth value ( > 90 || < 270 )
        else
        {
            if (!azimuthChanged)
            {
                // Change azimuth value to new value based on elevation
                if (previousAzimuthValue <= 180)
                {
                    azimuthKnob.setValue(previousAzimuthValue + audioProcessor.verticalSpacing);
                }
                else
                {
                    azimuthKnob.setValue(previousAzimuthValue - audioProcessor.verticalSpacing);
                }
                azimuthChanged = true;
            }
        }
    }
}
//==============================================================================
void MDAPPannerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Draw gradient
    juce::Colour gradientColor = juce::Colours::darkgrey;
    juce::ColourGradient gradient(juce::Colours::transparentBlack, getWidth() + 200, getHeight() + 200, juce::Colours::transparentBlack, -200, -200, false);
    gradient.addColour(0.5f, gradientColor);
    g.setGradientFill(gradient);
    g.fillRect(getX(), getY(), getWidth(), getHeight());
    // Draw surrounding rectangles
    g.setColour(juce::Colours::white);
    g.setOpacity(0.2f);
    g.drawRoundedRectangle(getWidth() - 385, 25, 311, 178, 3.0f, 1.0f);
    g.drawLine(getWidth() - 230, 25, getWidth() - 230, 202, 2.0f);
    g.drawRoundedRectangle(getWidth() - 74, 25, 60, 178, 3.0f, 1.0f);
    g.drawRoundedRectangle(getWidth() - 385, 203, 311, 100, 3.0f, 1.0f);
    g.drawRoundedRectangle(326, 203, 60, 100, 3.0f, 1.0f);
    // Draw version text
    g.setColour(juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText("Binaural MDAP Panner v1.0", getLocalBounds(), juce::Justification::centredTop, 1);
    g.setColour(juce::Colours::lightgrey);
    // Draw front head image
    headFrontImage = juce::ImageCache::getFromMemory(BinaryData::FronHead_png, BinaryData::FronHead_pngSize).rescaled(47, 70, juce::Graphics::highResamplingQuality);
    g.drawImageAt(headFrontImage, azimuthKnob.getX() + 57, azimuthKnob.getY() + 53, true);
    // Draw side head image
    headSideImage = juce::ImageCache::getFromMemory(BinaryData::SideHead_png, BinaryData::SideHead_pngSize).rescaled(47, 70, juce::Graphics::highResamplingQuality);;
    g.drawImageAt(headSideImage, elevationKnob.getX() + 57, elevationKnob.getY() + 53, true);
}

void MDAPPannerAudioProcessorEditor::resized()
{
    azimuthKnob.setBounds(getWidth() - 390, getHeight() - 280, 160, 160);
    elevationKnob.setBounds(getWidth() - 230, getHeight() - 280, 160, 160);
    horizontalSpreadKnob.setBounds(getWidth() - 380, getHeight() - 120, 290, 75);
    verticalSpreadKnob.setBounds(getWidth() - 380, getHeight() - 77, 290, 75);
    gainSlider.setBounds(getWidth() - 80, getHeight() - 280, 75, 160);
    crossoverKnob.setBounds(getWidth() - 85, getHeight() - 100, 80, 80);
    // Labels
    gainLabel.setBounds(gainSlider.getX() + 5, gainSlider.getY() + gainSlider.getHeight() - 8, 60, 20);
    crossoverLabel.setBounds(crossoverKnob.getX() - 10, crossoverKnob.getY() + crossoverKnob.getHeight() - 11, 100, 25);
    azimuthLabel.setBounds(azimuthKnob.getX() + 30, azimuthKnob.getY() + azimuthKnob.getHeight() - 5, 100, 15);
    elevationLabel.setBounds(elevationKnob.getX() + 30, elevationKnob.getY() + elevationKnob.getHeight() - 5, 100, 15);
    horizontalSpreadLabel.setBounds(horizontalSpreadKnob.getX() + 37, horizontalSpreadKnob.getY() + 48, 240, 15);
    verticalSpreadLabel.setBounds(verticalSpreadKnob.getX() + 29, verticalSpreadKnob.getY() + 48, 240, 15);
}
