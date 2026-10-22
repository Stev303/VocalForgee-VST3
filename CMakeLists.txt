```cmake
cmake_minimum_required(VERSION 3.22)

project(VocalForge VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# JUCE framework
add_subdirectory(JUCE)

# Plugin configuration
juce_add_plugin(VocalForge
    COMPANY_NAME "VocalForge Audio"
    BUNDLE_ID "com.vocalforge.multifx"

    PLUGIN_MANUFACTURER_CODE VFor
    PLUGIN_CODE VFx1

    FORMATS VST3 Standalone
    PRODUCT_NAME "VocalForge MultiFX"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

    COPY_PLUGIN_AFTER_BUILD FALSE
)

# Generate JuceHeader.h automatically
juce_generate_juce_header(VocalForge)

# Source files
target_sources(VocalForge
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# Compiler definitions
target_compile_definitions(VocalForge
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

# JUCE libraries
target_link_libraries(VocalForge
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Windows compatibility
if(WIN32)
    target_compile_definitions(VocalForge
        PRIVATE
            _CRT_SECURE_NO_WARNINGS
    )
endif()
```
