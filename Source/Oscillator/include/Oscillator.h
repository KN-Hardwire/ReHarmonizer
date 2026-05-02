#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <cmath>

//==============================================================================
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
        //==========================================================================
        enum class Waveform {
            Sine, Square, Saw, Triangle
        };

        //==========================================================================
        Oscillator() = default;
        ~Oscillator() = default;

        //==========================================================================
        void setSampleRate(double newSampleRate) {
            jassert(newSampleRate > 0.0);
            sampleRate = newSampleRate;
            updatePhaseIncrement();
        }

        void setFrequency(float newFrequencyHz) {
            jassert(newFrequencyHz > 0.0f);
            frequency = newFrequencyHz;
            updatePhaseIncrement();
        }

        void setWaveform(Waveform newWaveform) noexcept {
            waveform = newWaveform;
        }

        Waveform getWaveform() const noexcept {
            return waveform;
        }

        float getFrequency() const noexcept {
            return frequency;
        }

        double getSampleRate() const noexcept {
            return sampleRate;
        }

        //==========================================================================
        /** Reset the oscillator phase to zero. */
        void reset() noexcept {
            phase = 0.0f;
        }

        //==========================================================================
        /** Generate and return the next sample [-1, +1]. */
        float processSample() noexcept {
            float sample = generateSample(phase);
            advancePhase();
            return sample;
        }

        //==========================================================================
        /** Fill a buffer with generated samples (mono). */
        void processBlock(juce::AudioBuffer<float>& buffer, int channel = 0) {
            auto* channelData = buffer.getWritePointer(channel);
            const int numSamples = buffer.getNumSamples();

            for (int i = 0; i < numSamples; ++i)
                channelData[i] = processSample();
        }

        /** Add generated samples on top of existing buffer content. */
        void addToBlock(juce::AudioBuffer<float>& buffer, int channel = 0, float gain = 1.0f) {
            auto* channelData = buffer.getWritePointer(channel);
            const int numSamples = buffer.getNumSamples();

            for (int i = 0; i < numSamples; ++i)
                channelData[i] += processSample() * gain;
        }

        //==========================================================================
        static juce::StringArray getWaveformNames() {
            return {"Sine", "Square", "Saw", "Triangle"};
        }

        static Waveform waveformFromIndex(int index) {
            switch (index) {
                case 0: return Waveform::Sine;
                case 1: return Waveform::Square;
                case 2: return Waveform::Saw;
                case 3: return Waveform::Triangle;
                default: return Waveform::Sine;
            }
        }

        static int indexFromWaveform(Waveform w) {
            switch (w) {
                case Waveform::Sine: return 0;
                case Waveform::Square: return 1;
                case Waveform::Saw: return 2;
                case Waveform::Triangle: return 3;
                default: return 0;
            }
        }

    private:
        //==========================================================================
        float  frequency{440.0f};
        double sampleRate{44100.0};
        float  phase{0.0f};
        float  phaseIncrement{0.0f};
        Waveform waveform{Waveform::Sine};

        static constexpr float TWO_PI = juce::MathConstants<float>::twoPi;

        //==========================================================================
        void updatePhaseIncrement() noexcept {
            if (sampleRate > 0.0)
                phaseIncrement = static_cast<float>(TWO_PI * frequency / sampleRate);
        }

        void advancePhase() noexcept {
            phase += phaseIncrement;
            if (phase >= TWO_PI)
                phase -= TWO_PI;
        }

        //==========================================================================
        float generateSample(float p) const noexcept {
            switch (waveform) {
                case Waveform::Sine:
                    return std::sin(p);

                case Waveform::Square:
                    return (p < juce::MathConstants<float>::pi) ? 1.0f : -1.0f;

                case Waveform::Saw:
                    // Ramps from -1 to +1 over one period
                    return (p / juce::MathConstants<float>::pi) - 1.0f;

                case Waveform::Triangle:
                {
                    // V-shape: rises from -1 to +1 in first half, falls back in second
                    const float norm = p / TWO_PI;           // 0..1
                    if (norm < 0.25f)
                        return norm * 4.0f;
                    else if (norm < 0.75f)
                        return 1.0f - (norm - 0.25f) * 4.0f;
                    else
                        return -1.0f + (norm - 0.75f) * 4.0f;
                }

                default:
                    return 0.0f;
            }
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscillator)
};

