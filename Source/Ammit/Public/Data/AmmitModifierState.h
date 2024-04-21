// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AmmitModifierState.generated.h"

class UAmmitBaseModifier;

UENUM(BlueprintType)
enum class EAmmitSequenceType : uint8
{
	Unknown,
	Idle,
	TurnInPlace,
	Start,
	Loop,
	Stop,
	Pivot,
	JumpOrClimb
};

USTRUCT(BlueprintType)
struct FAmmitBonePoint
{
	GENERATED_BODY()
	
	float Time { 0.f };
	float DeltaTime { 0.f };
	int32 Key { 0 };
	FTransform Transform { FTransform::Identity };
	FTransform DeltaTransform { FTransform::Identity };
};

USTRUCT(BlueprintType)
struct FAmmitCurvePoint
{
	GENERATED_BODY()
	
	float KeyTime;
	float Value;
};

class UAmmitBaseFlow;

/**
 * 
 */
USTRUCT(BlueprintType)
struct AMMIT_API FAmmitModifierState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDirty { true };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAmmitBaseFlow* OwnerFlow { nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsRunning { false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FAssetData SourceAsset;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAnimSequence* AnimationSequence { nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AnimationSequenceLength { 0.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 AnimationSequenceFrames { 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FAmmitBonePoint> RootPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EAmmitSequenceType DetectedSequenceType { EAmmitSequenceType::Unknown };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentAnimationBatchIndex { 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 TotalAnimationBatchCount { 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentOperationIndex { 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAmmitBaseModifier* CurrentOperation { nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 TotalOperationCount { 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString CurrentOperationName { };
	
};
