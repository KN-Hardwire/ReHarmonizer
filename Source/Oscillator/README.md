# Oscillator Module

A multi-waveform oscillator audio plugin built with **JUCE 8** and **C++17**.

## Supported Waveforms

| Waveform | Description |
|----------|-------------|
| **Sine** | Pure tone, no harmonics |
| **Square** | Rich odd harmonics, buzzy |
| **Saw** | All harmonics, bright/aggressive |
| **Triangle** | Odd harmonics only, softer than square |

## Parameters

| Parameter | Range | Default |
|-----------|-------|---------|
| Frequency | 20 Hz – 20 kHz (log scale) | 440 Hz |
| Waveform  | Sine / Square / Saw / Triangle | Sine |
| Gain      | 0.0 – 1.0 | 0.7 |

## Project Structure

```
Source/Oscillator/
├── CMakeLists.txt
├── include/
│   ├── Oscillator.h           ← Core oscillator class (reusable, header-only)
│   ├── PluginProcessor.h      ← AudioProcessor + APVTS parameter handling
│   └── PluginEditor.h         ← GUI: knobs, combobox, waveform visualizer
├── src/
│   ├── PluginProcessor.cpp
│   └── PluginEditor.cpp
└── tests/
    ├── OscillatorTests.h      ← Unit tests
    └── RunTests.cpp           ← Test runner
```

## Building from Scratch

### Prerequisites

**Linux (Ubuntu/Debian):**
```bash
sudo apt install cmake git build-essential \
    libfreetype6-dev libfontconfig1-dev \
    libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev \
    libgl1-mesa-dev libasound2-dev
```

**macOS:** Xcode Command Line Tools + CMake (`brew install cmake`)

**Windows:** Visual Studio 2022 + CMake

### Steps

```bash
# 1. Clone the repository
git clone <your-repo>
cd <your-repo>

# 2. Fetch JUCE submodule
git submodule update --init --recursive

# 3. Configure the build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 4. Build
cmake --build build --config Release
```

### Artifacts

```
build/PluginProject_artefacts/Release/
├── Standalone/PluginProject   ← standalone application
└── VST3/PluginProject.vst3   ← plugin for your DAW
```

### Running the Standalone App

```bash
./build/PluginProject_artefacts/Release/Standalone/PluginProject
```

## Running Unit Tests

```bash
cmake --build build --target OscillatorTests
./build/OscillatorTests
```

Expected output:
```
JUCE v8.x.x
=== Oscillator Unit Tests ===
[PASS] Sine starts near 0
[PASS] Square wave outputs only ±1
[PASS] Saw wave stays within [-1, 1]
[PASS] Triangle wave stays within [-1, 1]
[PASS] Waveform switching works correctly
[PASS] Reset restores phase to 0
=== All tests passed! ===
```

Tests cover:
- Sine starts at phase 0
- Square outputs only ±1
- Saw/Triangle stay within [−1, 1]
- Waveform switching
- Phase reset

## Using the Oscillator Class

`Oscillator.h` is reusable and can be dropped into any JUCE project:

```cpp
#include "Oscillator.h"

Oscillator osc;
osc.setSampleRate(44100.0);
osc.setFrequency(440.0f);
osc.setWaveform(Oscillator::Waveform::Square);

// Single sample:
float sample = osc.processSample();   // [-1, +1]

// Or fill an entire buffer:
osc.processBlock(buffer, /*channel=*/0);
