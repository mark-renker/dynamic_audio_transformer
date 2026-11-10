# Lovely Transformer

A sophisticated audio transformer saturation plugin using the Preisach hysteresis model to accurately emulate the sound and behavior of high-quality transformer hardware.

## Install (no build needed)

Grab the latest files from the **[Releases page](../../releases/latest)**:

* **macOS** (11 or later, Apple Silicon and Intel): download `LovelyTransformer-<version>-macOS.pkg` and open it. If
  Gatekeeper objects (the installer is not notarized), right-click the file and choose *Open*. Tick the formats you
  want (AU, VST3); they go to `/Library/Audio/Plug-Ins`. Rescan plug-ins in your DAW. Prefer not to run an
  installer? `LovelyTransformer-<version>-macOS-bundles.zip` has the raw bundles and a manual-install note.
* **Windows** (x64): download `LovelyTransformer-<version>-Windows-x64.zip`, unzip, copy the
  `Lovely Transformer.vst3` folder to `C:\Program Files\Common Files\VST3\`.

The plug-in shows up under **Fourier_Ventures_LLC › Lovely Transformer**. Every tagged release is built by
`.github/workflows/release.yml`; `packaging/make_installer.sh` makes the same `.pkg` locally.

## Features

- Advanced Preisach hysteresis modeling for authentic transformer saturation
- Frequency-dependent processing for natural transformer response
- Separate control over even and odd harmonics
- Adjustable hysteresis and asymmetry parameters
- Simple but effective UI with intuitive controls

## Build from source

Requires CMake 3.22+ and a C++17 compiler (Xcode on macOS, Visual Studio 2022 on Windows). JUCE is picked up from
`-DDAT_JUCE_PATH=...`, `~/JUCE`, or `/Applications/JUCE`; otherwise JUCE 8.0.6 is fetched automatically.

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The VST3 and AU plugin files are created in `build/TransformerPlugin_artefacts/Release`. On macOS the build is
universal (arm64 + x86_64) by default; pass `-DDAT_ARCHS=arm64` for a faster development build.
