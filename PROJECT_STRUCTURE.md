# Project Structure

This document provides an overview of the Dumper-7 codebase organization and architecture.

## Directory Layout

```
Dumper-7-ml/
├── Dumper/                    # Main source code
│   ├── Engine/               # Unreal Engine core functionality
│   │   ├── Public/          # Engine headers (exposed API)
│   │   └── Private/         # Engine implementation
│   ├── Generator/           # SDK generation logic
│   │   ├── Public/          # Generator headers
│   │   │   ├── Generators/  # Different output format generators
│   │   │   ├── Managers/    # Resource and dependency managers
│   │   │   └── Wrappers/    # Type-safe wrappers for UE objects
│   │   └── Private/         # Generator implementations
│   ├── Platform/            # Platform-specific code
│   │   ├── Public/          # Platform API headers
│   │   └── Private/         # Windows, x86/x64 implementations
│   ├── Utils/               # Utility libraries
│   │   ├── Compression/     # ZStandard compression
│   │   ├── Dumpspace/       # Dumpspace format support
│   │   └── Encoding/        # Unicode handling
│   ├── Settings.h           # Configuration settings
│   ├── Settings.cpp         # Settings implementation
│   └── main.cpp             # Entry point (DllMain)
├── cmake/                   # CMake build scripts
├── Bin/                     # Build output (generated)
├── README.md                # Main documentation
├── UsingTheSDK.md          # SDK usage guide
├── UsingCMake.md           # CMake build guide
├── Xmake.md                # Xmake build guide
├── CONTRIBUTING.md         # Contribution guidelines
├── CMakeLists.txt          # CMake configuration
├── xmake.lua               # Xmake configuration
└── Dumper-7.sln            # Visual Studio solution

```

## Core Components

### 1. Engine Module (`Dumper/Engine/`)

Handles interaction with Unreal Engine's internal structures.

**Key Files:**
- **`UnrealObjects.h`** - Wrappers for UObject, UClass, UStruct, UEnum, etc.
- **`UnrealContainers.h`** - TArray, TMap, FString implementations
- **`ObjectArray.h`** - GObjects (global object array) management
- **`NameArray.h`** - GNames (global name array) management
- **`Enums.h`** - Unreal Engine enums (EObjectFlags, EPropertyFlags, etc.)

**Responsibilities:**
- Parsing Unreal Engine object structures
- Memory reading and pattern scanning
- Object and name table traversal
- Property and function discovery

### 2. Generator Module (`Dumper/Generator/`)

Generates SDK files in various formats.

#### Generators (`Generator/Public/Generators/`)

- **`CppGenerator`** - Generates C++ SDK files
  - Creates `.hpp` header files for classes/structs
  - Generates `.cpp` files for function implementations
  - Handles forward declarations and includes
- **`MappingGenerator`** - Generates `.usmap` mapping files for tools
- **`IDAMappingGenerator`** - Generates IDA Pro script files
- **`DumpspaceGenerator`** - Generates dumpspace format

#### Managers (`Generator/Public/Managers/`)

Resource management and dependency resolution:

- **`PackageManager`** - Manages UE packages and inter-package dependencies
  - Sorts packages by dependency order
  - Resolves circular dependencies
  - Tracks which files need which includes
  
- **`StructManager`** - Manages structs and classes within packages
  - Tracks inheritance hierarchies
  - Manages struct-to-struct dependencies
  
- **`MemberManager`** - Manages struct/class members
  - Sorts members by offset and type
  - Handles padding generation
  - Manages static vs. instance members
  
- **`DependencyManager`** - Generic dependency graph implementation
  - Topological sorting
  - Cycle detection
  - Recursive dependency traversal
  
- **`EnumManager`** - Manages enum types
  - Enum value collision detection
  - Scoped vs. unscoped enum handling
  
- **`CollisionManager`** - Handles name collisions
  - Detects conflicting names across namespaces
  - Generates unique names when needed

#### Wrappers (`Generator/Public/Wrappers/`)

Type-safe wrappers around Unreal Engine objects:

- **`StructWrapper`** - Wraps UStruct/UClass
- **`EnumWrapper`** - Wraps UEnum
- **`MemberWrappers`** - PropertyWrapper, FunctionWrapper
  - Provides type-safe access to properties
  - Handles property type conversions
  - Manages property flags and metadata

### 3. Platform Module (`Dumper/Platform/`)

Platform-specific functionality for Windows and CPU architectures.

**Key Components:**
- **`PlatformWindows.cpp`** - Windows API interactions
  - Memory reading (ReadProcessMemory)
  - Module enumeration
  - Section scanning
- **`Arch_x86.cpp`** - x86/x64 specific code
  - Pattern scanning optimizations
  - Instruction length disassembly

### 4. Utils Module (`Dumper/Utils/`)

Shared utilities used across the project.

- **`Utils.h`** - General-purpose helpers
- **`Compression/`** - ZStandard compression for mapping files
- **`Encoding/`** - Unicode/UTF-8 string handling
- **`Dumpspace/`** - Dumpspace format serialization

## SDK Generation Workflow

### High-Level Flow

```
1. Initialize Engine Core
   ↓
2. Find GObjects and GNames
   ↓
3. Iterate All UObjects
   ↓
4. Build Package Dependencies
   ↓
5. Sort Packages by Dependency
   ↓
6. Generate Files (in sorted order)
   ↓
7. Write SDK to Disk
```

### Detailed Steps

#### 1. Initialization (`main.cpp`)

```cpp
DllMain() 
  → MainThread()
    → Settings::Config::Load()        // Load Dumper-7.ini if exists
    → Generator::Generate()           // Start generation
```

#### 2. Engine Core Initialization (`Generator.cpp`)

```cpp
Generator::InitEngineCore()
  → ObjectArray::Init()               // Find GObjects
  → FName::Init()                     // Find GNames/NamePool
  → Off::InSDK::Init()               // Find offsets (ProcessEvent, etc.)
```

#### 3. Object Discovery (`Generator.cpp`)

```cpp
Generator::InitPackages()
  → Iterate GObjects
    → For each UObject:
      - Determine package ownership
      - Add to PackageManager
```

#### 4. Dependency Resolution (`PackageManager.cpp`)

```cpp
PackageManager::IterateDependencies()
  → For each package:
    - Analyze struct/class dependencies
    - Track which packages include which
    - Detect and break circular dependencies
  → Topologically sort packages
```

#### 5. Code Generation (`CppGenerator.cpp`)

```cpp
CppGenerator::Generate()
  → GenerateSDKHeader()               // SDK.hpp
  → GenerateBasicFiles()              // Basic.hpp, Basic.cpp
  → GenerateUnrealContainers()        // UnrealContainers.hpp
  → For each package (in sorted order):
    - GeneratePackageFiles()
      → GenerateStructsFile()         // Package_structs.hpp
      → GenerateClassesFile()         // Package_classes.hpp
      → GenerateParametersFile()      // Package_params.hpp
      → GenerateFunctionsFile()       // Package_functions.cpp
```

## Key Algorithms

### 1. Package Sorting Algorithm

**Purpose:** Ensure packages are included in correct order (dependencies before dependents)

**Implementation:** `PackageManager::IterateDependencies()`

**Algorithm:**
1. Build dependency graph for all packages
2. Perform topological sort using recursive DFS
3. Track separate dependencies for structs vs. classes
4. Handle circular dependencies by generating forward declarations

**Example:**
```
Engine depends on CoreUObject
Game depends on Engine, CoreUObject

Output order: CoreUObject → Engine → Game
```

### 2. Member Sorting Algorithm

**Purpose:** Sort struct/class members for correct layout and readability

**Implementation:** `ComparePredefinedMembers()` in `PredefinedMembers.h`

**Sorting Order:**
1. Static members (sorted alphabetically)
2. Instance members (sorted by offset)

**Rationale:**
- Static members have no memory offset, so alphabetical is clearest
- Instance members must match memory layout (offset order)

### 3. Function Sorting Algorithm

**Purpose:** Organize functions for readability and consistency

**Implementation:** `CompareUnrealFunctions()` / `ComparePredefinedFunctions()`

**Sorting Order:**
1. Non-inline functions
   - Static functions first
   - Instance functions second
2. Inline functions
   - Static functions first
   - Instance functions second
3. Const functions last (within each group)

**Rationale:**
- Non-inline functions go to .cpp files, inline stay in headers
- Static functions don't require an instance, group together
- Const functions are read-only, separate for clarity

### 4. Dependency Graph Traversal

**Purpose:** Visit all dependencies in correct order, handling cycles

**Implementation:** `DependencyManager::VisitAllNodesWithCallback()`

**Algorithm:**
- Recursive DFS with cycle detection
- Hit counter prevents revisiting nodes
- Callbacks allow custom actions per node

## Performance Optimizations

### 1. String Handling

- **`std::string_view`** for read-only string parameters (no copies)
- **`std::format`** for string formatting (faster than stringstream)
- **String reservation**: `str.reserve(capacity)` before appending

### 2. Container Optimizations

- **Reserve capacity**: `vector.reserve()` when size is known
- **Move semantics**: `std::move()` for expensive objects
- **In-place construction**: `emplace_back()` vs. `push_back()`

### 3. Iteration Optimizations

- **Hit counters** prevent redundant traversals in dependency graphs
- **Cached results** for expensive lookups (e.g., type resolution)
- **Early returns** using `[[likely]]` / `[[unlikely]]` attributes

### 4. Memory Layout

- **Sorted iteration** minimizes cache misses
- **Struct packing** for smaller memory footprint
- **Pool allocators** (where applicable)

## Configuration System

### Settings Namespace (`Settings.h`)

Organized into sub-namespaces:

- **`General`** - General dumper behavior
- **`Config`** - Runtime configuration (from .ini file)
- **`EngineCore`** - Engine detection settings
- **`Generator`** - SDK output paths
- **`CppGenerator`** - C++ generation options
- **`MappingGenerator`** - Mapping file options
- **`Debug`** - Debug assertions and logging
- **`Internal`** - Auto-detected engine settings (don't modify)

### Configuration Files

1. **`Settings.h`** - Compile-time defaults
2. **`Dumper-7.ini`** - Runtime overrides
   - Per-game: `GameDir/Dumper-7.ini`
   - Global: `C:/Dumper-7/Dumper-7.ini`

## Output Structure

Generated SDK structure:

```
C:/Dumper-7/GameName-GameVersion/
├── CppSDK/
│   ├── SDK.hpp                    # Main include (includes everything)
│   ├── PropertyFixup.hpp          # Property offset fixes
│   ├── NameCollisions.inl         # Name collision handling
│   ├── UnrealContainers.hpp       # TArray, TMap, etc.
│   ├── UtfN.hpp                   # Unicode utilities
│   ├── SDK/
│   │   ├── Basic.hpp              # Fundamental types
│   │   ├── Basic.cpp              # Basic implementations
│   │   ├── CoreUObject_structs.hpp
│   │   ├── CoreUObject_classes.hpp
│   │   ├── CoreUObject_functions.cpp
│   │   ├── Engine_structs.hpp
│   │   ├── Engine_classes.hpp
│   │   ├── Engine_functions.cpp
│   │   └── [Other packages...]
├── SDK/                           # Alternative flat structure
├── Mappings/
│   └── game_mappings.usmap       # Mapping file
└── Dumpspace/
    └── game.dumpspace            # Dumpspace format
```

## Testing & Debugging

### Debugging the Dumper

1. Build in **Debug** configuration
2. Attach Visual Studio debugger to target game
3. Inject Dumper-7.dll
4. Set breakpoints in generator code

### Debugging Generated SDK

1. Create test project with generated SDK
2. Include necessary SDK files
3. Test SDK functionality in game context
4. Check for compilation errors or runtime crashes

### Common Debug Scenarios

- **Dumper crashes**: Check object iteration, null checks
- **SDK won't compile**: Check forward declarations, includes
- **SDK functions crash**: Verify offsets, signatures
- **Wrong types generated**: Check property type resolution

## Extension Points

To add new functionality:

### 1. New Output Format

Create new generator class inheriting from base generator pattern:
- Add to `Dumper/Generator/Public/Generators/`
- Implement generation logic
- Call from `Generator::Generate()`

### 2. New Property Type

1. Add enum to `EPropertyType` in `Enums.h`
2. Handle in `PropertyWrapper` type resolution
3. Add C++ type mapping in generator

### 3. New Engine Version Support

1. Update `Settings::Internal` flags as needed
2. Add new layout to `ObjectArray.cpp` if GObjects layout changed
3. Test with game using that engine version

## Further Reading

- [README.md](README.md) - Project overview and basic usage
- [UsingTheSDK.md](UsingTheSDK.md) - Guide for using generated SDK
- [CONTRIBUTING.md](CONTRIBUTING.md) - How to contribute
- [OPTIMIZATION.md](OPTIMIZATION.md) - Performance optimization details
