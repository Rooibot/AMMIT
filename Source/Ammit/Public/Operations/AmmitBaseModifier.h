// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "AmmitLog.h"
#include "UObject/Object.h"
#include "Data/AmmitModifierState.h"
#include "AmmitBaseModifier.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=("AMMIT"), Abstract, EditInlineNew)
class AMMIT_API UAmmitBaseModifier : public UObject
{
	GENERATED_BODY()

public:
	UAmmitBaseModifier() {};
	
	/// If false, this will call the native implementations of operation stages directly, rather than incurring
	/// the Blueprint event cost. This will be true for all subclasses by default; only C++ versions can override
	/// to disable blueprint events.
	bool bUseBlueprintEvents { true };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, SimpleDisplay, Category="Settings")
	/// A name for this specific instanced operation, used in the AMMIT logfiles.
	FString OperationName { TEXT("") };

	FString GetOperationName() const;
	
	UFUNCTION(BlueprintNativeEvent)
	FString GetDefaultOperationName() const;
	
	UFUNCTION(BlueprintNativeEvent)
	bool InitializeOperation(const FAmmitModifierState& Context);

	UFUNCTION(BlueprintNativeEvent)
	bool PreProcessAnimation(const FAmmitModifierState& Context);

	UFUNCTION(BlueprintNativeEvent)
	void ProcessAnimation(const FAmmitModifierState& Context);
	
	UFUNCTION(BlueprintNativeEvent)
	void PostProcessAnimation(const FAmmitModifierState& Context);

	UFUNCTION(BlueprintNativeEvent)
	void FinalReport(const FAmmitModifierState& Context);

	UFUNCTION(BlueprintCallable)
	void AddToLog(const FAmmitModifierState& State, EAmmitLogVerbosity Level, const FString String);

	static bool GetBoneTransformAtTime(const UAnimSequence *Sequence, const FBoneReference& Bone, float Time, FTransform& Transform, bool bWalkParents = true);

	UFUNCTION(BlueprintCallable)
	void MarkStateDirty(const FAmmitModifierState& Context);
	
	void AddToLog(const FAmmitModifierState& State, EAmmitLogVerbosity Level, const FText& Text)
	{
		AddToLog(State, Level, Text.ToString());
	}
	
	// Wrappers will call either the native event version -- allowing either native or Blueprint code to be executed --
	// or if bUseBlueprintEvents is false, will call the native implementations directly. These are what AMMIT will
	// actually call to execute an operation.
	bool InitializeOperation_Wrapper(const FAmmitModifierState& Context);
	bool PreProcessAnimation_Wrapper(const FAmmitModifierState& Context);
	void ProcessAnimation_Wrapper(const FAmmitModifierState& Context);
	void PostProcessAnimation_Wrapper(const FAmmitModifierState& Context);
	void FinalReport_Wrapper(const FAmmitModifierState& Context);

	FString GetDefaultOperationName_Wrapper() const;
};
