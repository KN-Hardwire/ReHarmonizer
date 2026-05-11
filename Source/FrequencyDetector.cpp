#include "FrequencyDetector.h"

FrequencyDetector::FrequencyDetector()
    : forwardFFT(fftOrder),
      window(collectionSize, juce::dsp::WindowingFunction<float>::hann)
{
}

void FrequencyDetector::prepare(double sampleRate)
{
    currentSampleRate = static_cast<float>(sampleRate);
    fifoIndex = 0;
    lastCalculatedFrequency = 0.0f;
    std::fill(fifo.begin(), fifo.end(), 0.0f);
    std::fill(fftData.begin(), fftData.end(), 0.0f);
}

void FrequencyDetector::processSample(float sample)
{
    fifo[fifoIndex++] = sample;

    if (fifoIndex < collectionSize)
        return;

    analyseCurrentWindow();

    constexpr auto retainedSamples = collectionSize - hopSize;
    std::move(fifo.begin() + static_cast<std::ptrdiff_t>(hopSize), fifo.end(), fifo.begin());
    fifoIndex = retainedSamples;
}

void FrequencyDetector::analyseCurrentWindow()
{
    std::copy(fifo.begin(), fifo.end(), fftData.begin());
    std::fill(fftData.begin() + static_cast<std::ptrdiff_t>(collectionSize), fftData.end(), 0.0f);

    window.multiplyWithWindowingTable(fftData.data(), collectionSize);
    forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());

    float maxMagnitude { 0.0f };
    std::size_t maxBin { 0 };

    for (std::size_t i = 1; i < fftSize / 2 - 1; ++i)
    {
        if (fftData[i] > maxMagnitude)
        {
            maxMagnitude = fftData[i];
            maxBin = i;
        }
    }

    constexpr float minMagnitude = 1.0e-4f;
    if (maxMagnitude < minMagnitude)
    { // Prevent from picking up garbage values near zero
        lastCalculatedFrequency = 0.0f;
        return;
    }

    float interpolatedBinIndex = static_cast<float>(maxBin);

    if (maxBin > 0 && maxBin < (fftSize / 2 - 1))
    {
        const float a = fftData[maxBin - 1];
        const float b = fftData[maxBin];
        const float c = fftData[maxBin + 1];
        const float denominator = a - 2.0f * b + c;

        if (std::abs(denominator) > std::numeric_limits<float>::epsilon())
        {
            const float p = 0.5f * (a - c) / denominator;
            interpolatedBinIndex += p;
        }
    }

    lastCalculatedFrequency = interpolatedBinIndex * currentSampleRate / static_cast<float>(fftSize);
}

float FrequencyDetector::getFrequency() const
{
    return lastCalculatedFrequency;
}
