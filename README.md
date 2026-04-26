# delta

`delta` is a C++17 command-line unit conversion utility.

It supports temperature, length, volume, and pressure conversions, with:
- interactive REPL mode
- one-shot CLI conversion mode
- optional readline history support (when `readline` is available)

## Features

- Multiple quantity families in one CLI:
  - temperature (`C`, `F`, `K`, `R`)
  - length (`mm`, `cm`, `m`, `km`, `in`, `ft`, `mi`)
  - volume (`mL`, `L`, `gal`, `fl oz`, `m3`, `mm3`, `cm3`)
  - pressure (`Pa`, `kPa`, `bar`, `atm`, `psi`, `torr`, `mmHg`)
- Case-insensitive unit parsing
- Defensive error handling for invalid physical values
- CMake preset-based Debug and Release builds
- Automated CLI version from CMake project metadata

## Requirements

- CMake 3.20+
- C++17 compiler (`g++` configured in presets)
- Ninja (preset generator)
- Optional: GNU readline development package for history and line editing

## Build

### Debug

```bash
cmake --preset debug-gcc
cmake --build --preset build-debug
```

### Release

```bash
cmake --preset release-gcc
cmake --build --preset build-release
```

Build output binaries are written to `bin/`.

## Run

### REPL mode

```bash
./bin/delta
```

### One-shot conversion

```bash
./bin/delta 100 celsius
./bin/delta 5 km
./bin/delta 1 atm
./bin/delta 12 "fl oz"
```

### Version

```bash
./bin/delta --version
```

The version shown by the binary is generated from `project(... VERSION ...)` in `CMakeLists.txt` during CMake configure.

## Test

This project enables CTest and defines a smoke test executable.

```bash
ctest --test-dir build/debug --output-on-failure
```

If you built only Release, use `build/release` instead.

## Documentation

- Usage guide: `docs/usage.md`
- API reference: `docs/api.md`
- Contribution guide: `CONTRIBUTING.md`
