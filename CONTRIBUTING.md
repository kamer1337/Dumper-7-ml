# Contributing to Dumper-7

Thank you for your interest in contributing to Dumper-7! This document provides guidelines and information for contributors.

## Table of Contents

- [Getting Started](#getting-started)
- [Development Environment Setup](#development-environment-setup)
- [Code Style Guidelines](#code-style-guidelines)
- [Project Structure](#project-structure)
- [Building the Project](#building-the-project)
- [Submitting Changes](#submitting-changes)
- [Reporting Issues](#reporting-issues)

## Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Dumper-7-ml.git
   cd Dumper-7-ml
   ```
3. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## Development Environment Setup

### Prerequisites

- **Visual Studio 2019/2022** (recommended) with:
  - C++ Desktop Development workload
  - C++20 or later support
  - CMake tools (optional but recommended)
- **OR** CMake 3.15+ with a compatible C++ compiler
- **OR** Xmake (see [Xmake.md](Xmake.md) for details)

### Build Systems

Dumper-7 supports multiple build systems:

1. **Visual Studio Solution** (`.sln` file)
   - Open `Dumper-7.sln` in Visual Studio
   - Select `x64-Release` or `x64-Debug` configuration
   - Build the project (Ctrl+Shift+B)

2. **CMake** (see [UsingCMake.md](UsingCMake.md))
   ```bash
   cmake -B build -S .
   cmake --build build --config Release
   ```

3. **Xmake** (see [Xmake.md](Xmake.md))
   ```bash
   xmake f -p windows -a x64 -m release
   xmake build
   ```

## Code Style Guidelines

### C++ Standards

- Use **C++20** or later features
- Prefer `constexpr` for compile-time constants
- Use modern C++ practices (smart pointers, RAII, etc.)

### Naming Conventions

- **Classes/Structs**: PascalCase (e.g., `CppGenerator`, `PackageManager`)
- **Functions**: PascalCase for public methods (e.g., `GenerateSDKHeader`)
- **Variables**: camelCase with descriptive names (e.g., `packageIndex`, `bIsStruct`)
- **Constants**: PascalCase or UPPER_SNAKE_CASE in Settings namespace
- **Boolean flags**: Prefix with `b` (e.g., `bIsValid`, `bShouldGenerate`)

### Code Organization

- **Header files** (`.h`): Class/struct declarations, inline functions
- **Implementation files** (`.cpp`): Function definitions, complex logic
- **Namespaces**: Use `Settings`, `Off`, `SDK` as appropriate
- **Comments**: Use `//` for single-line, `/* */` for multi-line

### Performance Considerations

- Use `std::string_view` for read-only string parameters
- Reserve capacity for vectors when size is known: `vec.reserve(size)`
- Use `std::move` for expensive-to-copy objects
- Prefer `const&` for passing complex objects
- Use `[[likely]]` and `[[unlikely]]` attributes for hot paths

### Example Code Style

```cpp
class ExampleGenerator
{
private:
    std::vector<int32> Indices;
    bool bIsInitialized;

public:
    ExampleGenerator() : bIsInitialized(false)
    {
        Indices.reserve(100); // Reserve capacity when known
    }

    void Initialize(std::string_view name, const std::vector<int32>& data)
    {
        if (bIsInitialized) [[unlikely]]
            return;

        // Process data...
        bIsInitialized = true;
    }

    static std::string FormatOutput(const std::string& input)
    {
        return std::format("Output: {}", input);
    }
};
```

## Project Structure

See [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) for a detailed overview of the codebase organization.

### Key Directories

- **`Dumper/`** - Main source code
  - **`Engine/`** - Unreal Engine core structures and objects
  - **`Generator/`** - SDK generation logic
  - **`Platform/`** - Platform-specific code (Windows, x86/x64)
  - **`Utils/`** - Utility functions and helpers
- **`cmake/`** - CMake configuration files
- **Documentation** - `README.md`, `UsingTheSDK.md`, etc.

## Building the Project

### Debug Build

Debug builds include:
- Symbol information for debugging
- Runtime checks and assertions
- Console output for diagnostics

```bash
# Visual Studio
# Select "x64-Debug" configuration and build

# CMake
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Xmake
xmake f -m debug
xmake build
```

### Release Build

Release builds are optimized for:
- Performance (compiler optimizations enabled)
- Smaller binary size
- Production use

```bash
# CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Xmake
xmake f -m release
xmake build
```

## Submitting Changes

### Pull Request Process

1. **Ensure your code builds** without errors or warnings
2. **Test your changes** thoroughly
3. **Update documentation** if you've changed functionality
4. **Commit your changes** with clear, descriptive messages:
   ```bash
   git add .
   git commit -m "Add feature: improved package sorting algorithm"
   ```
5. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```
6. **Create a Pull Request** on GitHub with:
   - Clear title describing the change
   - Detailed description of what was changed and why
   - Any relevant issue numbers (e.g., "Fixes #123")

### Commit Message Guidelines

- Use present tense: "Add feature" not "Added feature"
- Be concise but descriptive
- Reference issues when applicable
- Examples:
  - `Fix crash when processing encrypted objects`
  - `Add support for UE 5.4 FProperty layout`
  - `Optimize package dependency iteration`

## Reporting Issues

When reporting issues, please include:

1. **Dumper-7 version** or commit hash
2. **Game information**:
   - Game name
   - Unreal Engine version (if known)
   - Game exe file info (version, size)
3. **Issue description**:
   - What you expected to happen
   - What actually happened
   - Steps to reproduce
4. **Crash reports** (if applicable):
   - Visual Studio debugger output
   - Callstack
   - Exception information
   - Console output
5. **Build errors** (if applicable):
   - **Only build errors**, not IntelliSense warnings
   - First error in the error list
   - Code snippet causing the error

### Issue Template

```markdown
**Dumper-7 Version:** [commit hash or release version]

**Game Information:**
- Name: [Game Name]
- UE Version: [e.g., UE 4.27, UE 5.3]

**Description:**
[Clear description of the issue]

**Steps to Reproduce:**
1. [First step]
2. [Second step]
3. [...]

**Expected Behavior:**
[What should happen]

**Actual Behavior:**
[What actually happens]

**Additional Context:**
[Screenshots, logs, error messages, etc.]
```

## Development Tips

### Debugging

1. **Attach Visual Studio debugger** to the target game process
2. **Inject Dumper-7.dll** while debugger is attached
3. **Set breakpoints** in generator code to understand flow
4. **Use console output** from Settings::Config settings

### Testing Changes

1. Test with **multiple games** if possible
2. Verify generated SDK **compiles without errors**
3. Check that **SDK functions work** at runtime
4. Ensure **no performance regressions** in generation time

### Code Review Checklist

- [ ] Code compiles without warnings
- [ ] Changes are minimal and focused
- [ ] Code follows project style guidelines
- [ ] Comments explain complex logic
- [ ] Documentation updated if needed
- [ ] No hardcoded paths or magic numbers
- [ ] Memory management is correct (no leaks)
- [ ] Error handling is appropriate

## Questions?

If you have questions:
- Check existing [documentation](README.md)
- Look at [existing issues](https://github.com/kamer1337/Dumper-7-ml/issues)
- Review similar code in the project
- Create a new issue with the "question" label

Thank you for contributing to Dumper-7! 🎉
