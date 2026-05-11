#pragma once

#include <juce_core/juce_core.h>
#include <cmath>

/**
 * A multi-waveform oscillator supporting sine, square, sawtooth, and triangle.
 *
 * Usage:
 *   oscillator.setSampleRate(44100.0);
 *   oscillator.setFrequency(440.0f);
 *   oscillator.setWaveform(Oscillator::Waveform::Sine);
 *   float sample = oscillator.processSample();
 */
class Oscillator {
public:
    enum class Waveform {
        Sine, Square, Saw, Triangle
    };

    //==========================================================================
    Oscillator() = default;
    ~Oscillator() = default;

    //==========================================================================
    void setSampleRate(double newSampleRate);

    void setFrequency(float newFrequencyHz);

    void setWaveform(Waveform newWaveform) noexcept;

    Waveform getWaveform() const noexcept;

    float getFrequency() const noexcept;

    double getSampleRate() const noexcept;

    //==========================================================================
    /** Reset the oscillator phase to zero. */
    void reset() noexcept;

    //==========================================================================
    /** Generate and return the next sample [-1, +1]. */
    float processSample() noexcept;

private:
    float  frequency{440.0f};
    double sampleRate{44100.0};
    float  phase{0.0f};
    float  phaseIncrement{0.0f};
    Waveform waveform{Waveform::Sine};

    static constexpr float TWO_PI = juce::MathConstants<float>::twoPi;

    //==========================================================================
    void updatePhaseIncrement() noexcept;

    void advancePhase() noexcept;

    //==========================================================================
    float generateSample(float p) const noexcept;
};
