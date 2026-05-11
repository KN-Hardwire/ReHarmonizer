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
}

void FrequencyDetector::processSample(float sample)
{
    if (fifoIndex == fftSize)
    {
        std::copy(fifo.begin(), fifo.end(), fftData.begin());
        std::fill(fftData.begin() + collectionSize, fftData.end(), 0.0f);

        window.multiplyWithWindowingTable(fftData.data(), collectionSize); // Windowing fftData
        forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());   // FFT

        float maxMagnitude { 0.0f };
        std::size_t maxBin { 0 };

        for (std::size_t i = 1; i < fftSize / 2 - 1; ++i)
        {
            // Search for max amplitude
            if (fftData[i] > maxMagnitude)
            {
                maxMagnitude = fftData[i];
                maxBin = i;
            }
        }

        // Parabolic interpolation for improved accuracy
        float interpolatedBinIndex = static_cast<float>(maxBin);

        if (maxBin > 0 && maxBin < (fftSize / 2 - 1)) {
            float a = fftData[maxBin - 1];
            float b = fftData[maxBin];
            float c = fftData[maxBin + 1];

            float p = 0.5f * (a - c) / (a - 2.0f * b + c);
            interpolatedBinIndex += p;
        }

        lastCalculatedFrequency = interpolatedBinIndex * currentSampleRate / static_cast<float>(fftSize);
        fifoIndex = 0;
    }

    fifo[fifoIndex++] = sample;
}

float FrequencyDetector::getFrequency() const
{
    return lastCalculatedFrequency;
}
