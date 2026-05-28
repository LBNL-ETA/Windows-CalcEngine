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

For compiler-specific presets (`gcc-13`, `clang-18`, `vs2022`, etc.), see "Per-machine compiler presets" below — each developer maintains their own `CMakeUserPresets.json` with one preset per toolchain they actually want to use.

#### Per-machine compiler presets (`CMakeUserPresets.json`)

`CMakePresets.json` ships only the four `default-*` / `local-*` framework presets — no compiler choices. To get explicit per-compiler presets (`vs2022-debug`, `gcc-13-release`, `clang-18-debug`, etc.), each developer maintains their own `CMakeUserPresets.json` next to `CMakePresets.json`. It is gitignored, read automatically by CMake (and CLion, VS Code, etc.), and stays on the developer's machine.

Personal presets `inherit` from one of the shipped presets (usually `local`, which gives you sibling-repo overrides for free) and override whatever they want. A complete realistic example — building with WSL Clang on a Windows machine, with CLion 2023.2+ routed through the WSL toolchain automatically:

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "clang-release",
            "displayName": "clang (Release)",
            "inherits": "local",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/clang-release",
            "cacheVariables": {
                "CMAKE_C_COMPILER":   "clang",
                "CMAKE_CXX_COMPILER": "clang++",
                "CMAKE_BUILD_TYPE":   "Release"
            },
            "vendor": {
                "jetbrains.com/clion": {
                    "toolchain": "WSL"
                }
            }
        }
    ]
}
```

A few things going on in that one preset:

- `"inherits": "local"` → picks up sibling-repo overrides (when present) and the rest of the framework setup.
- Bare compiler names (`clang`, `clang++`) rather than `/usr/bin/clang` → portable to any machine that has that toolchain on `PATH`. Use absolute paths only if the compiler isn't on `PATH` (e.g. `C:/Program Files/LLVM/bin/clang.exe` — forward slashes work in JSON, no escaping needed).
- `"vendor.jetbrains.com/clion.toolchain"` → tells CLion (2023.2+) which configured toolchain to route this preset through. Standard names are `WSL`, `Visual Studio`, `MinGW`; whatever you see in `Settings → Build, Execution, Deployment → Toolchains`. The hint is silently ignored if the name doesn't match — no configure-time error.

Add as many of those blocks as you have toolchains you want explicit presets for (one per compiler × build type). Each gets its own `binaryDir` so Debug and Release artifacts don't clobber each other.

Alternative if you don't want a personal preset at all: set `CC` and `CXX` environment variables in your shell rc (`~/.bashrc`, PowerShell profile) before invoking `cmake --preset default-release`. CMake picks them up. Tradeoff — affects every CMake project on your machine, not just this one.

### Build

Each preset lands in its own subdirectory under `build/`:

```
cmake --build build/default-release --parallel
cmake --build build/default-debug   --parallel
cmake --build build/clang-18-release --parallel        # if you ran cmake-user-presets
cmake --build build/vs2022-release   --parallel
```

### Manual configure (without presets)

```
cmake -B build
cmake --build build --config Release --parallel
```

### Clean rebuild

Delete the `build/` directory (removes every preset's build tree) and re-run the configure and build commands above.
