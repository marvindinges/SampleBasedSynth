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
    //loadSampleOneButton.onClick = [&]() { audioProcessor.loadSample(); };
    //addAndMakeVisible(loadSampleOneButton);
    // 
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

    g.setColour(juce::Colours::slategrey);

#pragma region Rectangles
    sampleRec = juce::Rectangle<float>(10, 10, getWidth() * 0.90 - 10, getHeight() * 0.35 - 10);
    g.drawRoundedRectangle(sampleRec, 15, 2);

    outputRec = juce::Rectangle<float>(sampleRec.getRight() + 10, 10, getWidth() * 0.10 - 20, getHeight() - 20);
    g.drawRoundedRectangle(outputRec, 15, 2);

    filterRec = juce::Rectangle<float>(10, sampleRec.getBottom() + 10, getWidth() * 0.20 - 10, getHeight() * 0.45 - 10);
    g.drawRoundedRectangle(filterRec, 15, 2);

    envelopeRec = juce::Rectangle<float>(filterRec.getRight() + 10, sampleRec.getBottom() + 10, getWidth() * 0.20 - 10, getHeight() * 0.45 - 10);
    g.drawRoundedRectangle(envelopeRec, 15, 2);

    modRec = juce::Rectangle<float>(envelopeRec.getRight() + 10, sampleRec.getBottom() + 10, getWidth() * 0.50 - 10, getHeight() * 0.45 - 10);
    g.drawRoundedRectangle(modRec, 15, 2);

    effectRec = juce::Rectangle<float>(10, filterRec.getBottom() + 10, getWidth() * 0.90 - 10, getHeight() * 0.20 - 20);
    g.drawRoundedRectangle(effectRec, 15, 2);

    zoneOne = juce::Rectangle<float>(10, 10, getWidth() * 0.40 - 10, getHeight() * 0.175 - 10);
    zoneTwo = juce::Rectangle<float>(10, zoneOne.getBottom() + 10, getWidth() * 0.40 - 10, getHeight() * 0.175 - 10);

    g.setColour(juce::Colours::pink);
    g.drawRect(zoneOne);
    g.drawRect(zoneTwo);

#pragma endregion

   
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SampleBasedSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //loadSampleOneButton.setBounds(10, 10, 50, 50);
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
            if (zoneOne.contains(x,y))
            {
                audioProcessor.loadSampleOne(file);
            }
            else if (zoneTwo.contains(x, y))
            {
                audioProcessor.loadSampleTwo(file);
            }
        }
    }
}