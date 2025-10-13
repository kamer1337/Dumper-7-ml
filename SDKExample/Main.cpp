#include <Windows.h>
#include <iostream>

// -------------------------------------------------------
// SDK Include Strategy
// -------------------------------------------------------
// Option 1: Include everything (slower compilation)
// #include "SDK.hpp"
//
// Option 2: Include only what you need (faster compilation) - RECOMMENDED
// #include "SDK/Basic.hpp"
// #include "SDK/CoreUObject_classes.hpp"
// #include "SDK/Engine_classes.hpp"
// -------------------------------------------------------

// For this example, we'll use selective includes for faster compilation
#include "SDK/Basic.hpp"
#include "SDK/CoreUObject_classes.hpp"
#include "SDK/Engine_classes.hpp"

// Note: You must also add the following files to your Visual Studio project:
// - SDK/Basic.cpp
// - SDK/CoreUObject_functions.cpp
// - SDK/Engine_functions.cpp (if you call Engine functions)

DWORD MainThread(HMODULE Module)
{
    /* Open a console window for output */
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    std::cout << "========================================\n";
    std::cout << "   Dumper-7 SDK Example Project\n";
    std::cout << "========================================\n\n";

    try
    {
        // -------------------------------------------------------
        // Example 1: Getting Global Instances
        // -------------------------------------------------------
        std::cout << "[Example 1] Getting global instances...\n";
        
        SDK::UEngine* Engine = SDK::UEngine::GetEngine();
        if (Engine)
        {
            std::cout << "  Engine: " << Engine->GetFullName() << "\n";
        }
        else
        {
            std::cout << "  Warning: Engine is nullptr\n";
        }

        SDK::UWorld* World = SDK::UWorld::GetWorld();
        if (World)
        {
            std::cout << "  World: " << World->GetFullName() << "\n";
        }
        else
        {
            std::cout << "  Warning: World is nullptr\n";
        }

        // -------------------------------------------------------
        // Example 2: Accessing Player Controller
        // -------------------------------------------------------
        std::cout << "\n[Example 2] Accessing PlayerController...\n";
        
        if (World && World->OwningGameInstance && 
            World->OwningGameInstance->LocalPlayers.Num() > 0)
        {
            SDK::APlayerController* PlayerController = 
                World->OwningGameInstance->LocalPlayers[0]->PlayerController;
            
            if (PlayerController)
            {
                std::cout << "  PlayerController: " << PlayerController->GetFullName() << "\n";
                
                // Example: Get mouse position (if the function exists)
                // float MouseX = 0.0f, MouseY = 0.0f;
                // PlayerController->GetMousePosition(&MouseX, &MouseY);
                // std::cout << "  Mouse Position: (" << MouseX << ", " << MouseY << ")\n";
            }
            else
            {
                std::cout << "  Warning: PlayerController is nullptr\n";
            }
        }
        else
        {
            std::cout << "  Warning: Unable to access LocalPlayers\n";
        }

        // -------------------------------------------------------
        // Example 3: Finding Objects by Name
        // -------------------------------------------------------
        std::cout << "\n[Example 3] Finding objects by name...\n";
        
        // Find any object by full name
        // SDK::UObject* MyObject = SDK::UObject::FindObject("ClassName PackageName.ObjectName");
        
        // Find object quickly by name only (faster but may return wrong object if names collide)
        // SDK::UObject* MyObjectFast = SDK::UObject::FindObjectFast("ObjectName");
        
        // Find a specific type
        // SDK::UObject* MyStruct = SDK::UObject::FindObjectFast("StructName", SDK::EClassCastFlags::Struct);
        
        std::cout << "  Use SDK::UObject::FindObject() or FindObjectFast() to find objects\n";

        // -------------------------------------------------------
        // Example 4: Type Checking
        // -------------------------------------------------------
        std::cout << "\n[Example 4] Type checking examples...\n";
        
        if (World)
        {
            // Method 1: Using EClassCastFlags (fastest, limited to base types)
            bool bIsWorld = World->IsA(SDK::EClassCastFlags::World);
            std::cout << "  World->IsA(EClassCastFlags::World): " << (bIsWorld ? "true" : "false") << "\n";
            
            // Method 2: Using StaticClass() (ideal for native classes)
            // bool bIsSpecificClass = MyObject->IsA(SDK::UWorld::StaticClass());
            
            // Method 3: Using StaticName() with FName (works for all classes including blueprints)
            // bool bIsBlueprintClass = MyObject->IsA(SDK::UMyBlueprintClass_C::StaticName());
        }

        // -------------------------------------------------------
        // Example 5: Calling Static Functions
        // -------------------------------------------------------
        std::cout << "\n[Example 5] Calling static functions...\n";
        std::cout << "  Static functions can be called without an instance\n";
        std::cout << "  Example: SDK::UKismetStringLibrary::Conv_StringToName(L\"MyName\")\n";
        
        // SDK::FName MyName = SDK::UKismetStringLibrary::Conv_StringToName(L"DemoNetDriver");

        // -------------------------------------------------------
        // Example 6: Casting Objects
        // -------------------------------------------------------
        std::cout << "\n[Example 6] Casting example...\n";
        std::cout << "  Use IsA() to check type, then static_cast to desired type:\n";
        std::cout << "    if (Pawn->IsA(SDK::ACustomPawn::StaticClass()))\n";
        std::cout << "        auto* CustomPawn = static_cast<SDK::ACustomPawn*>(Pawn);\n";

        std::cout << "\n========================================\n";
        std::cout << "   All examples completed!\n";
        std::cout << "========================================\n";
        std::cout << "\nPress Enter to exit...\n";
        std::cin.get();
    }
    catch (const std::exception& e)
    {
        std::cerr << "\nError: " << e.what() << "\n";
        std::cerr << "Press Enter to exit...\n";
        std::cin.get();
    }

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
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);
        break;
    
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
