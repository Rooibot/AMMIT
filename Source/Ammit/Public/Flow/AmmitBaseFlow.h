// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AmmitLog.h"
#include "Data/AmmitModifierState.h"
#include "AmmitBaseFlow.generated.h"

class UAmmitBaseModifier;

/**
 * 
 */
UCLASS(BlueprintType, meta=(DisplayName="Ammit Flow"))
class AMMIT_API UAmmitBaseFlow : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	/// Default path to use for the Source Folder when executing a Flow. Not exposed as a default, used to save
	/// the settings from the Execution tab of the Flow editor window.
	FString SourcePath { };

	UPROPERTY()
	/// Default path to use for the Target Folder when executing a Flow. Not exposed as a default, used to save
	/// the settings from the Execution tab of the Flow editor window.
	FString TargetPath { };
	
	UPROPERTY()
	/// Whether to copy all animations into the Target Path before running operations on them. Used by the
	/// Flow editor to save the most recently-used setting.
	bool bUseSeparateTargetDirectory { false };
	
	UPROPERTY(EditAnywhere, Category="Settings")
	/// If set, the "use root motion" setting will be forced on for all processed animations.
	bool bForceRootMotion { false };

	UPROPERTY(EditAnywhere, Category="Settings")
	/// If set, the "root motion lock" setting will be forced on for all processed animations.
	bool bForceRootMotionLock { false };

	UPROPERTY(EditAnywhere, Category="Settings")
	bool bUseNamesAsContext { true };
	
	UPROPERTY(EditAnywhere, Category="Settings")
	float IdleDistanceThreshold { 10.f };

	UPROPERTY(EditAnywhere, Category="Settings")
	float IdleTurnThreshold { 40.f };

	UPROPERTY(EditAnywhere, Category="Settings")
	float JumpOrClimbThreshold { 60.f };

	UPROPERTY(EditAnywhere, Category="Settings")
	float StartStopThreshold { 50.f };

	UPROPERTY(EditAnywhere, Category="Settings")
	float GaitShiftThreshold { 150.f };
	
	UPROPERTY(EditAnywhere, Category="Settings")
	float PivotAngleThreshold { 135.f };
	
	UPROPERTY(EditDefaultsOnly, Instanced, SimpleDisplay, Category="Operations")
	TArray<TObjectPtr<UAmmitBaseModifier>> ModifierOperations;

	// State variables for while a flow is running.

	UPROPERTY(Transient)
	int32 CurrentAnimationIndex { 0 };

	UPROPERTY(Transient)
	int32 TotalAnimationCount { 0 };

	UPROPERTY(Transient)
	FText CurrentLog { };

	UPROPERTY(Transient)
	FAmmitModifierState State { };

	bool CanExecuteFlow() const;

	void MarkStateDirty() { State.bIsDirty = true; }
	
	// Used by operations to add to the running log.
	void AddToModifierLog(UAmmitBaseModifier* Modifier, EAmmitLogVerbosity Level, FString LogText);

	void AddToModifierLog(UAmmitBaseModifier* Modifier, EAmmitLogVerbosity Level, FText& LogText) 
	{
		AddToModifierLog(Modifier, Level, LogText.ToString());
	}
	
};
