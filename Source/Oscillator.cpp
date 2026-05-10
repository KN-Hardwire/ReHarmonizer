#include "Oscillator.h"

void Oscillator::setSampleRate(double newSampleRate) {
    jassert(newSampleRate > 0.0);
    sampleRate = newSampleRate;
    updatePhaseIncrement();
}

void Oscillator::setFrequency(float newFrequencyHz) {
    jassert(newFrequencyHz > 0.0f);
    frequency = newFrequencyHz;
    updatePhaseIncrement();
}

void Oscillator::setWaveform(Waveform newWaveform) noexcept {
    waveform = newWaveform;
}

Oscillator::Waveform Oscillator::getWaveform() const noexcept {
    return waveform;
}

float Oscillator::getFrequency() const noexcept {
    return frequency;
}

double Oscillator::getSampleRate() const noexcept {
    return sampleRate;
}

void Oscillator::reset() noexcept {
    phase = 0.0f;
}

float Oscillator::processSample() noexcept {
    float sample = generateSample(phase);
    advancePhase();
    return sample;
}

void Oscillator::updatePhaseIncrement() noexcept {
    if (sampleRate > 0.0)
        phaseIncrement = static_cast<float>(TWO_PI * frequency / sampleRate);
}

void Oscillator::advancePhase() noexcept {
    phase += phaseIncrement;
    if (phase >= TWO_PI)
        phase -= TWO_PI;
}

float Oscillator::generateSample(float p) const noexcept {
    switch (waveform) {
        case Oscillator::Waveform::Sine:
            return std::sin(p);

        case Oscillator::Waveform::Square:
            return (p < juce::MathConstants<float>::pi) ? 1.0f : -1.0f;

        case Oscillator::Waveform::Saw:
            return (p / juce::MathConstants<float>::pi) - 1.0f;

        case Oscillator::Waveform::Triangle:
        {
            const float norm = p / TWO_PI;
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
