# Machine Learning SDK Guide

This guide explains how to use the encrypted machine learning features in the Dumper-7 SDK.

## Overview

The Dumper-7 SDK now includes support for **encrypted machine learning models** that can be integrated with Unreal Engine games. This feature enables:

- **Secure Model Storage**: Encrypt ML models to protect intellectual property
- **Runtime Inference**: Execute ML models within the game process
- **Game Integration**: Use ML for gameplay analysis, predictions, and automation
- **Tensor Operations**: Work with multi-dimensional data structures

## Features

### 1. Model Encryption

Protect your ML models using XOR-based encryption:

```cpp
// Create encryption key
ML::EncryptionKey Key;
for (int i = 0; i < 32; ++i)
{
    Key.Key[i] = static_cast<uint8>((i * 7 + 13) ^ 0xAB);
}
for (int i = 0; i < 16; ++i)
{
    Key.IV[i] = static_cast<uint8>((i * 11 + 17) ^ 0xCD);
}

// Encrypt model data
ML::EncryptedModelData EncryptedModel;
// ... populate and encrypt data
```

### 2. Tensor Operations

Work with multi-dimensional data:

```cpp
// Create a tensor (e.g., 2x3 matrix)
uint32 Shape[] = { 2, 3 };
float Data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };

ML::TensorData Tensor;
Tensor.Shape = Shape;
Tensor.ShapeSize = 2;
Tensor.Data = Data;
Tensor.TotalElements = ML::CalculateTensorSize(Shape, 2);

// Normalize data
ML::NormalizeTensor(Tensor, 1.0f, 6.0f);
```

### 3. Game Integration

Integrate ML with game objects:

```cpp
SDK::UWorld* World = SDK::UWorld::GetWorld();
if (World)
{
    // Extract features from game state
    float Features[] = { PlayerX, PlayerY, PlayerZ };
    uint32 FeatureShape[] = { 3 };
    
    ML::TensorData FeatureTensor;
    FeatureTensor.Data = Features;
    FeatureTensor.Shape = FeatureShape;
    FeatureTensor.ShapeSize = 1;
    FeatureTensor.TotalElements = 3;
    
    // Use tensor for inference...
}
```

## Configuration

Enable ML support in `Settings.h`:

```cpp
namespace Settings::MachineLearning
{
    constexpr bool bEnableMLSupport = true;
    constexpr bool bGenerateMLUtilities = true;
    constexpr bool bIncludeMLRuntime = true;
    constexpr bool bGenerateEncryptedModelWrappers = true;
}
```

## API Reference

### ML::EncryptionKey

Encryption key structure for model protection.

**Members:**
- `uint8 Key[32]` - 256-bit encryption key
- `uint8 IV[16]` - 128-bit initialization vector

### ML::EncryptedModelData

Container for encrypted model data.

**Members:**
- `uint8* EncryptedData` - Encrypted model bytes
- `uint32 OriginalSize` - Size of original model
- `uint32 EncryptedSize` - Size of encrypted data
- `uint32 ModelVersion` - Model version identifier

### ML::TensorData

Multi-dimensional array structure.

**Members:**
- `float* Data` - Tensor data array
- `uint32* Shape` - Dimension sizes
- `uint32 ShapeSize` - Number of dimensions
- `uint32 TotalElements` - Total element count

### ML::DecryptModel()

Decrypt an encrypted model.

**Parameters:**
- `EncryptedData` - Encrypted model container
- `Key` - Decryption key
- `OutBuffer` - Buffer for decrypted data

**Example:**
```cpp
std::vector<uint8> DecryptedData(EncryptedModel.OriginalSize);
ML::DecryptModel(EncryptedModel, Key, DecryptedData.data());
```

### ML::CalculateTensorSize()

Calculate total number of elements in a tensor.

**Parameters:**
- `Shape` - Array of dimension sizes
- `ShapeSize` - Number of dimensions

**Returns:** Total element count

**Example:**
```cpp
uint32 Shape[] = { 2, 3, 4 };
uint32 TotalSize = ML::CalculateTensorSize(Shape, 3); // Returns 24
```

### ML::NormalizeTensor()

Normalize tensor values to a specified range.

**Parameters:**
- `Tensor` - Tensor to normalize (modified in-place)
- `Min` - Current minimum value (default: 0.0f)
- `Max` - Current maximum value (default: 1.0f)

**Example:**
```cpp
ML::NormalizeTensor(Tensor, 0.0f, 255.0f); // Normalize from [0, 255] to [0, 1]
```

## Examples

### Example 1: Basic Model Encryption

```cpp
#include "SDK/Basic.hpp"

void EncryptMyModel()
{
    // Generate key
    ML::EncryptionKey Key;
    // ... initialize key

    // Load model data
    std::vector<uint8> ModelBytes = LoadModelFromFile("model.bin");

    // Encrypt
    ML::EncryptedModelData Encrypted;
    Encrypted.OriginalSize = ModelBytes.size();
    Encrypted.EncryptedData = new uint8[ModelBytes.size()];
    
    for (size_t i = 0; i < ModelBytes.size(); ++i)
    {
        Encrypted.EncryptedData[i] = 
            ModelBytes[i] ^ Key.Key[i % 32] ^ Key.IV[i % 16];
    }
}
```

### Example 2: Game State Analysis

```cpp
#include "SDK/Basic.hpp"
#include "SDK/Engine_classes.hpp"

void AnalyzeGameState()
{
    SDK::UWorld* World = SDK::UWorld::GetWorld();
    if (!World) return;

    // Extract player position
    auto* LocalPlayer = World->OwningGameInstance->LocalPlayers[0];
    auto* PlayerController = LocalPlayer->PlayerController;
    
    // Create feature tensor
    float Features[3];
    // ... extract features from game state

    ML::TensorData Input;
    Input.Data = Features;
    Input.TotalElements = 3;
    
    // Normalize and use for inference
    ML::NormalizeTensor(Input);
}
```

### Example 3: Complete Workflow

See `SDKExample/MLExample.cpp` for a complete example demonstrating:
- Model encryption/decryption
- Tensor operations
- Game integration
- Error handling

## Use Cases

### 1. Gameplay Prediction

Use ML to predict player actions or game outcomes:
- Predict enemy movement patterns
- Anticipate player decisions
- Optimize game difficulty

### 2. Behavior Analysis

Analyze player behavior for insights:
- Detect unusual patterns
- Identify playstyle preferences
- Track skill progression

### 3. Automated Testing

Use ML for intelligent game testing:
- Generate test cases
- Find edge cases
- Optimize test coverage

### 4. Performance Optimization

Optimize game performance using ML:
- Predict performance bottlenecks
- Optimize resource allocation
- Balance computational loads

## Security Considerations

### Model Protection

The XOR encryption provides basic obfuscation but is not cryptographically secure. For production:

1. **Use proper encryption**: Consider AES-256 or similar
2. **Secure key management**: Don't hardcode keys
3. **Protect model assets**: Use secure storage
4. **Validate inputs**: Sanitize all tensor inputs

### Memory Safety

When working with tensors:

```cpp
// Always verify tensor sizes
if (Tensor.TotalElements != ML::CalculateTensorSize(Tensor.Shape, Tensor.ShapeSize))
{
    // Handle error
    return;
}

// Check bounds before access
if (Index >= Tensor.TotalElements)
{
    // Handle error
    return;
}
```

## Performance Tips

### 1. Minimize Allocations

Reuse tensors when possible:
```cpp
// Good: Reuse tensor
ML::TensorData Tensor;
Tensor.Data = PreallocatedBuffer;

// Bad: Allocate each time
float* Data = new float[Size]; // Slow
```

### 2. Batch Operations

Process multiple samples together:
```cpp
// Good: Batch processing
for (int i = 0; i < BatchSize; ++i)
{
    ProcessSample(Samples[i]);
}

// Bad: One at a time with overhead
```

### 3. Use Native Types

Prefer SDK types for better performance:
```cpp
// Good
ML::TensorData Tensor;

// Less optimal
std::vector<float> Data;
```

## Troubleshooting

### Model Decryption Fails

**Symptom:** Decrypted data doesn't match original

**Solutions:**
1. Verify encryption key matches
2. Check data size is correct
3. Ensure IV is properly initialized

### Tensor Size Mismatch

**Symptom:** Calculated size doesn't match data

**Solutions:**
1. Verify shape array is correct
2. Check ShapeSize matches shape array length
3. Ensure no integer overflow

### Runtime Crashes

**Symptom:** Crash when using ML functions

**Solutions:**
1. Check for null pointers
2. Verify buffer sizes
3. Enable debug assertions
4. Use try-catch blocks

## Advanced Topics

### Custom Encryption

Implement your own encryption:

```cpp
namespace CustomML
{
    void EncryptModel(const uint8* Data, size_t Size, uint8* Output)
    {
        // Your encryption logic
    }
}
```

### Multi-Model Systems

Manage multiple models:

```cpp
struct ModelManager
{
    std::map<std::string, ML::EncryptedModelData> Models;
    
    void LoadModel(const std::string& Name, const ML::EncryptedModelData& Data)
    {
        Models[Name] = Data;
    }
};
```

### Integration with ML Frameworks

The SDK structures are compatible with common ML frameworks:

- **ONNX Runtime**: Convert TensorData to ONNX tensors
- **TensorFlow**: Use as input/output buffers
- **PyTorch**: Bridge via C++ API

## Resources

- **Example Code**: `SDKExample/MLExample.cpp`
- **Header Files**: `Dumper/Generator/Public/MLEncryption.h`, `MLInference.h`
- **Settings**: `Dumper/Settings.h` (MachineLearning namespace)

## Future Enhancements

Planned features for future releases:

- [ ] ONNX runtime integration
- [ ] GPU acceleration support
- [ ] Model compression
- [ ] Quantization support
- [ ] Automatic feature extraction
- [ ] Built-in common models

## License

This ML SDK feature is part of Dumper-7 and follows the same license terms as the main project.

---

**Note**: This is an advanced feature. Ensure you understand both ML concepts and UE internals before using in production.
