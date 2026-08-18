#include "PitchQuantizer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace
{
constexpr std::array<int, 7> majorIntervals { 0, 2, 4, 5, 7, 9, 11 };
constexpr std::array<int, 7> minorIntervals { 0, 2, 3, 5, 7, 8, 10 };

constexpr int notesPerOctave { 12 };
constexpr float referenceFrequencyHz { 440.0f };
constexpr float referenceMidiNote { 69.0f };

const std::array<int, 7>& getScaleIntervals(PitchQuantizer::ScaleMode scaleMode) noexcept
{
    switch (scaleMode)
    {
        case PitchQuantizer::ScaleMode::Minor:
            return minorIntervals;
        case PitchQuantizer::ScaleMode::Major:
        default:
            return majorIntervals;
    }
}

bool isPitchClassInScale(int pitchClass,
                         int rootPitchClass,
                         PitchQuantizer::ScaleMode scaleMode) noexcept
{
    int intervalFromRoot = (pitchClass - rootPitchClass) % notesPerOctave;
    if (intervalFromRoot < 0)
        intervalFromRoot += notesPerOctave;

    const auto& intervals = getScaleIntervals(scaleMode);
    return std::find(intervals.begin(), intervals.end(), intervalFromRoot) != intervals.end();
}

bool isKeyValueValid(int keyValue)
{
	return keyValue >= static_cast<int>(PitchQuantizer::Key::C)
		&& keyValue <= static_cast<int>(PitchQuantizer::Key::B);
}

} // namespace

void PitchQuantizer::setKey(Key newKey) noexcept
{
    const auto keyValue = static_cast<int>(newKey);
    if (isKeyValueValid(keyValue))
	{
        key = newKey;
	}
}

PitchQuantizer::Key PitchQuantizer::getKey() const noexcept
{
    return key;
}

void PitchQuantizer::setScaleMode(ScaleMode newScaleMode) noexcept
{
    switch (newScaleMode)
    {
        case ScaleMode::Major:
        case ScaleMode::Minor:
            scaleMode = newScaleMode;
            break;
        default:
            break;
    }
}

PitchQuantizer::ScaleMode PitchQuantizer::getScaleMode() const noexcept
{
    return scaleMode;
}

void PitchQuantizer::setQuantizationLevel(float newLevel) noexcept
{
    if (!std::isfinite(newLevel))
	{
		return;
	}

    quantizationLevel = std::clamp(newLevel, 0.0f, 1.0f);
}

float PitchQuantizer::getQuantizationLevel() const noexcept
{
    return quantizationLevel;
}

void PitchQuantizer::processFrequency(float frequencyHz) noexcept
{
    lastQuantizedFrequency = quantizeFrequency(frequencyHz);
}

float PitchQuantizer::getQuantizedFrequency() const noexcept
{
    return lastQuantizedFrequency;
}

float PitchQuantizer::quantizeFrequency(float frequencyHz) const noexcept
{
    if (!std::isfinite(frequencyHz) || frequencyHz <= 0.0f || quantizationLevel <= 0.0f)
	{
        return frequencyHz;
	}

    const float inputMidiNote = referenceMidiNote
		+ static_cast<float>(notesPerOctave) * std::log2(frequencyHz / referenceFrequencyHz);

    if (!std::isfinite(inputMidiNote)
        || inputMidiNote < static_cast<float>(std::numeric_limits<int>::min() + notesPerOctave)
        || inputMidiNote > static_cast<float>(std::numeric_limits<int>::max() - notesPerOctave))
    {
        return frequencyHz;
    }

    const int rootPitchClass = static_cast<int>(key);
    const int centreNote = static_cast<int>(std::floor(inputMidiNote));
    int nearestScaleNote = centreNote;
    float nearestDistance = std::numeric_limits<float>::max();

    // Every supported diatonic scale has a matching note within two semitones.
    // A full octave on each side also keeps this correct for future sparse scales.
    for (int candidate = centreNote - notesPerOctave;
         candidate <= centreNote + notesPerOctave;
         ++candidate)
    {
        int pitchClass = candidate % notesPerOctave;
        if (pitchClass < 0)
            pitchClass += notesPerOctave;

        if (!isPitchClassInScale(pitchClass, rootPitchClass, scaleMode))
            continue;

        const float distance = std::abs(inputMidiNote - static_cast<float>(candidate));
        if (distance < nearestDistance)
        {
            nearestDistance = distance;
            nearestScaleNote = candidate;
        }
    }

    // Blend in semitone space so that partial correction is perceptually linear.
    const float correctedMidiNote = inputMidiNote
        + quantizationLevel * (static_cast<float>(nearestScaleNote) - inputMidiNote);

    return referenceFrequencyHz
        * std::exp2((correctedMidiNote - referenceMidiNote) / static_cast<float>(notesPerOctave));
}
