# ML Implementation Notes

## Implementation Overview

This document describes the implementation of encrypted machine learning features in Dumper-7 SDK.

## Architecture

### Core Components

1. **MLEncryption.h** (Dumper/Generator/Public/)
   - Encryption key management
   - Model encryption/decryption
   - Security utilities

2. **MLInference.h** (Dumper/Generator/Public/)
   - Tensor data structures
   - Model runtime
   - Inference utilities

3. **CppGenerator.cpp** (Modified)
   - Generates ML namespace in Basic.hpp
   - Conditional compilation based on settings

4. **Settings.h** (Modified)
   - MachineLearning namespace with feature flags

### Generated SDK Structure

When ML support is enabled, the generated SDK includes:

```cpp
namespace ML
{
    // Encryption structures
    struct EncryptionKey { ... };
    struct EncryptedModelData { ... };
    struct TensorData { ... };
    
    // Utility functions
    void DecryptModel(...);
    uint32 CalculateTensorSize(...);
    void NormalizeTensor(...);
}
```

## Design Patterns

### 1. Snowman Pattern Influence

The implementation follows common patterns found in encrypted ML systems:

- **Separation of Concerns**: Encryption, inference, and utilities are separated
- **Inline Utilities**: Performance-critical functions are inlined
- **Type Safety**: Strong typing for tensors and encrypted data
- **Security Layers**: Multiple validation points for model integrity

### 2. SDK Generation Pattern

Follows Dumper-7's existing patterns:

- **Predefined Structures**: ML types are generated like other predefined types
- **Conditional Compilation**: Uses constexpr settings for feature gating
- **Namespace Organization**: ML namespace keeps features organized
- **Documentation**: Inline documentation in generated code

### 3. Memory Management

- **User-managed**: Encryption utilities use user-provided buffers
- **No allocations**: Core functions avoid dynamic allocation
- **RAII in wrappers**: C++ wrappers (MLEncryption.h) use RAII

## Feature Details

### Encryption System

**XOR-based encryption:**
- Fast and simple
- Suitable for obfuscation
- Can be replaced with stronger encryption
- Key rotation support through separate Key/IV

**Security considerations:**
- Not cryptographically secure
- Suitable for IP protection
- Should be upgraded for sensitive data
- Provides basic model obfuscation

### Tensor System

**Design:**
- Flat memory layout for performance
- Shape array for dimension tracking
- Compatible with common ML frameworks
- Minimal overhead

**Operations:**
- Size calculation
- Normalization (min-max scaling)
- Type conversions
- Bounds checking

### Runtime System

**EncryptedModelRuntime (MLInference.h):**
- Manages model lifecycle
- Handles decryption
- Provides inference interface
- Validates model integrity

## Integration Points

### 1. With Dumper-7

```cpp
// Settings.h
namespace Settings::MachineLearning
{
    constexpr bool bEnableMLSupport = true;
}

// CppGenerator.cpp
if constexpr (Settings::MachineLearning::bEnableMLSupport)
{
    // Generate ML code
}
```

### 2. With Game SDK

```cpp
// User code
#include "SDK/Basic.hpp"

ML::EncryptionKey Key = /* ... */;
ML::EncryptedModelData Model = /* ... */;

std::vector<uint8> Decrypted(Model.OriginalSize);
ML::DecryptModel(Model, Key, Decrypted.data());
```

### 3. With Game Objects

```cpp
SDK::UWorld* World = SDK::UWorld::GetWorld();
// Extract features
float Features[3] = { X, Y, Z };
ML::TensorData Tensor = /* ... */;
ML::NormalizeTensor(Tensor);
```

## Testing Strategy

### Unit Testing

Due to Windows-specific nature:
1. Test encryption/decryption round-trip
2. Verify tensor calculations
3. Check normalization accuracy
4. Validate boundary conditions

### Integration Testing

1. Generate SDK with ML enabled
2. Compile example project
3. Run ML examples
4. Verify game integration

### Performance Testing

1. Measure encryption overhead
2. Profile tensor operations
3. Check memory usage
4. Benchmark inference

## Usage Examples

### Basic Encryption

```cpp
#include "SDK/Basic.hpp"

// Create key
ML::EncryptionKey Key;
for (int i = 0; i < 32; ++i)
    Key.Key[i] = /* random byte */;

// Encrypt model
ML::EncryptedModelData Model;
Model.OriginalSize = DataSize;
Model.EncryptedData = new uint8[DataSize];
// XOR encryption with key
```

### Tensor Operations

```cpp
// Create tensor
uint32 Shape[] = { 2, 3 };
float Data[] = { 1, 2, 3, 4, 5, 6 };

ML::TensorData Tensor;
Tensor.Shape = Shape;
Tensor.ShapeSize = 2;
Tensor.Data = Data;
Tensor.TotalElements = ML::CalculateTensorSize(Shape, 2);

// Normalize
ML::NormalizeTensor(Tensor, 1.0f, 6.0f);
```

### Complete Workflow

See `SDKExample/MLExample.cpp` for full examples.

## Future Enhancements

### Short Term

1. **Stronger encryption**
   - AES-256 implementation
   - Proper key derivation
   - Secure random number generation

2. **Extended tensor operations**
   - Matrix multiplication
   - Convolution operations
   - Activation functions

3. **Model formats**
   - ONNX support
   - TensorFlow Lite
   - Custom format

### Long Term

1. **GPU acceleration**
   - CUDA support
   - DirectML integration
   - Vulkan compute

2. **Quantization**
   - INT8 quantization
   - Mixed precision
   - Dynamic quantization

3. **Optimization**
   - SIMD operations
   - Batch processing
   - Caching strategies

## Troubleshooting

### Common Issues

1. **Compilation errors**
   - Ensure ML support is enabled
   - Check namespace usage
   - Verify include paths

2. **Runtime crashes**
   - Check buffer sizes
   - Validate tensor dimensions
   - Verify model integrity

3. **Incorrect results**
   - Verify encryption key
   - Check normalization ranges
   - Validate input data

### Debug Tips

1. Enable debug assertions
2. Use memory sanitizers
3. Profile performance
4. Log intermediate values

## References

- Dumper-7 architecture: `PROJECT_STRUCTURE.md`
- SDK usage guide: `UsingTheSDK.md`
- ML SDK guide: `ML_SDK_GUIDE.md`
- Examples: `SDKExample/MLExample.cpp`

## Contributing

When extending ML features:

1. Follow existing patterns
2. Maintain conditional compilation
3. Document all public APIs
4. Provide examples
5. Update this document

## License

ML features follow the same license as Dumper-7.

---

**Note**: This is an advanced feature requiring understanding of both ML concepts and Unreal Engine internals.
