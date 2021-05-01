/*
  ==============================================================================

    SliderReverse.h
    Created: 20 Jan 2021 12:41:27am
    Author:  Jacob Wolfe

  ==============================================================================
*/

#pragma once

//==============================================================================
/*
*/
#ifndef SLIDER_REVERSE_H
#define SLIDER_REVERSE_H
class Slider_reverse : public JUCE_NAMESPACE::Slider
{
public:
    Slider_reverse (const juce::String& componentName): JUCE_NAMESPACE::Slider(componentName) {};
    ~Slider_reverse() {};
    double proportionOfLengthToValue (double proportion) {   return JUCE_NAMESPACE::Slider::proportionOfLengthToValue(1.0f-proportion);};
    double valueToProportionOfLength (double value) {   return 1.0f-(JUCE_NAMESPACE::Slider::valueToProportionOfLength(value)); };
};
#endif // SLIDER_REVERSE_H
