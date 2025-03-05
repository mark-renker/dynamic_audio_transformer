#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

TransformerAudioProcessorEditor::TransformerAudioProcessorEditor (TransformerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (600, 400);
    
    // Initialize all sliders with rotary style
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& labelText)
    {
        slider.setSliderStyle(juce::Slider::Rotary);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
        slider.setColour(juce::Slider::thumbColourId, juce::Colours::orangered);
        
        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::Font(14.0f, juce::Font::bold));
        // Position the label above the slider but with a gap
        label.attachToComponent(&slider, false);
        label.setTopLeftPosition(label.getX(), label.getY() - 5);
        
        addAndMakeVisible(slider);
        addAndMakeVisible(label);
    };
    
    // Core controls
    setupSlider(driveSlider, driveLabel, "Drive");
    setupSlider(outputGainSlider, outputGainLabel, "Output");
    
    // Harmonic controls
    setupSlider(evenHarmonicsSlider, evenHarmonicsLabel, "Even Harm");
    setupSlider(oddHarmonicsSlider, oddHarmonicsLabel, "Odd Harm");
    
    // Transformer physics controls
    setupSlider(hysteresisSlider, hysteresisLabel, "Hysteresis");
    setupSlider(asymmetrySlider, asymmetryLabel, "Asymmetry");
    
    // Parameter attachments
    driveAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.parameters, "drive", driveSlider));
    
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.parameters, "outputGain", outputGainSlider));
    
    evenHarmonicsAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.parameters, "evenHarmonics", evenHarmonicsSlider));
    
    oddHarmonicsAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.parameters, "oddHarmonics", oddHarmonicsSlider));
    
    hysteresisAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.parameters, "hysteresis", hysteresisSlider));
    
    asymmetryAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.parameters, "asymmetry", asymmetrySlider));
}

TransformerAudioProcessorEditor::~TransformerAudioProcessorEditor() { }

void TransformerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill background with a dark gradient
    juce::ColourGradient gradient(juce::Colour(30, 30, 30), 0, 0,
                                  juce::Colour(10, 10, 10), getWidth(), getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Draw title
    g.setColour(juce::Colours::orange);
    g.setFont(24.0f);
    g.drawText("Lovely Transformer", getLocalBounds().reduced(10).removeFromTop(30), 
              juce::Justification::centred, true);
    
    // Draw section dividers
    g.setColour(juce::Colours::grey);
    g.drawLine(getWidth() / 2.0f, 60, getWidth() / 2.0f, getHeight() - 20, 1.0f);
    g.drawLine(0, getHeight() / 2.0f, getWidth(), getHeight() / 2.0f, 1.0f);
    
    // Section labels
    g.setFont(16.0f);
    g.setColour(juce::Colours::white);
    g.drawText("I/O", 10, 40, getWidth() / 2 - 20, 20, juce::Justification::left);
    g.drawText("Harmonics", getWidth() / 2 + 10, 40, getWidth() / 2 - 20, 20, juce::Justification::left);
    g.drawText("Transformer", 10, getHeight() / 2 - 10, getWidth() - 20, 20, juce::Justification::left);
}

void TransformerAudioProcessorEditor::resized()
{
    // Define areas for each section
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(40); // Space for title
    
    auto topHalf = bounds.removeFromTop(bounds.getHeight() / 2 - 10);
    auto bottomHalf = bounds;
    
    auto leftColumn = topHalf.removeFromLeft(topHalf.getWidth() / 2);
    auto rightColumn = topHalf;
    
    // Position sliders
    int sliderSize = 100;
    
    // Core controls in top left
    driveSlider.setBounds(leftColumn.withSizeKeepingCentre(sliderSize, sliderSize));
    outputGainSlider.setBounds(rightColumn.withSizeKeepingCentre(sliderSize, sliderSize));
    
    // Harmonic controls in top right - ensure they're spaced further apart
    auto rightTopArea = topHalf.translated(topHalf.getWidth() / 2, 0);
    
    auto rightLeftThird = rightTopArea.removeFromLeft(rightTopArea.getWidth() / 2);
    auto rightRightThird = rightTopArea;
    
    // Position with more space between them
    evenHarmonicsSlider.setBounds(rightLeftThird.reduced(20).withSizeKeepingCentre(sliderSize, sliderSize));
    oddHarmonicsSlider.setBounds(rightRightThird.reduced(20).withSizeKeepingCentre(sliderSize, sliderSize));
    
    // Physics controls in bottom
    auto physicsLeft = bottomHalf.removeFromLeft(bottomHalf.getWidth() / 2);
    auto physicsRight = bottomHalf;
    
    hysteresisSlider.setBounds(physicsLeft.withSizeKeepingCentre(sliderSize, sliderSize));
    asymmetrySlider.setBounds(physicsRight.withSizeKeepingCentre(sliderSize, sliderSize));
}
