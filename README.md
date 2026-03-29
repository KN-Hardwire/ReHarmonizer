Plugin Project @ HARDWIRE (setting up)

## Dependencies
- JUCE 8
- CMake (version 3.22 or higher)

## Setting up
To set up and be able to build the project locally you will need 
the JUCE framework installed locally. If you're cloning the project 
for the first time, include a flag to also install JUCE as a submodule:
```
git clone --recurse-submodules <repo-url>
```

If you have already done a 'plain' clone on the repository and need to 
install the JUCE submodule, from project root run:
```
git submodule update --init --recursive
```

## Building
To build the project run this from project root path:
```
cmake -B build
cd build
make
```

If the building process was succesful you should see relevant artefacts 
in the build directory (plugin in formats VST3, AU and Standalone).
