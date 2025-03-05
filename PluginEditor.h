#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
class TransformerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TransformerAudioProcessorEditor (TransformerAudioProcessor&);
    ~TransformerAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    TransformerAudioProcessor& audioProcessor;
    
    // Core controls
    juce::Slider driveSlider;
    juce::Slider outputGainSlider;
    
    // Harmonic controls
    juce::Slider evenHarmonicsSlider;
    juce::Slider oddHarmonicsSlider;
    
    // Transformer physics controls
    juce::Slider hysteresisSlider;
    juce::Slider asymmetrySlider;
    
    // Labels
    juce::Label driveLabel;
    juce::Label outputGainLabel;
    juce::Label evenHarmonicsLabel;
    juce::Label oddHarmonicsLabel;
    juce::Label hysteresisLabel;
    juce::Label asymmetryLabel;
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> evenHarmonicsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oddHarmonicsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hysteresisAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> asymmetryAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransformerAudioProcessorEditor)
};
