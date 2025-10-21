# SDK Example Project

This is a ready-to-compile Visual Studio project that demonstrates how to use a Dumper-7 generated SDK to interact with Unreal Engine games.

## Features

✅ **Complete Visual Studio Project** - Ready to build with VS 2019/2022  
✅ **Comprehensive Examples** - Demonstrates all major SDK features  
✅ **Optimized Build Configuration** - Uses selective includes for fast compilation  
✅ **Well-Documented Code** - Clear comments explaining each example  
✅ **Best Practices** - Follows recommended SDK usage patterns  

## Prerequisites

1. **Visual Studio 2019 or 2022** with C++ development tools
2. **Windows SDK 10.0** or later
3. **Dumper-7 Generated SDK** from your target game

## Setup Instructions

### Step 1: Generate the SDK

1. Build and inject `Dumper-7.dll` into your target game
2. Wait for SDK generation to complete
3. Find the generated SDK at `C:\Dumper-7\GameName-GameVersion\CppSDK\`

### Step 2: Copy SDK Files to Project

1. Copy the entire `CppSDK` folder contents to this `SDKExample` directory
2. Your directory structure should look like this:
   ```
   SDKExample/
   ├── Main.cpp
   ├── SDKExample.sln
   ├── SDKExample.vcxproj
   ├── README.md
   └── SDK/
       ├── SDK.hpp
       ├── Basic.hpp
       ├── Basic.cpp
       ├── CoreUObject_structs.hpp
       ├── CoreUObject_classes.hpp
       ├── CoreUObject_functions.cpp
       ├── Engine_structs.hpp
       ├── Engine_classes.hpp
       ├── Engine_functions.cpp
       └── [other package files...]
   ```

### Step 3: Configure the Visual Studio Project

1. Open `SDKExample.sln` in Visual Studio
2. Open `SDKExample.vcxproj` in a text editor
3. Find the commented section near the bottom:
   ```xml
   <!-- Uncomment these lines after copying your generated SDK:
   <ClCompile Include="SDK\Basic.cpp" />
   <ClCompile Include="SDK\CoreUObject_functions.cpp" />
   <ClCompile Include="SDK\Engine_functions.cpp" />
   -->
   ```
4. Uncomment those lines to add the required SDK .cpp files:
   ```xml
   <ClCompile Include="SDK\Basic.cpp" />
   <ClCompile Include="SDK\CoreUObject_functions.cpp" />
   <ClCompile Include="SDK\Engine_functions.cpp" />
   ```
5. Save and reload the project in Visual Studio

### Step 4: Build the Project

1. Select **x64** platform (not x86)
2. Choose **Debug** or **Release** configuration
3. Build the project (Ctrl+Shift+B)
4. Output DLL will be in `Bin\Debug\` or `Bin\Release\`

### Step 5: Inject into Game

1. Open your target game
2. Use a DLL injector to inject `SDKExample.dll`
3. A console window will appear showing example outputs

## Build Configurations

### Debug
- Full debugging symbols
- No optimizations
- Larger DLL size
- Use for development and troubleshooting

### Release
- Optimized for speed
- Smaller DLL size
- Use for final builds

## Customizing the Example

The example files contain several demonstrations:

### Main.cpp
1. **Getting Global Instances** - Accessing UEngine and UWorld
2. **Player Controller Access** - Getting the local player controller
3. **Finding Objects** - Using FindObject and FindObjectFast
4. **Type Checking** - Different methods to check object types
5. **Calling Static Functions** - Using SDK static functions
6. **Casting Objects** - Safe type casting with IsA()

### MLExample.cpp
1. **Model Encryption** - Encrypting and decrypting ML models
2. **Tensor Operations** - Creating and manipulating tensors
3. **ML with Game Objects** - Integrating ML with UE game state
4. **Complete Workflow** - End-to-end ML example

For detailed ML documentation, see [ML_SDK_GUIDE.md](../ML_SDK_GUIDE.md).

### Adding More Examples

To add your own code:

1. Modify the `MainThread` function in `Main.cpp`
2. Add additional SDK package includes if needed:
   ```cpp
   #include "SDK/YourPackage_classes.hpp"
   ```
3. Add corresponding .cpp files to the project:
   ```xml
   <ClCompile Include="SDK\YourPackage_functions.cpp" />
   ```

## Performance Tips

### Fast Compilation Strategy

The example uses **selective includes** for faster compilation:

```cpp
#include "SDK/Basic.hpp"
#include "SDK/CoreUObject_classes.hpp"
#include "SDK/Engine_classes.hpp"
```

**Benefits:**
- 10-100x faster incremental builds
- Only recompiles what you use
- Clearer dependencies

**Alternative (slower):**
```cpp
#include "SDK.hpp"  // Includes everything
```

### Adding More Packages

Only include packages you actually use:

```cpp
// Only add what you need
#include "SDK/Basic.hpp"              // Always required
#include "SDK/CoreUObject_classes.hpp"  // Almost always needed
#include "SDK/Engine_classes.hpp"    // For engine functionality
#include "SDK/GameSpecific_classes.hpp"  // For game-specific classes
```

## Troubleshooting

### Build Errors

**"Cannot open include file: 'SDK/Basic.hpp'"**
- You haven't copied the SDK files to the project directory yet
- See Step 2 above

**"Unresolved external symbol"**
- You're missing a .cpp file in your project
- Check which package the function belongs to and add the corresponding `_functions.cpp` file

**"IntelliSense errors"**
- IntelliSense often shows false positives
- Check the actual **Build Output** (not Error List)
- Select "Build Only" in Error List dropdown

### Runtime Crashes

**DLL fails to inject**
- Ensure you're injecting into the correct process
- Try running the injector as administrator
- Check that the DLL architecture (x64) matches the game

**Crash on accessing World/Engine**
- Add null checks before accessing pointers
- The game might not be fully initialized yet
- Try adding a delay after injection

**Functions crash when called**
- Verify the function signature matches the game's version
- Some games might have different UE versions
- Check if offsets are correct for your game version

## Project Structure

```
SDKExample/
├── Main.cpp                 # Example code with DllMain
├── SDKExample.sln          # Visual Studio solution
├── SDKExample.vcxproj      # Visual Studio project file
├── README.md               # This file
├── SDK/                    # (You copy the generated SDK here)
│   ├── SDK.hpp
│   ├── Basic.hpp
│   ├── Basic.cpp
│   └── [package files...]
├── Bin/                    # Build output (generated)
│   ├── Debug/
│   └── Release/
└── Obj/                    # Intermediate files (generated)
```

## Additional Resources

- [UsingTheSDK.md](../UsingTheSDK.md) - Complete SDK usage guide
- [README.md](../README.md) - Dumper-7 main documentation
- [OPTIMIZATION.md](../OPTIMIZATION.md) - Performance optimization tips

## License

This example project is provided as part of Dumper-7 and follows the same license.

## Contributing

Found an issue or want to improve the example? Please open an issue or pull request on the main Dumper-7 repository.
