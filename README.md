<div align="center">

# ReHarmonizer

### *Real-time lightweight harmonizing effect synth driven directly by input audio frequency.*

<b>Framework:</b> JUCE 8 &nbsp;&nbsp;|&nbsp;&nbsp; <b>Language:</b> C++17

---

 [Features](#features) • [Prerequisites](#prerequisites) • [Setting Up](#setting-up) • [Building](#building) • [License](#license)

---

</div>

## Features

ReHarmonizer listens to the pitch of an incoming signal, detects its fundamental frequency in real time and uses it to control a built-in synthesizer engine - transforming any monophonic audio source into a dynamically playable harmonic texture. 

---

## Prerequisites

| Requirement | Specification |
| :--- | :--- |
| **Compiler** | C++17 compatible compiler |
| **Build System** | CMake 3.22 or higher |
| **Framework** | JUCE 8 |

---

## Setting Up

### Clone the Repository
To get the source code along with the JUCE submodule, run the following command:
```bash
git clone --recurse-submodules <repo-url>
```

### Missing Submodules
If you already did a plain clone and need to fetch the JUCE submodule, execute:
```bash
git submodule update --init --recursive
```

---

## Building

Execute the following commands from the project root path to build the project:

```bash
cmake -B build
cmake --build build
```

### Build Artifacts
After a successful build, relevant artifacts will be in the `build` directory:
* plugin in formats **VST3**
* **AU** (macOS only)
* **Standalone**

---

## License

This project is licensed under the [MIT License](LICENSE).

---
<div align="center">
  <sub>ReHarmonizer • KN-Hardwire</sub>
</div>
