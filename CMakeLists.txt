cmake_minimum_required(VERSION 3.15)
# Add this line after cmake_minimum_required
cmake_policy(SET CMP0175 NEW)
project(TransformerPlugin VERSION 0.1.0)

# Add JUCE
add_subdirectory(/Applications/JUCE ${CMAKE_BINARY_DIR}/JUCE)

juce_add_plugin(TransformerPlugin
    COMPANY_NAME "Fourier_Ventures_LLC"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_ENABLED TRUE
    PLUGIN_MANUFACTURER_CODE Fvnt  # just an example 4-char code
    PLUGIN_CODE TSAT  # 4-char unique plugin code
    FORMATS VST3 AU
    PRODUCT_NAME "Lovely Transformer")

target_sources(TransformerPlugin PRIVATE 
    PluginProcessor.cpp PluginEditor.cpp)
#target_link_libraries(TransformerPlugin PRIVATE juce::juce_audio_processors)

# Add these lines after your existing target_link_libraries
target_link_libraries(TransformerPlugin PRIVATE
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_dsp
)

# Add this to ensure proper include paths
target_include_directories(TransformerPlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)