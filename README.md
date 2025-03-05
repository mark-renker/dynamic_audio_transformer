# Lovely Transformer

A sophisticated audio transformer saturation plugin using the Preisach hysteresis model to accurately emulate the sound and behavior of high-quality transformer hardware.

## Features

- Advanced Preisach hysteresis modeling for authentic transformer saturation
- Frequency-dependent processing for natural transformer response
- Separate control over even and odd harmonics
- Adjustable hysteresis and asymmetry parameters
- Simple but effective UI with intuitive controls

## Building

This project uses CMake to build the plugin. From the project directory:

```
mkdir -p build
cd build
cmake ..
cmake --build .
```

The VST3 and AU plugin files will be created in the `build/TransformerPlugin_artefacts` directory.
