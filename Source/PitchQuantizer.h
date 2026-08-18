#pragma once

class PitchQuantizer
{
public:
    enum class Key
    {
        C,
        CSharp,
        D,
        DSharp,
        E,
        F,
        FSharp,
        G,
        GSharp,
        A,
        ASharp,
        B
    };

    enum class ScaleMode
    {
        Major,
        Minor
    };

    PitchQuantizer() = default;

    void setKey(Key newKey) noexcept;
    Key getKey() const noexcept;

    void setScaleMode(ScaleMode newScaleMode) noexcept;
    ScaleMode getScaleMode() const noexcept;

    // 0.0 leaves the input pitch untouched; 1.0 applies full correction.
    // Values outside that range are clamped.
    void setQuantizationLevel(float newLevel) noexcept;
    float getQuantizationLevel() const noexcept;

    // Stores the corrected frequency, mirroring FrequencyDetector's
    // process/get API while keeping this module independent of audio processing.
    void processFrequency(float frequencyHz) noexcept;
    float getQuantizedFrequency() const noexcept;

    // Stateless convenience method for callers that do not need stored output.
    float quantizeFrequency(float frequencyHz) const noexcept;

private:
    Key key { Key::C };
    ScaleMode scaleMode { ScaleMode::Major };
    float quantizationLevel { 1.0f };
    float lastQuantizedFrequency { 0.0f };
};
