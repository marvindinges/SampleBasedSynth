/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SampleBasedSynthAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                              public juce::FileDragAndDropTarget
{
public:
    SampleBasedSynthAudioProcessorEditor (SampleBasedSynthAudioProcessor&);
    ~SampleBasedSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:
    juce::Rectangle<float> sampleRec;
    juce::Rectangle<float> outputRec;
    juce::Rectangle<float> filterRec;
    juce::Rectangle<float> envelopeRec;
    juce::Rectangle<float> modRec;
    juce::Rectangle<float> effectRec;
    juce::Rectangle<float> zoneOne;
    juce::Rectangle<float> zoneTwo;


    std::vector<float> pointsOneLeft;
    std::vector<float> pointsOneRight;

    std::vector<float> pointsTwoLeft;
    std::vector<float> pointsTwoRight;

    bool loadedSampleOne{ false };
    bool loadedSampleTwo{ false };

    juce::TextButton LoopSampleOne{ "Loop" };
    juce::TextButton LoopSampleTwo{ "Loop" };

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SampleBasedSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleBasedSynthAudioProcessorEditor)
};
