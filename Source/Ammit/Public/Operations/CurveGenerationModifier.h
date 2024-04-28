// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "AmmitBaseModifier.h"
#include "CurveGenerationModifier.generated.h"


USTRUCT(BlueprintType)
struct FCurveGenerationVelocity
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Toggles")
	bool bX { true };

	UPROPERTY(EditAnywhere, Category="Toggles")
	bool bY { true };
	
	UPROPERTY(EditAnywhere, Category="Toggles")
	bool bZ { true };
};

/**
 * 
 */
UCLASS(NotBlueprintable, meta=(DisplayName="Curve Generation"))
class AMMIT_API UCurveGenerationModifier : public UAmmitBaseModifier
{
	GENERATED_BODY()

public:
	UCurveGenerationModifier();
	
	virtual bool InitializeOperation_Implementation(const FAmmitModifierState& State) override;
	virtual bool PreProcessAnimation_Implementation(const FAmmitModifierState& State) override;
	virtual void ProcessAnimation_Implementation(const FAmmitModifierState& State) override;
	virtual void PostProcessAnimation_Implementation(const FAmmitModifierState& State) override;
	virtual void FinalReport_Implementation(const FAmmitModifierState& State) override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings", meta=(DisplayName="Clear Tracks"))
	/// Erase all curves already in the animation sequence before writing our data.
	bool bEraseExistingTracks { false };
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings")
	bool bDistanceMatch { true };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings", meta=(EditCondition="bDistanceMatch", EditConditionHides))
	FName DistanceTrackName { TEXT("Distance") };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings", meta=(EditCondition="bDistanceMatch", EditConditionHides))
	FName DistanceJumpApexTrackName { TEXT("DistanceToApex") };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings", meta=(EditCondition="bDistanceMatch", EditConditionHides))
	FName DistanceJumpLandTrackName { TEXT("DistanceToGround") };
	
	TArray<FAmmitCurvePoint> DistanceMatchPoints;
	FVector MaxDistancePoint { FVector::ZeroVector };
	float MaxDistance { 0.f };
	float MinDistance { 0.f };
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings")
	bool bYawRemaining { true };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings", meta=(EditCondition="bYawRemaining", EditConditionHides))
	FName YawTrackName { TEXT("RemainingTurnYaw") };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings", meta=(EditCondition="bYawRemaining", EditConditionHides))
	FName YawWeightTrackName { TEXT("TurnYawWeight") };
	
	TArray<FAmmitCurvePoint> YawRemainingPoints;
	float MaxYawAngle { 0.f };

	TArray<FAmmitCurvePoint> YawWeightPoints;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings")
	bool bLocomotionAngle { true };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings", meta=(EditCondition="bLocomotionAngle", EditConditionHides))
	FName LocomotionAngleTrackName { TEXT("LocomotionAngle") };
	
	TArray<FAmmitCurvePoint> LocomotionAnglePoints;
	float MaxLocomotionAngle { 0.f };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings")
	bool bLinearVelocity { true };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings", meta=(EditCondition="bLinearVelocity", EditConditionHides))
	FName LinearVelocityTrackName { TEXT("LinearVelocity") };
	
	TArray<FAmmitCurvePoint> SpeedPoints;
	float MaxLinearVelocity { 0.f };
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings")
	bool bLinearAxisVelocity { true };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings", meta=(EditCondition="bLinearAxisVelocity", EditConditionHides))
	FString LinearAxisVelocityTrackPrefix { TEXT("LinearVelocity") };
	
	TArray<FAmmitCurvePoint> VelocityPointsX;
	TArray<FAmmitCurvePoint> VelocityPointsY;
	TArray<FAmmitCurvePoint> VelocityPointsZ;
	
	void WriteDistanceTrack(const FAmmitModifierState& State);
	void WriteYawTrack(const FAmmitModifierState& State);
	void WriteLocomotionAngleTrack(const FAmmitModifierState& State);
	void WriteLinearVelocityTrack(const FAmmitModifierState& State);
	void WriteSeparateVelocityTracks(const FAmmitModifierState& State);

	static void WriteKeysToTrack(const FAmmitModifierState& State, const FName& TrackName, const TArray<FAmmitCurvePoint>& Points);
	
};
