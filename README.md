# ReHarmonizer

A real-time lightweight harmonizing effect synth driven directly by input audio frequency.

ReHarmonizer listens to the pitch of an incoming signal, detects its fundamental frequency
in real time and uses it to control a built-in synthesizer engine - transforming any 
monophonic audio source into a dynamically playable harmonic texture.


## Prerequisites

- C++17 compatible compiler
- CMake 3.22 or higher
- JUCE 8


## Setting up

Clone the repository with JUCE submodule:

```
git clone --recurse-submodules <repo-url>
```

If you have already done a 'plain' clone on the repository and need to 
install the JUCE submodule, run:

```
git submodule update --init --recursive
```


## Building

To build the project run this from project root path:

```
cmake -B build
cmake --build build
```

Relevant build artefacts will be in the build directory (plugin in formats VST3, 
AU (if on macOS) and Standalone).


## License

This project is licensed under the [MIT License](LICENSE).
