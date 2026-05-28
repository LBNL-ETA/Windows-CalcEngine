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

For compiler-specific presets (`gcc-13`, `clang-18`, `vs2022`, etc.), see "Per-machine compiler presets" below — the [lbnl-scripts](https://github.com/vidanovic/scripts) `cmake-user-presets` command scans your machine and writes a personal `CMakeUserPresets.json` with one preset per detected toolchain, pre-wired for CLion's WSL / Visual Studio toolchains.

#### Per-machine compiler presets (`CMakeUserPresets.json`)

`CMakePresets.json` ships only the four `default-*` / `local-*` framework presets — no compiler choices. To get explicit per-compiler presets (`vs2022-debug`, `gcc-13-release`, `clang-18-debug`, etc.), use the **[lbnl-scripts](https://github.com/vidanovic/scripts) `cmake-user-presets` command**. One-time setup per machine:

```bash
pip install --upgrade git+https://github.com/vidanovic/scripts.git
cd Windows-CalcEngine
cmake-user-presets
```

The script:

- Detects every C/C++ toolchain available to your machine — local PATH, WSL (on Windows when available), and every installed Visual Studio version (via `vswhere`)
- Writes a `Debug` and a `Release` preset per detected toolchain into `CMakeUserPresets.json` (next to `CMakePresets.json`)
- Inherits each from the shipped `local` preset so sibling-repo overrides apply automatically
- Embeds a JetBrains vendor hint (`vendor.jetbrains.com/clion.toolchain`) so CLion 2023.2+ routes each preset through the correct toolchain automatically — `WSL`-detected presets bounce through your WSL distro, `vs2022-*` use Visual Studio, no manual Toolchain dropdown configuration needed

After running the command, `Tools → CMake → Reset Cache and Reload Project` in CLion picks up every variant in the picker. Pick `clang-18-release`, build, done.

`CMakeUserPresets.json` is gitignored — your file stays on your machine, doesn't enter the repo.

##### Windows + WSL + CLion walkthrough

The Windows + WSL + CLion combination is the most common LBNL dev setup, and the script is built around it. Concretely, on a Windows laptop with WSL Ubuntu installed:

```powershell
# PowerShell
pip install --upgrade git+https://github.com/vidanovic/scripts.git
cd D:\Programming\GitHub\Windows-CalcEngine
cmake-user-presets
```

The script detects:

- Visual Studio 2022 (via `vswhere`)
- Whatever gcc / clang versions are installed inside your WSL distro (`gcc-13`, `clang-18`, etc.)

…and writes `CMakeUserPresets.json` next to the shipped `CMakePresets.json`, with a `Debug` and a `Release` variant for each toolchain, each tagged with the right JetBrains toolchain hint.

In CLion:

1. `Tools → CMake → Reset Cache and Reload Project` (or close + reopen the project)
2. The profile picker (top bar) now lists every preset: `default-debug`, `default-release`, `local-debug`, `local-release`, plus the per-compiler ones the script generated.
3. Pick `clang-18-release` → CLion routes the configure through your WSL `cmake`, finds `/usr/bin/clang++-18`, builds in `build/clang-18-release/` on the WSL side. Run/Debug works through the WSL toolchain end-to-end.
4. Pick `vs2022-release` → CLion uses Windows-side `cmake.exe` with the Visual Studio 17 2022 generator, builds in `build/vs2022-release/`. Native Windows debugger attaches.

No manual Toolchain dropdown changes, no per-preset configuration in `Settings → CMake`. The script's vendor hint does the routing.

**Assumption:** the CLion toolchains in `Settings → Build, Execution, Deployment → Toolchains` keep their default names (`WSL`, `Visual Studio`). If a developer has renamed one, the vendor hint silently doesn't match for that one — CLion falls back to its global default. No configure-time error, just a one-row dropdown to set manually.

##### Hand-writing a personal preset

If you need something the script doesn't generate (a custom compiler path, an extra build dir for a side experiment, etc.), edit `CMakeUserPresets.json` directly. Personal presets `inherit` from any shipped preset:

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "clang-windows",
            "inherits": "local-release",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/clang-windows",
            "cacheVariables": {
                "CMAKE_C_COMPILER":   "C:/Program Files/LLVM/bin/clang.exe",
                "CMAKE_CXX_COMPILER": "C:/Program Files/LLVM/bin/clang++.exe"
            }
        }
    ]
}
```

Use forward slashes even on Windows — CMake handles them and you avoid escaping backslashes in JSON.

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
