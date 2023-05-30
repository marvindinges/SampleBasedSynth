/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SampleBasedSynthAudioProcessorEditor::SampleBasedSynthAudioProcessorEditor (SampleBasedSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    loadSampleOneButton.onClick = [&]() { audioProcessor.loadSample(); };
    addAndMakeVisible(loadSampleOneButton);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1500, 750);
}

SampleBasedSynthAudioProcessorEditor::~SampleBasedSynthAudioProcessorEditor()
{
}

//==============================================================================
void SampleBasedSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SampleBasedSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    loadSampleOneButton.setBounds(10, 10, 50, 50);
}

bool SampleBasedSynthAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto file : files)
    {
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif"))
        {
            return true;
        }
    }
    return false;
}

void SampleBasedSynthAudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            if (x <= 70 && y <= 70)
            {
                audioProcessor.loadSample(file);
            }
        }
    }
}