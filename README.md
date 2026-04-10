This is a prototype branch for the early stage of the plugin development.

## Core idea
The plugin takes an audio signal on input and extracts the dominant
frequency from that signal using FFT analysis. Then a synthesizer generates
a sine tone controlled by the volume and dominant frequency of the input signal.

## How it works
1. Split input audio into buffers
2. Apply Hanning window + FFT to find dominant frequency
3. Generate continuous sine wave using phase accumulator from previous buffer
4. Write output to a new WAV file

## Problems
Main problems:
- Clicking sounds between buffers caused by phase misalignments. This is fixed
  by generating the next sine samples using phase incrementation, instead of
  generating a new sine signal for every buffer
- When the input signal changes frequency very quickly, clicking sounds can
  occur in the output. This is also mostly fixed by the phase incrementation approach
- When a complex vocal sound is fed to the input, analysing FFT of the raw signal fails (due to
  constant overlapping harmonics that confuse the FFT analysis).
  This was tried to be fixed by filtering the signal before processing, but unsuccessfully

## Missing features
The main features that are missing in the prototype but should be implemented
in the actual plugin are:
- **Real-time** processing (the prototype is file-based)
- A **Dry/Wet Knob** to control the level of raw input and synthesized sine going to the output
- A **Pitch Correction Knob** to control how much the pitch of the synthesized sine tone is
  corrected to the standard tuning system
