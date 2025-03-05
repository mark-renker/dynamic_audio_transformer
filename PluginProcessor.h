#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

// Preisach hysteresis model for transformer saturation
class PreisachTransformerModel
{
public:
    PreisachTransformerModel() 
    {
        // Initialize with default values
        reset();
    }
    
    void reset()
    {
        // Clear history and reset state
        mPreviousInput = 0.0f;
        mHistoryState.clear();
        
        // Initialize 10 history points for hysteresis (can be adjusted)
        for (int i = 0; i < 10; ++i)
            mHistoryState.push_back(0.0f);
    }
    
    void setDensityParams(float width, float skew)
    {
        mWidth = width;
        mSkew = skew;
    }
    
    void setHarmonics(float evenHarmonics, float oddHarmonics)
    {
        mEvenHarmonics = evenHarmonics;
        mOddHarmonics = oddHarmonics;
    }
    
    float process(float input, float drive)
    {
        // Apply drive and input conditioning
        float x = input * drive;
        
        // Preisach hysteresis model calculation
        float output = 0.0f;
        
        // Update history
        mHistoryState.pop_back();
        mHistoryState.insert(mHistoryState.begin(), x);
        
        // Calculate weighted sum of history to simulate hysteresis
        float weightSum = 0.0f;
        for (size_t i = 0; i < mHistoryState.size(); ++i)
        {
            float weight = std::exp(-static_cast<float>(i) * mWidth);
            weightSum += weight;
            output += mHistoryState[i] * weight;
        }
        
        // Normalize
        if (weightSum > 0.0f)
            output /= weightSum;
        
        // Apply nonlinear shaping for harmonics
        float sign = (output >= 0.0f) ? 1.0f : -1.0f;
        float absOutput = std::abs(output);
        
        // Tanh for odd harmonics, squared term for even harmonics
        float oddTerm = std::tanh(absOutput * mOddHarmonics);
        float evenTerm = absOutput * absOutput * sign * mEvenHarmonics;
        
        // Combine and apply skew for asymmetrical distortion
        float shaped = (oddTerm + evenTerm) * (1.0f + mSkew * sign);
        
        // Store for next iteration
        mPreviousInput = input;
        
        return shaped;
    }
    
private:
    std::vector<float> mHistoryState;
    float mPreviousInput = 0.0f;
    float mWidth = 0.2f;      // Controls how quickly past states decay
    float mSkew = 0.1f;       // Controls asymmetry of saturation
    float mEvenHarmonics = 0.3f; // Controls amount of even harmonics
    float mOddHarmonics = 1.0f;  // Controls amount of odd harmonics
};

class TransformerAudioProcessor  : public juce::AudioProcessor
{
public:
    TransformerAudioProcessor();
    ~TransformerAudioProcessor() override;
    
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState parameters;

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                  juce::dsp::IIR::Coefficients<float>> lowPassFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                  juce::dsp::IIR::Coefficients<float>> highPassFilter;
    
    PreisachTransformerModel transformerModelL;
    PreisachTransformerModel transformerModelR;
    
    double sampleRate = 44100.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransformerAudioProcessor)
};
