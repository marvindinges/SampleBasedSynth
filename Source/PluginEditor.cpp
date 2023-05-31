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
    addAndMakeVisible(levelMeterLeft);
    addAndMakeVisible(levelMeterRight);

    setSize (1500, 750);
    startTimerHz(24);
}

SampleBasedSynthAudioProcessorEditor::~SampleBasedSynthAudioProcessorEditor()
{
}

//==============================================================================
void SampleBasedSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::slategrey);

#pragma region DrawBackgroundLayout
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

    zoneOne = juce::Rectangle<float>(10, 10, getWidth() * 0.45 - 10, getHeight() * 0.175 - 10);
    zoneTwo = juce::Rectangle<float>(10, zoneOne.getBottom() + 10, getWidth() * 0.45 - 10, getHeight() * 0.175 - 10);

    g.setColour(juce::Colours::lightblue);
    juce::Rectangle<float> sampleOneBackground(15, 15, zoneOne.getWidth() - 5, zoneOne.getHeight() - 5);
    g.fillRoundedRectangle(sampleOneBackground, 15);
    
    juce::Rectangle<float> sampleTwoBackground(zoneTwo.getX() + 5, zoneTwo.getY(), zoneTwo.getWidth() - 5, zoneTwo.getHeight() - 5);
    g.fillRoundedRectangle(sampleTwoBackground, 15);

    g.setColour(juce::Colours::white);
    g.setFont(36.0f);

    if (!loadedSampleOne) { g.drawFittedText("Drag&Drop Sample", sampleOneBackground.getSmallestIntegerContainer(), juce::Justification::centred, 1); }
    if (!loadedSampleTwo) { g.drawFittedText("Drag&Drop Sample", sampleTwoBackground.getSmallestIntegerContainer(), juce::Justification::centred, 1); }

    levelMeterLeft.setBounds(outputRec.getX() + 5, outputRec.getY() + 5, (outputRec.getWidth() / 2) - 10, getHeight() * 0.80 - 20);
    levelMeterRight.setBounds(outputRec.getX() + 5 + outputRec.getWidth() / 2, outputRec.getY() + 5, outputRec.getWidth() / 2 - 10, getHeight() * 0.80 - 20);

    //juce::Path p;
    //p.addEllipse(outputRec.getCentreX(), outputRec.getCentreY() + outputRec.getHeight() * 0.25f, 50, 50);
    //g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    //g.fillPath(p);

#pragma endregion
    
#pragma region DrawWaveformOne
    if (loadedSampleOne)
    {
        pointsOneLeft.clear();
        pointsOneRight.clear();
        auto waveform = audioProcessor.getMySampleBufferOne();
        auto ratio = waveform.getNumSamples() / (zoneOne.getWidth() - 20);
        auto bufferLeft = waveform.getReadPointer(0);
        auto bufferRight = waveform.getReadPointer(1);

        for (int i = 0; i < waveform.getNumSamples(); i += ratio)
        {
            pointsOneLeft.push_back(bufferLeft[i]);
            pointsOneRight.push_back(bufferRight[i]);
        }

        juce::Path pLeft;
        juce::Path pRight;

        auto startX = zoneOne.getX() + 10;
        auto startYLeft = zoneOne.getHeight() * 0.25f + 10;
        auto startYRight = zoneOne.getHeight() * 0.75f + 10;
        auto minLeft = zoneOne.getCentreY();    auto maxLeft = zoneOne.getY();
        auto minRight = zoneOne.getBottom();    auto maxRight = zoneOne.getCentreY();

        pLeft.startNewSubPath(startX, startYLeft);
        pRight.startNewSubPath(startX, startYRight);

        for (int i = 0; i < pointsOneLeft.size(); i++)
        {
            auto pointLeft = juce::jmap<float>(pointsOneLeft[i], -1.0f, 1.0f, minLeft, maxLeft);
            auto pointRight = juce::jmap<float>(pointsOneRight[i], -1.0f, 1.0f, minRight, maxRight);
            pLeft.lineTo(startX + i, pointLeft);
            pRight.lineTo(startX + i, pointRight);
        }
        g.strokePath(pLeft, juce::PathStrokeType(2));
        g.strokePath(pRight, juce::PathStrokeType(2));
    }
#pragma endregion

#pragma region DrawWaveformTwo
    if (loadedSampleTwo)
    {
        pointsTwoLeft.clear();
        pointsTwoRight.clear();
        auto waveform = audioProcessor.getMySampleBufferTwo();
        auto ratio = waveform.getNumSamples() / (zoneTwo.getWidth() - 20);
        auto bufferLeft = waveform.getReadPointer(0);
        auto bufferRight = waveform.getReadPointer(1);

        for (int i = 0; i < waveform.getNumSamples(); i += ratio)
        {
            pointsTwoLeft.push_back(bufferLeft[i]);
            pointsTwoRight.push_back(bufferRight[i]);
        }

        juce::Path pLeft;
        juce::Path pRight;

        auto startX = zoneTwo.getX() + 10;
        auto startYLeft = zoneTwo.getHeight() * 0.25f + zoneOne.getHeight() + 20;
        auto startYRight = zoneTwo.getHeight() * 0.75f + zoneOne.getHeight() + 20;
        auto minLeft = zoneTwo.getCentreY();    auto maxLeft = zoneTwo.getY();
        auto minRight = zoneTwo.getBottom();    auto maxRight = zoneTwo.getCentreY();

        pLeft.startNewSubPath(startX, startYLeft);
        pRight.startNewSubPath(startX, startYRight);

        for (int i = 0; i < pointsTwoLeft.size(); i++)
        {
            auto pointLeft = juce::jmap<float>(pointsTwoLeft[i], -1.0f, 1.0f, minLeft, maxLeft);
            auto pointRight = juce::jmap<float>(pointsTwoRight[i], -1.0f, 1.0f, minRight, maxRight);
            pLeft.lineTo(startX + i, pointLeft);
            pRight.lineTo(startX + i, pointRight);
        }
        g.strokePath(pLeft, juce::PathStrokeType(2));
        g.strokePath(pRight, juce::PathStrokeType(2));
    }
#pragma endregion
 
}

void SampleBasedSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //loadSampleOneButton.setBounds(10, 10, 50, 50);
    //levelMeterLeft.setBounds(outputRec.getX(), outputRec.getY(), outputRec.getWidth() / 2, outputRec.getHeight());
    //levelMeterRight.setBounds(outputRec.getX() + outputRec.getWidth() / 2, outputRec.getY(), outputRec.getWidth() / 2, outputRec.getHeight());
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
            if (zoneOne.contains(x, y))
            {
                loadedSampleOne = true;
                audioProcessor.loadSampleOne(file);
            }
            else if (zoneTwo.contains(x, y))
            {
                loadedSampleTwo = true;
                audioProcessor.loadSampleTwo(file);
            }
        }
    }
    repaint();
}

void SampleBasedSynthAudioProcessorEditor::timerCallback()
{
    levelMeterLeft.setLevel(audioProcessor.getRmsValueLeft());
    levelMeterRight.setLevel(audioProcessor.getRmsValueRight());
    levelMeterLeft.repaint();
    levelMeterRight.repaint();

}
