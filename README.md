# Windows-CalcEngine

Thermal and optical routines for modeling properties of window and shading systems.

## Prerequisites

- **C++ compiler** with C++20 support (Visual Studio 2022, GCC 11+, Clang 14+)
- **CMake 3.21+**
- **Git** (for FetchContent to download dependencies)

## Building

All dependencies are downloaded automatically via CMake FetchContent on first configure. CMake presets are provided for convenience.

### Presets

`CMakePresets.json` ships two configure presets — framework only, no compiler choices:

| Preset | When to use it |
|---|---|
| `default` | First-time configure; CI. Fetches everything from declared remotes. Picks the system default compiler (MSVC on Windows, system `cc`/`c++` on Linux/macOS). |
| `local` | Consume sibling working copies of dependencies instead of fetching them. |

Examples:

```
cmake --preset default
cmake --preset local
```

`local` expects a sibling directory layout — e.g. `../googletest` next to `../Windows-CalcEngine`. Currently overridden:

| Dependency | Expected path |
|------------|--------------|
| googletest | `../googletest` |
| mimalloc | `../mimalloc` |

Missing siblings fall back to the declared remote automatically, so `local` is safe to invoke even with only a subset of overrides checked out.

For compiler-specific presets (`gcc-13`, `clang-18`, `vs2022`, etc.), see "Personal overrides" below — the [lbnl-scripts](https://github.com/vidanovic/scripts) `cmake-user-presets` command scans your machine and writes a personal `CMakeUserPresets.json` with one preset per detected toolchain.

#### Personal overrides (`CMakeUserPresets.json`)

The four shipped presets cover the common cases — you do **not** need to write `CMakeUserPresets.json` to get default, local, gcc, or clang builds. Create one only when you need something that is specific to your machine and shouldn't ship to the team:

- pin a specific compiler version (e.g. `g++-13` rather than whatever `g++` symlinks to)
- point at a compiler at a non-standard absolute path
- use a custom CMake toolchain file
- want an extra build directory for a side experiment

`CMakeUserPresets.json` lives next to `CMakePresets.json` and is read automatically by CMake (and CLion, VS Code, etc.). It is in `.gitignore`, so each developer's file stays on their own machine. Personal presets can `inherit` from any shipped preset — usually you'll inherit from `local`, `gcc`, or `clang` and only change the few fields you care about.

**Example 1 — pin gcc-13 under WSL** (when your distro has both gcc-11 and gcc-13 installed and `gcc` defaults to the older one):

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "gcc-13",
            "inherits": "gcc",
            "binaryDir": "${sourceDir}/build/gcc-13",
            "cacheVariables": {
                "CMAKE_C_COMPILER":   "/usr/bin/gcc-13",
                "CMAKE_CXX_COMPILER": "/usr/bin/g++-13"
            }
        }
    ]
}
```

Then `cmake --preset gcc-13` (in your WSL shell) configures into `build/gcc-13/` with that specific compiler.

**Example 2 — Windows LLVM clang at a non-PATH location**:

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "clang-windows",
            "inherits": "local",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/clang-windows",
            "cacheVariables": {
                "CMAKE_C_COMPILER":   "C:/Program Files/LLVM/bin/clang.exe",
                "CMAKE_CXX_COMPILER": "C:/Program Files/LLVM/bin/clang++.exe",
                "CMAKE_BUILD_TYPE":   "Release"
            }
        }
    ]
}
```

Use forward slashes even on Windows — CMake handles them and you avoid escaping backslashes in JSON.

**Example 3 — a separate debug build dir on top of any shipped preset**:

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "gcc-debug",
            "inherits": "gcc",
            "binaryDir": "${sourceDir}/build/gcc-debug",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug"
            }
        }
    ]
}
```

This is a **one-time setup per machine** — write the file once, then `cmake --preset <name>` and `cmake --build build/<name>` forever after. Different compilers cache differently, so each personal preset should usually have its own `binaryDir`.

Alternative if you don't want a personal preset at all: set `CC` and `CXX` environment variables in your shell rc (`~/.bashrc`, PowerShell profile) before invoking `cmake --preset default`. CMake picks them up. Trade-off — affects every CMake project on your machine, not just this one.

### Build

```
cmake --build build --config Release --parallel
```

For Debug:

```
cmake --build build --config Debug --parallel
```

### Manual configure (without presets)

```
cmake -B build
```

### Clean rebuild

Delete the `build/` directory and re-run the configure and build commands above.
