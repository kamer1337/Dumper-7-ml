
# Dumper-7

**Dumper-7** is a powerful SDK generator for all Unreal Engine games, supporting all versions of **UE4** and **UE5**. It automatically generates a complete C++ SDK from any UE game, enabling you to interact with game internals programmatically.

## Features

✨ **Comprehensive UE Support** - Works with UE4 (all versions) and UE5  
🚀 **Fast Generation** - Generates complete SDKs in seconds  
📦 **Multiple Output Formats** - C++ SDK, .usmap mappings, IDA scripts, Dumpspace format  
🔧 **Highly Configurable** - Extensive settings for customization  
📚 **Well-Documented** - Complete guides for building and using the SDK  
🎯 **Type-Safe** - Generated SDK with proper C++ types and inheritance  
🔄 **Dependency Resolution** - Automatically handles package dependencies  
⚡ **Optimized** - Efficient algorithms for large games (100k+ objects)

## Quick Start

### 1. Build the DLL

Choose your preferred build system:

<details>
<summary><b>Visual Studio (Recommended)</b></summary>

```bash
1. Open Dumper-7.sln in Visual Studio 2019/2022
2. Select x64-Release configuration
3. Build Solution (Ctrl+Shift+B)
4. Output: Bin/Release/Dumper-7.dll
```
</details>

<details>
<summary><b>CMake</b></summary>

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

See [UsingCMake.md](UsingCMake.md) for details.
</details>

<details>
<summary><b>Xmake</b></summary>

```bash
xmake f -p windows -a x64 -m release
xmake build
```

See [Xmake.md](Xmake.md) for details.
</details>

### 2. Inject into Target Game

- Use your preferred DLL injector (e.g., Xenos, Process Hacker, etc.)
- Inject `Dumper-7.dll` into the game process
- Wait for generation to complete (console window will show progress)

### 3. Use the Generated SDK

- Default output location: `C:\Dumper-7\GameName-GameVersion\CppSDK\`
- The generated `SDK.hpp` file is now organized into clear sections (structs, classes) for easy navigation
- **See [UsingTheSDK.md](UsingTheSDK.md) for a complete guide on using the generated SDK**
- **Check out [SDKExample/](SDKExample/) for a ready-to-compile sample project demonstrating SDK usage**

**Quick Tip:** For faster compilation, include only specific package headers instead of `SDK.hpp`:
```cpp
#include "SDK/Engine_classes.hpp"  // Instead of "SDK.hpp"
```

## Documentation

| Document | Description |
|----------|-------------|
| [UsingTheSDK.md](UsingTheSDK.md) | Complete guide for using the generated SDK in your projects |
| [SDKExample/](SDKExample/) | **Ready-to-compile sample project** demonstrating SDK usage |
| [UsingCMake.md](UsingCMake.md) | CMake build system guide |
| [Xmake.md](Xmake.md) | Xmake build system guide |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Guidelines for contributing to the project |
| [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) | Detailed codebase organization and architecture |
| [OPTIMIZATION.md](OPTIMIZATION.md) | Performance optimizations and best practices |
## Configuration

### Settings

Dumper-7 can be configured in two ways:

1. **Compile-time settings** - Edit `Dumper/Settings.h`
2. **Runtime settings** - Create `Dumper-7.ini` file (see [Config File](#config-file))

### Config File

You can dynamically change settings through a `Dumper-7.ini` file:

- **Per-game**: Place in the same directory as the game's executable
- **Global**: Place in `C:\Dumper-7\`

**Example `Dumper-7.ini`:**
```ini
[Settings]
SleepTimeout=100
SDKNamespaceName=MyOwnSDKNamespace
```

### Common Settings

| Setting | Description | Default |
|---------|-------------|---------|
| `SDKGenerationPath` | Output directory for generated SDK | `C:\Dumper-7` |
| `SDKNamespaceName` | Namespace for SDK classes | `SDK` |
| `ParamNamespaceName` | Namespace for function parameters | `Params` |
| `bAddFinalSpecifier` | Add `final` to classes with no children | `true` |

See `Dumper/Settings.h` for all available settings.

## Support the Developer

If you find Dumper-7 useful, consider supporting the original developer:

**Donate:**
- KoFi: https://ko-fi.com/fischsalat
- Patreon: https://www.patreon.com/u119629245

**Crypto:**
- LTC: `LLtXWxDbc5H9d96VJF36ZpwVX6DkYGpTJU`
- BTC: `1DVDUMcotWzEG1tyd1FffyrYeu4YEh7spx`

## Advanced Configuration

### Overriding GObjects-Layout

⚠️ **Only add a new layout if GObjects isn't automatically found for your game.**

Layouts are defined in `ObjectArray.cpp` (around line 30):

<details>
<summary><b>UE4.11 - UE4.20 Layout</b></summary>

Add to `FFixedUObjectArrayLayouts`:

```cpp
FFixedUObjectArrayLayout // Default UE4.11 - UE4.20
{
    .ObjectsOffset = 0x0,
    .MaxObjectsOffset = 0x8,
    .NumObjectsOffset = 0xC
}
```
</details>

<details>
<summary><b>UE4.21+ and UE5 Layout</b></summary>

Add to `FChunkedFixedUObjectArrayLayouts`:

```cpp
FChunkedFixedUObjectArrayLayout // Default UE4.21 and above
{
    .ObjectsOffset = 0x00,
    .MaxElementsOffset = 0x10,
    .NumElementsOffset = 0x14,
    .MaxChunksOffset = 0x18,
    .NumChunksOffset = 0x1C,
}
```
</details>

### Overriding Offsets

⚠️ **Only override offsets if the generator doesn't find them or if they are incorrect.**

All overrides are made in `Generator::InitEngineCore()` inside `Generator.cpp`:

<details>
<summary><b>GObjects Override</b></summary>

```cpp
// Basic override
ObjectArray::Init(/*GObjectsOffset*/, /*ChunkSize*/, /*bIsChunked*/);

// With decryption
InitObjectArrayDecryption([](void* ObjPtr) -> uint8* { 
    return reinterpret_cast<uint8*>(uint64(ObjPtr) ^ 0x8375); 
});
```
</details>

<details>
<summary><b>FName::AppendString Override</b></summary>

```cpp
// Force GNames (useful if AppendString offset is wrong)
FName::Init(/*bForceGNames*/);

// Override offset
FName::Init(/*OverrideOffset, OverrideType=[AppendString, ToString, GNames], bIsNamePool*/);
```
</details>

<details>
<summary><b>ProcessEvent Override</b></summary>

```cpp
Off::InSDK::InitPE(/*PEIndex*/);
```
</details>
## Troubleshooting

### Common Issues

<details>
<summary><b>Game crashes during dumping</b></summary>

1. Attach Visual Studio debugger to the game process
2. Inject `Dumper-7.dll` in **Debug** configuration
3. When crash occurs, note the exception and callstack
4. Create an issue with:
   - Exception details screenshot
   - Callstack screenshot
   - Console output
   - Game information (name, version, UE version)
</details>

<details>
<summary><b>SDK compilation errors</b></summary>

**Note:** Only **build errors** matter; IntelliSense often shows false positives.

1. In Visual Studio, go to **Error List**
2. Select **"Build Only"** from dropdown (not "Build + IntelliSense")
3. Screenshot the **first error** (often causes chain reactions)
4. Screenshot the code causing the error
5. Create an issue with screenshots
</details>

<details>
<summary><b>Generated SDK crashes at runtime</b></summary>

1. Verify your code is correct (common mistake)
2. Check that you're using the SDK correctly (see [UsingTheSDK.md](UsingTheSDK.md))
3. Ensure all required `.cpp` files are included in your project
4. If still crashing, verify offsets are correct
</details>

<details>
<summary><b>GObjects not found</b></summary>

See [Overriding GObjects-Layout](#overriding-gobjects-layout) to add your game's layout.
</details>

### Getting Help

**Before creating an issue:**
1. Read the documentation thoroughly
2. Check existing issues for similar problems
3. Verify you're using the latest version

**When creating an issue:**
- Use the issue template
- Provide all requested information
- Include screenshots/logs
- Be detailed and specific

See the [Issues](#issues) section below for detailed reporting guidelines.

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for:
- Development environment setup
- Code style guidelines
- Submission process
- Testing requirements

## Project Architecture

For developers wanting to understand or contribute to Dumper-7:

- **[PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)** - Detailed codebase organization and architecture
- **[OPTIMIZATION.md](OPTIMIZATION.md)** - Performance optimization details and best practices

### Key Components

```
Dumper-7 Architecture:
├── Engine Module      → Unreal Engine object parsing
├── Generator Module   → SDK file generation
│   ├── Managers       → Dependency and resource management
│   ├── Generators     → Format-specific output (C++, mappings, etc.)
│   └── Wrappers       → Type-safe UE object wrappers
├── Platform Module    → OS and architecture-specific code
└── Utils Module       → Shared utilities (compression, encoding)
```

See [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) for details.

## Issues

If you have any issues using Dumper, please create an issue on this repository and explain the problem **in detail**.

### Issue Reporting Guidelines

<details>
<summary><b>For Game Crashes</b></summary>

Attach Visual Studio's debugger to the game and inject `Dumper-7.dll` in debug configuration.

**Include:**
- Screenshots of the exception causing the crash
- Screenshot of the callstack
- Console output
- Game information (name, version, UE version if known)
</details>

<details>
<summary><b>For Compiler Errors in SDK</b></summary>

**Important:** Only **build errors** are relevant. IntelliSense often reports false positives.

**Include:**
- Screenshot of the **first error** in the build output (causes chain reactions)
- Screenshot of the code causing the error
- Build configuration (Debug/Release, x64)
- Compiler version (MSVC/Clang/GCC)
</details>

<details>
<summary><b>For SDK Runtime Crashes</b></summary>

**First:** Verify your code thoroughly to ensure the error lies within the generated SDK, not your code.

**Include:**
- Minimal code example reproducing the crash
- Exception details
- Callstack
- Which SDK function/class causes the crash
</details>

## License

This project is provided as-is for educational purposes. Use responsibly and respect game developers' terms of service.

## Acknowledgments

- Original Dumper-7 by **[Fischsalat](https://github.com/Encryqed)**
- Contributors to this fork
- Unreal Engine community for research and documentation

---

<div align="center">

**[Documentation](UsingTheSDK.md)** • **[Issues](https://github.com/kamer1337/Dumper-7-ml/issues)** • **[Contributing](CONTRIBUTING.md)**

</div>
