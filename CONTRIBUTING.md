# Contributing to delta

Thanks for contributing.

This guide focuses on the day-to-day workflow for building, testing, and submitting changes.

## Prerequisites

- CMake 3.20+
- C++17-capable compiler (`g++` in current presets)
- Ninja
- Optional: `readline` development package for richer REPL behavior

## Repository Layout

- `main.cpp`: program entry point
- `inc/`: public headers
- `src/`: implementation files
- `tests/`: CTest smoke test source
- `docs/`: project documentation
- `CMakePresets.json`: configure/build presets used by contributors and CI-style flows

## Build Workflow

### Debug build

```bash
cmake --preset debug-gcc
cmake --build --preset build-debug
```

### Release build

```bash
cmake --preset release-gcc
cmake --build --preset build-release
```

### Convenience script

Use the helper script if you prefer:

```bash
./build.sh debug
./build.sh release
```

## Test Workflow

Run tests after a successful build:

```bash
ctest --test-dir build/debug --output-on-failure
```

For release-only builds:

```bash
ctest --test-dir build/release --output-on-failure
```

## Running Locally

```bash
./bin/delta
./bin/delta 100 celsius
```

See `docs/usage.md` for more examples and accepted unit aliases.

## Coding Notes

- Keep changes focused and minimal.
- Preserve existing style in touched files.
- Add tests when behavior changes.
- Update docs when user-visible behavior changes.

## Versioning

- Runtime version output (`./bin/delta --version`) is automated from CMake project metadata.
- Source of truth is `project(delta VERSION x.y.z ...)` in `CMakeLists.txt`.
- To bump the application version, update that version and reconfigure/build.

## Pull Request Checklist

- [ ] Code builds with the intended preset(s)
- [ ] Relevant tests pass
- [ ] New or changed behavior is covered by tests where appropriate
- [ ] Documentation is updated (`README.md`, `docs/usage.md`, `docs/api.md`)
- [ ] Commit messages clearly describe the change
