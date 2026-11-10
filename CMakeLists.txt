# Lovely Transformer — transformer saturation plug-in (Preisach-style hysteresis model). AU / VST3.
cmake_minimum_required(VERSION 3.22)
project(TransformerPlugin VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(APPLE)
  set(DAT_ARCHS "arm64;x86_64" CACHE STRING "macOS architectures (use arm64 for a fast dev build)")
  set(CMAKE_OSX_ARCHITECTURES "${DAT_ARCHS}" CACHE STRING "" FORCE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)
endif()
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ---- JUCE: local checkout when present, otherwise fetch 8.0.6 --------------
include(FetchContent)
if(DEFINED DAT_JUCE_PATH AND EXISTS "${DAT_JUCE_PATH}/CMakeLists.txt")
  add_subdirectory("${DAT_JUCE_PATH}" JUCE)
elseif(EXISTS "$ENV{HOME}/JUCE/CMakeLists.txt")
  message(STATUS "Using local JUCE at $ENV{HOME}/JUCE")
  add_subdirectory("$ENV{HOME}/JUCE" JUCE)
elseif(EXISTS "/Applications/JUCE/CMakeLists.txt")
  message(STATUS "Using local JUCE at /Applications/JUCE")
  add_subdirectory("/Applications/JUCE" JUCE)
else()
  FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.6 GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(JUCE)
endif()

set(DAT_FORMATS VST3)
if(APPLE)
  list(APPEND DAT_FORMATS AU)
endif()

juce_add_plugin(TransformerPlugin
    PRODUCT_NAME "Lovely Transformer"
    COMPANY_NAME "Fourier_Ventures_LLC"
    BUNDLE_ID com.fourierventures.lovelytransformer
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_ENABLED TRUE
    PLUGIN_MANUFACTURER_CODE Fvnt
    PLUGIN_CODE TSAT
    FORMATS ${DAT_FORMATS}
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(TransformerPlugin PRIVATE PluginProcessor.cpp PluginEditor.cpp)
target_include_directories(TransformerPlugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(TransformerPlugin PUBLIC
    JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_DISPLAY_SPLASH_SCREEN=0)
target_link_libraries(TransformerPlugin
    PRIVATE
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags)
