cmake_minimum_required(VERSION 3.22)

project(ReHarmonizer VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(third_party/JUCE)

# ==========================================
# Platform-specific plugin formats
# AU is only supported on macOS
# ==========================================
if(APPLE)
	set(PLUGIN_FORMATS AU VST3 Standalone)
else()
	set(PLUGIN_FORMATS VST3 Standalone)
endif()

# ==========================================
# Plugin info
# ==========================================
juce_add_plugin(ReHarmonizer
	COMPANY_NAME "HARDWIRE"
	PLUGIN_MANUFACTURER_CODE KNHW
	PLUGIN_CODE RHRM

	FORMATS ${PLUGIN_FORMATS}
	PRODUCT_NAME "ReHarmonizer"

	IS_SYNTH FALSE
	NEEDS_MIDI_INPUT FALSE
	NEEDS_MIDI_OUTPUT FALSE
	IS_MIDI_EFFECT FALSE
)

# ==========================================
# Source files
# ==========================================
target_sources(ReHarmonizer
	PRIVATE
	Source/PluginProcessor.h
	Source/PluginProcessor.cpp
	Source/PluginEditor.h
	Source/PluginEditor.cpp
	Source/FrequencyDetector.h
	Source/FrequencyDetector.cpp
	# If your contribution adds a new source file, add it above this line 
)

# ==========================================
# Compile definitions
# ==========================================
target_compile_definitions(ReHarmonizer
	PUBLIC
	JUCE_WEB_BROWSER=0
	JUCE_USE_CURL=0
	JUCE_VST3_CAN_REPLACE_VST2=0
)

# ==========================================
# Link JUCE modules
# ==========================================
target_link_libraries(ReHarmonizer
	PRIVATE
	juce::juce_audio_basics
	juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
	PUBLIC
	juce::juce_recommended_config_flags
	juce::juce_recommended_lto_flags
	juce::juce_recommended_warning_flags
)
