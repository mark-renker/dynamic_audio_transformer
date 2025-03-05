#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

TransformerAudioProcessor::TransformerAudioProcessor()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "Parameters",
      {
          std::make_unique<juce::AudioParameterFloat>(
              "drive",                     // parameterID
              "Drive",                     // parameter name
              juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f), // range
              1.0f                         // default value
          ),
          std::make_unique<juce::AudioParameterFloat>(
              "outputGain",                // parameterID
              "Output Gain",               // parameter name
              juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), // range
              1.0f                         // default value
          ),
          std::make_unique<juce::AudioParameterFloat>(
              "evenHarmonics",             // parameterID
              "Even Harmonics",            // parameter name
              juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), // range
              0.3f                         // default value
          ),
          std::make_unique<juce::AudioParameterFloat>(
              "oddHarmonics",              // parameterID
              "Odd Harmonics",             // parameter name
              juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), // range
              1.0f                         // default value
          ),
          std::make_unique<juce::AudioParameterFloat>(
              "hysteresis",                // parameterID
              "Hysteresis",                // parameter name
              juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), // range
              0.2f                         // default value
          ),
          std::make_unique<juce::AudioParameterFloat>(
              "asymmetry",                 // parameterID
              "Asymmetry",                 // parameter name
              juce::NormalisableRange<float>(-0.5f, 0.5f, 0.01f), // range
              0.1f                         // default value
          )
      })
{
    // Initialize transformer models with default settings
    transformerModelL.setDensityParams(0.2f, 0.1f);
    transformerModelL.setHarmonics(0.3f, 1.0f);
    
    transformerModelR.setDensityParams(0.2f, 0.1f);
    transformerModelR.setHarmonics(0.3f, 1.0f);
}

TransformerAudioProcessor::~TransformerAudioProcessor()
{
}

const juce::String TransformerAudioProcessor::getName() const
{
    return "Lovely Transformer";
}

bool TransformerAudioProcessor::acceptsMidi() const { return false; }
bool TransformerAudioProcessor::producesMidi() const { return false; }
bool TransformerAudioProcessor::isMidiEffect() const { return false; }
double TransformerAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int TransformerAudioProcessor::getNumPrograms() { return 1; }
int TransformerAudioProcessor::getCurrentProgram() { return 0; }
void TransformerAudioProcessor::setCurrentProgram(int index) { }
const juce::String TransformerAudioProcessor::getProgramName(int index) { return {}; }
void TransformerAudioProcessor::changeProgramName(int index, const juce::String& newName) { }

void TransformerAudioProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = newSampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    // Setup multiband processing for frequency-dependent transformer behavior
    lowPassFilter.reset();
    highPassFilter.reset();
    
    // Crossover frequency at 1kHz - typical for transformer frequency response changes
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(newSampleRate, 1000.0f);
    *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(newSampleRate, 1000.0f);
    
    lowPassFilter.prepare(spec);
    highPassFilter.prepare(spec);
    
    // Reset transformer models
    transformerModelL.reset();
    transformerModelR.reset();
}

void TransformerAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TransformerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

void TransformerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, 
                                         juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();
    
    // Clear any output channels beyond the input channels
    for (int channel = totalNumInputChannels; channel < getTotalNumOutputChannels(); ++channel)
        buffer.clear(channel, 0, numSamples);
    
    // Get parameters
    float drive = parameters.getRawParameterValue("drive")->load();
    float outputGain = parameters.getRawParameterValue("outputGain")->load();
    float evenHarmonics = parameters.getRawParameterValue("evenHarmonics")->load();
    float oddHarmonics = parameters.getRawParameterValue("oddHarmonics")->load();
    float hysteresis = parameters.getRawParameterValue("hysteresis")->load();
    float asymmetry = parameters.getRawParameterValue("asymmetry")->load();
    
    // Update transformer model parameters
    transformerModelL.setDensityParams(hysteresis, asymmetry);
    transformerModelL.setHarmonics(evenHarmonics, oddHarmonics);
    transformerModelR.setDensityParams(hysteresis, asymmetry);
    transformerModelR.setHarmonics(evenHarmonics, oddHarmonics);
    
    // Create temporary buffers for multiband processing
    juce::AudioBuffer<float> lowFreqBuffer(buffer.getNumChannels(), numSamples);
    juce::AudioBuffer<float> highFreqBuffer(buffer.getNumChannels(), numSamples);
    
    // Copy input to both bands
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        lowFreqBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
        highFreqBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
    }
    
    // Process crossover filters
    juce::dsp::AudioBlock<float> lowBlock(lowFreqBuffer);
    juce::dsp::ProcessContextReplacing<float> lowContext(lowBlock);
    lowPassFilter.process(lowContext);
    
    juce::dsp::AudioBlock<float> highBlock(highFreqBuffer);
    juce::dsp::ProcessContextReplacing<float> highContext(highBlock);
    highPassFilter.process(highContext);
    
    // Apply Preisach transformer model to each band with different intensities
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* originalData = buffer.getWritePointer(channel);
        float* lowData = lowFreqBuffer.getWritePointer(channel);
        float* highData = highFreqBuffer.getWritePointer(channel);
        
        // Use appropriate model based on channel (L/R)
        PreisachTransformerModel& model = (channel == 0) ? transformerModelL : transformerModelR;
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Process low frequencies with more intense saturation (transformers affect lows more)
            float saturatedLow = model.process(lowData[sample], drive * 1.5f);
            
            // Process high frequencies with less saturation
            float saturatedHigh = model.process(highData[sample], drive * 0.5f);
            
            // Mix back together and apply output gain
            originalData[sample] = (saturatedLow + saturatedHigh) * outputGain;
        }
    }
}

bool TransformerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* TransformerAudioProcessor::createEditor()
{
    return new TransformerAudioProcessorEditor(*this);
}

void TransformerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TransformerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr)
        if (xml->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TransformerAudioProcessor();
}
