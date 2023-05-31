/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LevelMeter.h"

//==============================================================================
/**
*/
class SampleBasedSynthAudioProcessorEditor : public juce::AudioProcessorEditor,
                                                public juce::FileDragAndDropTarget,
                                                public juce::Timer
{
public:
    SampleBasedSynthAudioProcessorEditor (SampleBasedSynthAudioProcessor&);
    ~SampleBasedSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void timerCallback() override;

private:
    juce::Rectangle<float> sampleRec, outputRec, filterRec, envelopeRec, modRec, effectRec;
    juce::Rectangle<float> zoneOne, zoneTwo;

    std::vector<float> pointsOneLeft, pointsOneRight;
    std::vector<float> pointsTwoLeft, pointsTwoRight;

    bool loadedSampleOne{ false };
    bool loadedSampleTwo{ false };

    juce::TextButton LoopSampleOne{ "Loop" };
    juce::TextButton LoopSampleTwo{ "Loop" };

    Gui::LevelMeter levelMeterLeft, levelMeterRight;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SampleBasedSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleBasedSynthAudioProcessorEditor)
};
