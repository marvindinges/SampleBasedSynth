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
    getLookAndFeel().setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::white.withAlpha(0.0f));
    //loadSampleOneButton.onClick = [&]() { audioProcessor.loadSample(); };
    //addAndMakeVisible(loadSampleOneButton);
    // 
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    lowPassSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lowPassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    lowPassSlider.setColour(juce::Slider::thumbColourId, juce::Colours::purple.brighter(0.9f));
    addAndMakeVisible(lowPassSlider);
    lowPassSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "LowPass Freq", lowPassSlider);

    lowQ_Slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lowQ_Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    lowQ_Slider.setColour(juce::Slider::thumbColourId, juce::Colours::purple.brighter(0.9f));
    addAndMakeVisible(lowQ_Slider);
    lowQ_SA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "LowQ", lowQ_Slider);

    highPassSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    highPassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    highPassSlider.setColour(juce::Slider::thumbColourId, juce::Colours::purple.brighter(0.9f));
    addAndMakeVisible(highPassSlider);
    highpassSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "HighPass Freq", highPassSlider);

    highQ_Slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    highQ_Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    highQ_Slider.setColour(juce::Slider::thumbColourId, juce::Colours::purple.brighter(0.9f));
    addAndMakeVisible(highQ_Slider);
    highQ_SA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "HighQ", highQ_Slider);

    addAndMakeVisible(slopeBox);
    slopeCBA = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "Slope", slopeBox);

    attackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    attackSlider.setColour(juce::Slider::thumbColourId, juce::Colours::yellow.brighter(0.9f));
    addAndMakeVisible(attackSlider);
    attackSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Attack", attackSlider);
    
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    decaySlider.setColour(juce::Slider::thumbColourId, juce::Colours::yellow.brighter(0.9f));
    addAndMakeVisible(decaySlider);
    decaySA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Decay", decaySlider);

    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    sustainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::yellow.brighter(0.9f));
    addAndMakeVisible(sustainSlider);
    sustainSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Sustain", sustainSlider);

    relaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    relaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    relaseSlider.setColour(juce::Slider::thumbColourId, juce::Colours::yellow.brighter(0.9f));
    addAndMakeVisible(relaseSlider);
    relaseSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Relase", relaseSlider);

    curveSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    curveSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    curveSlider.setColour(juce::Slider::thumbColourId, juce::Colours::yellow.brighter(0.9f));
    curveSlider.setColour(juce::Slider::trackColourId, juce::Colours::white.withAlpha(0.0f));
    addAndMakeVisible(curveSlider);
    curveSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Curve", curveSlider);



    masterGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    masterGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    masterGainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::lightgreen);
    addAndMakeVisible(masterGainSlider);
    masterGainSA = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "Gain", masterGainSlider);

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
    
    g.drawRoundedRectangle(sampleRec, 15, 2);
    g.drawRoundedRectangle(outputRec, 15, 2);
    g.drawRoundedRectangle(filterRec, 15, 2);
    g.drawRoundedRectangle(envelopeRec, 15, 2);
    g.drawRoundedRectangle(modRec, 15, 2);
    g.drawRoundedRectangle(effectRec, 15, 2);
    
    g.setColour(juce::Colours::lightblue);

    g.fillRoundedRectangle(sampleOneBackground, 15);
    g.fillRoundedRectangle(sampleTwoBackground, 15);

    g.setColour(juce::Colours::white);
    g.setFont(28.0f);

    if (!loadedSampleOne) { g.drawFittedText("drag&drop sample here!", sampleOneBackground.getSmallestIntegerContainer(), juce::Justification::centred, 1); }
    if (!loadedSampleTwo) { g.drawFittedText("drag&drop sample here!", sampleTwoBackground.getSmallestIntegerContainer(), juce::Justification::centred, 1); }

    g.setFont(lowPassSlider.getBounds().getWidth() / 5);

    g.drawFittedText("LP", lowPassSlider.getBounds().withBottom(lowPassSlider.getBottom() - lowPassSlider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("Q", lowQ_Slider.getBounds().withBottom(lowQ_Slider.getBottom() - lowQ_Slider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("HP", highPassSlider.getBounds().withBottom(highPassSlider.getBottom() - highPassSlider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("Q", highQ_Slider.getBounds().withBottom(highQ_Slider.getBottom() - highQ_Slider.getTextBoxHeight()), juce::Justification::centred, 1);

    g.drawFittedText("A", attackSlider.getBounds().withBottom(attackSlider.getBottom()-attackSlider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("D", decaySlider.getBounds().withBottom(decaySlider.getBottom() - decaySlider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("S", sustainSlider.getBounds().withBottom(sustainSlider.getBottom() - sustainSlider.getTextBoxHeight()), juce::Justification::centred, 1);
    g.drawFittedText("R", relaseSlider.getBounds().withBottom(relaseSlider.getBottom() - relaseSlider.getTextBoxHeight()), juce::Justification::centred, 1);

    g.setFont(attackSlider.getBounds().getWidth() / 6);
    g.drawFittedText("lin", curveSlider.getBounds().withHeight(curveSlider.getHeight() + 15), juce::Justification::centredBottom, 1);
    g.drawFittedText("log", sustainSlider.getBounds().withHeight(curveSlider.getHeight() + sustainSlider.getHeight() + 25).withLeft(sustainSlider.getX() + 25), juce::Justification::bottomLeft, 1);
    g.drawFittedText("exp", relaseSlider.getBounds().withHeight(curveSlider.getHeight() + sustainSlider.getHeight() + 25).translated(-25, 0), juce::Justification::bottomRight, 1);


    g.setFont(masterGainSlider.getBounds().getWidth() / 5);
    g.drawFittedText("Gain", masterGainSlider.getBounds(), juce::Justification::centred, 1);

    //juce::Path p;
    //p.addEllipse(outputRec.getCentreX(), outputRec.getCentreY() + outputRec.getHeight() * 0.25f, 50, 50);
    //g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    //g.fillPath(p);

    
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
    sampleRec = juce::Rectangle<float>(10, 10, getWidth() * 0.90 - 10, getHeight() * 0.35 - 10);
    outputRec = juce::Rectangle<float>(sampleRec.getRight() + 10, 10, getWidth() * 0.10 - 20, getHeight() - 20);
    filterRec = juce::Rectangle<float>(10, sampleRec.getBottom() + 10, getWidth() * 0.20 - 10, getHeight() * 0.45 - 10);
    envelopeRec = juce::Rectangle<float>(filterRec.getRight() + 10, sampleRec.getBottom() + 10, getWidth() * 0.20 - 10, getHeight() * 0.45 - 10);
    modRec = juce::Rectangle<float>(envelopeRec.getRight() + 10, sampleRec.getBottom() + 10, getWidth() * 0.50 - 10, getHeight() * 0.45 - 10);
    effectRec = juce::Rectangle<float>(10, filterRec.getBottom() + 10, getWidth() * 0.90 - 10, getHeight() * 0.20 - 20);

    zoneOne = juce::Rectangle<float>(10, 10, getWidth() * 0.45 - 10, getHeight() * 0.175 - 10);
    zoneTwo = juce::Rectangle<float>(10, zoneOne.getBottom() + 10, getWidth() * 0.45 - 10, getHeight() * 0.175 - 10);

    sampleOneBackground = juce::Rectangle<float>(15, 15, zoneOne.getWidth() - 5, zoneOne.getHeight() - 5);
    sampleTwoBackground = juce::Rectangle<float>(zoneTwo.getX() + 5, zoneTwo.getY(), zoneTwo.getWidth() - 5, zoneTwo.getHeight() - 5);

    lowPassSlider.setBounds(filterRec.getX() + 5, filterRec.getY() + 5, filterRec.getWidth() / 2 - 10, filterRec.getHeight() * 0.40 - 20);
    lowQ_Slider.setBounds(filterRec.getX() + 5 + filterRec.getWidth() / 2, filterRec.getY() + 5, filterRec.getWidth() / 2 - 10, filterRec.getHeight() * 0.40 - 20);
    highPassSlider.setBounds(filterRec.getX() + 5, lowPassSlider.getBottom() + 10, filterRec.getWidth() / 2 - 10, filterRec.getHeight() * 0.40 - 20);
    highQ_Slider.setBounds(filterRec.getX() + 5 + filterRec.getWidth() / 2, lowPassSlider.getBottom() + 10, filterRec.getWidth() / 2 - 10, filterRec.getHeight() * 0.40 - 20);
    slopeBox.setBounds(filterRec.getX() + 20, highPassSlider.getBottom() + 20, filterRec.getWidth() - 40, filterRec.getHeight() * 0.20 - 20);

    attackSlider.setBounds(envelopeRec.getX() + 5, envelopeRec.getY() + 5, envelopeRec.getWidth() / 2 - 10, envelopeRec.getHeight() * 0.40 - 20);
    decaySlider.setBounds(envelopeRec.getX() + 5 + envelopeRec.getWidth() / 2, envelopeRec.getY() + 5, envelopeRec.getWidth() / 2 - 10, envelopeRec.getHeight() * 0.40 - 20);
    sustainSlider.setBounds(envelopeRec.getX() + 5, attackSlider.getBottom() + 10, envelopeRec.getWidth() /2 - 10, envelopeRec.getHeight() * 0.40 - 20);
    relaseSlider.setBounds(envelopeRec.getX() + 5 + envelopeRec.getWidth() / 2, attackSlider.getBottom() + 10, envelopeRec.getWidth() / 2 - 10, envelopeRec.getHeight() * 0.40 - 20);
    curveSlider.setBounds(envelopeRec.getX() + 20, sustainSlider.getBottom() + 10, envelopeRec.getWidth() - 40, envelopeRec.getHeight() * 0.20 - 20);

    levelMeterLeft.setBounds(outputRec.getX() + 5, outputRec.getY() + 5, (outputRec.getWidth() / 2) - 10, getHeight() * 0.80 - 20);
    levelMeterRight.setBounds(outputRec.getX() + 5 + outputRec.getWidth() / 2, outputRec.getY() + 5, outputRec.getWidth() / 2 - 10, getHeight() * 0.80 - 20);
    masterGainSlider.setBounds(outputRec.getX() + 5, levelMeterLeft.getBottom() + 10, outputRec.getWidth() - 10, outputRec.getHeight() * 0.20 - 20);

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
