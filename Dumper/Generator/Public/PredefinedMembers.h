#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "Unreal/Enums.h"
#include "Unreal/UnrealObjects.h"

struct PredefinedMember
{
    std::string Comment;

    std::string Type;
    std::string Name;

    int32 Offset;
    int32 Size;
    int32 ArrayDim;
    int32 Alignment;

    bool bIsStatic;
    bool bIsZeroSizeMember;

    bool bIsBitField;
    uint8 BitIndex;
    uint8 BitCount = 0x1;

    std::string DefaultValue = std::string();
};

struct PredefinedFunction
{
    std::string CustomComment;
    std::string CustomTemplateText = std::string();
    std::string ReturnType;
    std::string NameWithParams;
    std::string NameWithParamsWithoutDefaults = std::string();

    std::string Body;

    bool bIsStatic;
    bool bIsConst;
    bool bIsBodyInline;
};

struct PredefinedElements
{
    std::vector<PredefinedMember> Members;
    std::vector<PredefinedFunction> Functions;
};

struct PredefinedStruct
{
    std::string CustomTemplateText = std::string();
    std::string UniqueName;
    int32 Size;
    int32 Alignment;
    bool bUseExplictAlignment;
    bool bIsFinal;
    bool bIsClass;
    bool bIsUnion;

    const PredefinedStruct* Super;

    std::vector<PredefinedMember> Properties;
    std::vector<PredefinedFunction> Functions;
};

/* unordered_map<StructIndex, Members/Functions> */
using PredefinedMemberLookupMapType = std::unordered_map<int32 /* StructIndex */, PredefinedElements /* Members/Functions */>;

// requires strict weak ordering
inline bool CompareUnrealProperties(UEProperty Left, UEProperty Right)
{
    if (Left.IsA(EClassCastFlags::BoolProperty) && Right.IsA(EClassCastFlags::BoolProperty))
    {
        if (Left.GetOffset() == Right.GetOffset())
        {
            return Left.Cast<UEBoolProperty>().GetBitIndex() < Right.Cast<UEBoolProperty>().GetBitIndex();
        }
    }

    return Left.GetOffset() < Right.GetOffset();
};

/**
 * @brief Comparison function for sorting predefined struct/class members
 * 
 * Sorting order:
 *   1. Static members (alphabetically by name)
 *   2. Instance members (by memory offset)
 * 
 * Rationale:
 *   - Static members have no memory offset, so alphabetical sorting provides consistency
 *   - Instance members must be sorted by offset to match actual memory layout
 *   - Grouping static members first improves SDK readability
 * 
 * Performance:
 *   - Fast path: Integer comparison for instance members (most common case)
 *   - Slow path: String comparison only when both members are static (rare)
 * 
 * @param Left First member to compare
 * @param Right Second member to compare
 * @return true if Left should come before Right in sorted order
 * @note Satisfies strict weak ordering requirements for std::sort
 */
inline bool ComparePredefinedMembers(const PredefinedMember& Left, const PredefinedMember& Right)
{
    // Both static: sort lexically by name for consistency
    if (Left.bIsStatic && Right.bIsStatic)
        return Left.Name < Right.Name;

    // One static: static members come first
    if (Left.bIsStatic || Right.bIsStatic)
        return Left.bIsStatic > Right.bIsStatic;

    // Both instance: sort by memory offset (must match actual layout)
    return Left.Offset < Right.Offset;
};

/*
Order:
    static non-inline
    non-inline
    static inline
    inline
*/

/**
 * @brief Comparison function for sorting Unreal Engine functions
 * 
 * Sorting order:
 *   1. Static functions (no instance required)
 *   2. Non-const functions (can modify state)
 *   3. Const functions (read-only operations)
 *   Within each group, sort by object index for stability
 * 
 * Rationale:
 *   - Static functions are utility functions, group first for visibility
 *   - Const functions are read-only, separate for clarity
 *   - Stable sort by index ensures consistent generation between runs
 * 
 * @param Left First function to compare
 * @param Right Second function to compare
 * @return true if Left should come before Right in sorted order
 * @note Satisfies strict weak ordering requirements for std::sort
 */
inline bool CompareUnrealFunctions(UEFunction Left, UEFunction Right)
{
    const bool bIsLeftStatic = Left.HasFlags(EFunctionFlags::Static);
    const bool bIsRightStatic = Right.HasFlags(EFunctionFlags::Static);

    const bool bIsLeftConst = Left.HasFlags(EFunctionFlags::Const);
    const bool bIsRightConst = Right.HasFlags(EFunctionFlags::Const);

    // Static functions come first (don't require instance)
    if (bIsLeftStatic != bIsRightStatic)
        return bIsLeftStatic > bIsRightStatic;

    // Const functions come last (read-only operations)
    if (bIsLeftConst != bIsRightConst)
        return bIsLeftConst < bIsRightConst;

    // Stable sort by object index
    return Left.GetIndex() < Right.GetIndex();
};

/**
 * @brief Comparison function for sorting predefined functions (manual SDK additions)
 * 
 * Sorting order:
 *   1. Non-inline functions (implementation in .cpp files)
 *      a. Static first
 *      b. Instance second
 *   2. Inline functions (implementation in headers)
 *      a. Static first
 *      b. Instance second
 *   3. Within each group, const functions last
 *   4. Finally, sort alphabetically by function signature
 * 
 * Rationale:
 *   - Non-inline separates interface (.hpp) from implementation (.cpp)
 *   - Static functions first as they're often utilities
 *   - Const functions last as they're read-only operations
 *   - Alphabetical for predictable ordering within each group
 * 
 * @param Left First function to compare
 * @param Right Second function to compare
 * @return true if Left should come before Right in sorted order
 * @note Satisfies strict weak ordering requirements for std::sort
 */
inline bool ComparePredefinedFunctions(const PredefinedFunction& Left, const PredefinedFunction& Right)
{
    // Non-inline functions come first (go in .cpp files)
    if (Left.bIsBodyInline != Right.bIsBodyInline)
        return Left.bIsBodyInline < Right.bIsBodyInline;

    // Static functions come first (don't require instance)
    if (Left.bIsStatic != Right.bIsStatic)
        return Left.bIsStatic > Right.bIsStatic;

    // Const functions come last (read-only operations)
    if (Left.bIsConst != Right.bIsConst)
        return Left.bIsConst < Right.bIsConst;

    // Alphabetical sort by signature for consistency
    return Left.NameWithParams < Right.NameWithParams;
};


