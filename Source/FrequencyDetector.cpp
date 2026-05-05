#include "FrequencyDetector.h"

FrequencyDetector::FrequencyDetector()
    : forwardFFT (fftOrder),
      window (collectionSize, juce::dsp::WindowingFunction<float>::hann)
{
}

void FrequencyDetector::prepare (double sampleRate)
{
    currentSampleRate = static_cast<float>(sampleRate);
    fifoIndex = 0;
    lastCalculatedFrequency = 0.0f;
}

void FrequencyDetector::processSample (float sample)
{
    if (fifoIndex == fftSize)
    {
        // Skopiowanie fifo do fftdata
        std::copy (fifo.begin(), fifo.end(), fftData.begin());
        // Wypelnienie drugiej połowy fftdata zerami
        std::fill (fftData.begin() + collectionSize, fftData.end(), 0.0f);

        window.multiplyWithWindowingTable (fftData.data(), collectionSize); // Okienkowanie fftdata
        forwardFFT.performFrequencyOnlyForwardTransform (fftData.data());   // FFT

        float maxMagnitude { 0.0f };
        int maxBin { 0 };

        for (std::size_t i = 1; i < static_cast<std::size_t>(fftSize) / 2 - 1; ++i)
        {
            // Wyszukiwanie gdzie jest najwieksza amplituda
            if (fftData[i] > maxMagnitude)
            {
                maxMagnitude = fftData[i];
                maxBin = static_cast<int>(i);
            }
        }
        //Interpolacja paraboliczna w celu zwiększenia dokładności
        float interpolatedBinIndex = static_cast<float>(maxBin);

        if (maxBin > 0 && maxBin < (fftSize / 2 - 1)) {
            float a = fftData[maxBin - 1];
            float b  = fftData[maxBin];
            float c = fftData[maxBin + 1];

            float p = 0.5f * (a - c) / (a - 2.0f * b + c);
            interpolatedBinIndex += p;
        }

        lastCalculatedFrequency = interpolatedBinIndex * currentSampleRate / static_cast<float>(fftSize);
        fifoIndex = 0;
    }

    fifo[static_cast<std::size_t>(fifoIndex++)] = sample;
}

float FrequencyDetector::getFrequency() const
{
    return lastCalculatedFrequency;
}
