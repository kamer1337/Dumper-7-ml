#include <Windows.h>
#include <iostream>
#include <vector>

// Include SDK with ML support
#include "SDK/Basic.hpp"
#include "SDK/CoreUObject_classes.hpp"
#include "SDK/Engine_classes.hpp"

/*
* Machine Learning SDK Example
* Demonstrates encrypted ML model usage with the generated SDK
*/

namespace MLDemo
{
    /**
     * Example: Create and encrypt a simple ML model
     */
    void DemoModelEncryption()
    {
        std::cout << "[ML Demo] Model Encryption Example\n";
        
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
        
        // Example model data (in practice, this would be your trained model)
        std::vector<uint8> ModelData = { 0x01, 0x02, 0x03, 0x04, 0x05 };
        
        // Create encrypted model container
        ML::EncryptedModelData EncryptedModel;
        EncryptedModel.OriginalSize = static_cast<uint32>(ModelData.size());
        EncryptedModel.EncryptedSize = EncryptedModel.OriginalSize;
        EncryptedModel.ModelVersion = 1;
        EncryptedModel.EncryptedData = new uint8[ModelData.size()];
        
        // Encrypt the model data
        for (size_t i = 0; i < ModelData.size(); ++i)
        {
            uint8 KeyByte = Key.Key[i % 32];
            uint8 IVByte = Key.IV[i % 16];
            EncryptedModel.EncryptedData[i] = ModelData[i] ^ KeyByte ^ IVByte;
        }
        
        std::cout << "  Model encrypted successfully\n";
        std::cout << "  Original size: " << EncryptedModel.OriginalSize << " bytes\n";
        std::cout << "  Encrypted size: " << EncryptedModel.EncryptedSize << " bytes\n";
        
        // Decrypt the model
        std::vector<uint8> DecryptedData(EncryptedModel.OriginalSize);
        ML::DecryptModel(EncryptedModel, Key, DecryptedData.data());
        
        // Verify decryption
        bool bDecryptionSuccessful = true;
        for (size_t i = 0; i < ModelData.size(); ++i)
        {
            if (DecryptedData[i] != ModelData[i])
            {
                bDecryptionSuccessful = false;
                break;
            }
        }
        
        std::cout << "  Decryption " << (bDecryptionSuccessful ? "successful" : "failed") << "\n";
        
        // Cleanup
        delete[] EncryptedModel.EncryptedData;
    }
    
    /**
     * Example: Create and use ML tensors
     */
    void DemoTensorOperations()
    {
        std::cout << "\n[ML Demo] Tensor Operations Example\n";
        
        // Create a simple tensor (e.g., 2x3 matrix)
        uint32 Shape[] = { 2, 3 };
        float Data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
        
        ML::TensorData Tensor;
        Tensor.Shape = Shape;
        Tensor.ShapeSize = 2;
        Tensor.Data = Data;
        Tensor.TotalElements = ML::CalculateTensorSize(Shape, 2);
        
        std::cout << "  Created tensor with shape: [" << Shape[0] << ", " << Shape[1] << "]\n";
        std::cout << "  Total elements: " << Tensor.TotalElements << "\n";
        
        // Print original data
        std::cout << "  Original data: ";
        for (uint32 i = 0; i < Tensor.TotalElements; ++i)
        {
            std::cout << Tensor.Data[i] << " ";
        }
        std::cout << "\n";
        
        // Normalize tensor to [0, 1] range
        ML::NormalizeTensor(Tensor, 1.0f, 6.0f);
        
        // Print normalized data
        std::cout << "  Normalized data: ";
        for (uint32 i = 0; i < Tensor.TotalElements; ++i)
        {
            std::cout << Tensor.Data[i] << " ";
        }
        std::cout << "\n";
    }
    
    /**
     * Example: Integrate ML with game objects
     */
    void DemoMLWithGameObjects()
    {
        std::cout << "\n[ML Demo] ML Integration with Game Objects\n";
        
        try
        {
            // Get game world
            SDK::UWorld* World = SDK::UWorld::GetWorld();
            if (World)
            {
                std::cout << "  World: " << World->GetFullName() << "\n";
                
                // Example: Use ML to analyze game state
                // In a real scenario, you might extract features from the game world
                // and use them as input to an ML model
                
                std::cout << "  ML model could analyze:\n";
                std::cout << "    - Player position and state\n";
                std::cout << "    - Enemy positions\n";
                std::cout << "    - Game metrics\n";
                std::cout << "    - Performance data\n";
                
                // Create a simple feature tensor
                float Features[] = { 100.0f, 200.0f, 300.0f };  // Example: X, Y, Z coordinates
                uint32 FeatureShape[] = { 3 };
                
                ML::TensorData FeatureTensor;
                FeatureTensor.Data = Features;
                FeatureTensor.Shape = FeatureShape;
                FeatureTensor.ShapeSize = 1;
                FeatureTensor.TotalElements = 3;
                
                std::cout << "  Feature tensor created with " << FeatureTensor.TotalElements << " elements\n";
            }
            else
            {
                std::cout << "  World not available\n";
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "  Error: " << e.what() << "\n";
        }
    }
    
    /**
     * Run all ML demos
     */
    void RunAllDemos()
    {
        std::cout << "========================================\n";
        std::cout << "   ML SDK Examples\n";
        std::cout << "========================================\n\n";
        
        DemoModelEncryption();
        DemoTensorOperations();
        DemoMLWithGameObjects();
        
        std::cout << "\n========================================\n";
        std::cout << "   All ML demos completed!\n";
        std::cout << "========================================\n";
    }
}

DWORD MLExampleThread(HMODULE Module)
{
    /* Open a console window for output */
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    // Run ML demos
    MLDemo::RunAllDemos();

    std::cout << "\nPress Enter to exit...\n";
    std::cin.get();

    FreeConsole();
    FreeLibraryAndExitThread(Module, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MLExampleThread, hModule, 0, nullptr);
        break;
    
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
