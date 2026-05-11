#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <array>

class FrequencyDetector
{
public:
    FrequencyDetector();

    void prepare(double sampleRate);
    void processSample(float sample);
    float getFrequency() const;

private:
    static constexpr std::size_t collectionSize = 2048;
    static constexpr std::size_t fftOrder = 12;
    static constexpr std::size_t fftSize  = 1 << fftOrder;

    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    std::array<float, fftSize> fifo { 0.0f };
    std::array<float, fftSize * 2> fftData { 0.0f };

    std::size_t fifoIndex { 0 };
    float currentSampleRate { 44100.0f };
    float lastCalculatedFrequency { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyDetector)
};
