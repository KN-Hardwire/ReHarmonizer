#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include "Oscillator.h"

namespace OscillatorTests {
    static bool approxEqual(float a, float b, float tol = 1e-4f) {
        return std::fabs(a - b) < tol;
    }

    static void testSineZeroCrossings() {
        Oscillator osc;
        osc.setSampleRate(44100.0);
        osc.setFrequency(440.0f);
        osc.setWaveform(Oscillator::Waveform::Sine);

        const float first = osc.processSample();
        assert(approxEqual(first, 0.0f, 0.01f) && "Sine should start near 0");
        std::cout << "[PASS] Sine starts near 0\n";
    }

    static void testSquareOnlyPlusMinus1() {
        Oscillator osc;
        osc.setSampleRate(44100.0);
        osc.setFrequency(100.0f);
        osc.setWaveform(Oscillator::Waveform::Square);

        for (int i = 0; i < 441; ++i) {
            const float s = osc.processSample();
            assert((approxEqual(s, 1.0f) || approxEqual(s, -1.0f))
                   && "Square wave must be +1 or -1");
        }
        std::cout << "[PASS] Square wave outputs only ±1\n";
    }

    static void testSawRange() {
        Oscillator osc;
        osc.setSampleRate(44100.0);
        osc.setFrequency(100.0f);
        osc.setWaveform(Oscillator::Waveform::Saw);

        for (int i = 0; i < 441; ++i) {
            const float s = osc.processSample();
            assert(s >= -1.0f && s <= 1.0f && "Saw wave must stay in [-1, 1]");
        }
        std::cout << "[PASS] Saw wave stays within [-1, 1]\n";
    }

    static void testTriangleRange() {
        Oscillator osc;
        osc.setSampleRate(44100.0);
        osc.setFrequency(100.0f);
        osc.setWaveform(Oscillator::Waveform::Triangle);

        for (int i = 0; i < 441; ++i) {
            const float s = osc.processSample();
            assert(s >= -1.001f && s <= 1.001f && "Triangle must stay in [-1, 1]");
        }
        std::cout << "[PASS] Triangle wave stays within [-1, 1]\n";
    }

    static void testWaveformSwitching() {
        Oscillator osc;
        osc.setSampleRate(44100.0);
        osc.setFrequency(440.0f);

        osc.setWaveform(Oscillator::Waveform::Sine);
        assert(osc.getWaveform() == Oscillator::Waveform::Sine);

        osc.setWaveform(Oscillator::Waveform::Square);
        assert(osc.getWaveform() == Oscillator::Waveform::Square);

        osc.setWaveform(Oscillator::Waveform::Saw);
        assert(osc.getWaveform() == Oscillator::Waveform::Saw);

        osc.setWaveform(Oscillator::Waveform::Triangle);
        assert(osc.getWaveform() == Oscillator::Waveform::Triangle);

        std::cout << "[PASS] Waveform switching works correctly\n";
    }

    static void testReset() {
        Oscillator osc;
        osc.setSampleRate(44100.0);
        osc.setFrequency(440.0f);
        osc.setWaveform(Oscillator::Waveform::Sine);

        for (int i = 0; i < 100; ++i) osc.processSample();

        osc.reset();
        const float afterReset = osc.processSample();
        assert(approxEqual(afterReset, 0.0f, 0.01f) && "After reset, sine should start near 0");
        std::cout << "[PASS] Reset restores phase to 0\n";
    }

    static void runAll() {
        std::cout << "=== Oscillator Unit Tests ===\n";
        testSineZeroCrossings();
        testSquareOnlyPlusMinus1();
        testSawRange();
        testTriangleRange();
        testWaveformSwitching();
        testReset();
        std::cout << "=== All tests passed! ===\n";
    }
}