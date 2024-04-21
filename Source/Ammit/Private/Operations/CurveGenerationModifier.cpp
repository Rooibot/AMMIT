// Copyright 2024 Rooibot Games, LLC


#include "Operations/CurveGenerationModifier.h"
#include "AmmitLog.h"
#include "Flow/AmmitBaseFlow.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utility/AmmitAnimLibrary.h"

#define LOCTEXT_NAMESPACE "AMMIT"

UCurveGenerationModifier::UCurveGenerationModifier()
{
	bUseBlueprintEvents = false;
}

bool UCurveGenerationModifier::InitializeOperation_Implementation(const FAmmitModifierState& State)
{
	if (!bDistanceMatch && !bYawRemaining && !bLinearVelocity && !bLocomotionAngle && !bLinearAxisVelocity)
	{
		// We aren't going to generate any data, we might as well exit and inform Ammit to skip us.
		AddToLog(State, EAmmitLogVerbosity::Warning,
			LOCTEXT("CurveGenerator.AllDisabled", "All operations are disabled, skipping modifier."));
		return false;
	}
	
	return true;
}

bool UCurveGenerationModifier::PreProcessAnimation_Implementation(const FAmmitModifierState& State)
{
	if (State.RootPoints.Num() < 2)
	{
		FText Message = FText::Format(LOCTEXT("CurveGenerator.NoPoints", "{0} does not have enouogh root motion keyframes to process, skipping."),
			FText::FromString(State.AnimationSequence->GetName()));
		AddToLog(State, EAmmitLogVerbosity::Warning, Message);
		return false;
	}
	
	DistanceMatchPoints.Empty();
	MaxDistance = 0.f;
	MinDistance = 999999.f;
	MaxDistancePoint = FVector::ZeroVector;
	YawRemainingPoints.Empty();
	MaxYawAngle = 0.f;
	SpeedPoints.Empty();
	MaxLinearVelocity = 0.f;
	LocomotionAnglePoints.Empty();
	MaxLocomotionAngle = 0.f;
	VelocityPointsX.Empty();
	VelocityPointsY.Empty();
	VelocityPointsZ.Empty();

	const FAmmitBonePoint& FirstPoint = State.RootPoints[0];
	for (int32 Idx = 1; Idx < State.RootPoints.Num(); Idx++)
	{
		const FAmmitBonePoint& PrevPoint = State.RootPoints[Idx - 1];
		const FAmmitBonePoint& Point = State.RootPoints[Idx];
		
		if (bDistanceMatch)
		{
			float Distance = Point.Transform.GetTranslation().Size2D();
			if (Distance > MaxDistancePoint.Size2D()) MaxDistancePoint = Point.Transform.GetTranslation();
			
			if (State.DetectedSequenceType == EAmmitSequenceType::JumpOrClimb)
			{
				Distance = Point.Transform.GetTranslation().Z;
			}

			if (Distance > MaxDistance) MaxDistance = Distance;
			if (Distance < MinDistance) MinDistance = Distance;
		}

		if (bYawRemaining)
		{
			const float Yaw = Point.Transform.GetRotation().Rotator().Yaw;
			if (FMath::Abs(Yaw) > FMath::Abs(MaxYawAngle)) MaxYawAngle = Yaw;
		}
	}
	
	return true;
}

void UCurveGenerationModifier::ProcessAnimation_Implementation(const FAmmitModifierState& State)
{
	for(int32 Idx = 1; Idx < State.RootPoints.Num(); Idx++)
	{
		const FAmmitBonePoint& Point = State.RootPoints[Idx];

		const FMatrix Matrix = Point.Transform.GetRotation().ToMatrix();
		const FVector Forward = Matrix.GetScaledAxis(EAxis::Y);
		const FVector Velocity = (Point.DeltaTransform.GetTranslation() / Point.DeltaTime);
		const float AngleDifference = UKismetMathLibrary::NormalizeAxis(UKismetMathLibrary::DegAcos(Forward.Dot(Velocity * FVector(1.f, 1.f, 0.f))));
		
		if (bDistanceMatch)
		{
			float Value = Point.Transform.GetTranslation().Size2D();

			if (State.DetectedSequenceType == EAmmitSequenceType::Stop)
			{
				Value = MaxDistancePoint.Size2D() - Value;
			}

			if (State.DetectedSequenceType == EAmmitSequenceType::JumpOrClimb)
			{
				Value = FMath::Abs(MaxDistancePoint.Z - Point.Transform.GetTranslation().Z);
			}

			if (State.DetectedSequenceType == EAmmitSequenceType::Pivot)
			{
				Value = MaxDistancePoint.Size2D() - Value;
				if (FMath::Abs(AngleDifference) >= State.OwnerFlow->PivotAngleThreshold) Value *= -1.f;
			}

			if (Point.Time > 0.f)
			{
				FAmmitCurvePoint DistanceValue;
				DistanceValue.KeyTime = Point.Time;
				DistanceValue.Value = Value;
				DistanceMatchPoints.Add(DistanceValue);						
			}
		}

		if (bYawRemaining)
		{
			const float Value = MaxYawAngle - Point.Transform.GetRotation().Rotator().Yaw;

			if (Point.Time > 0.f)
			{
				FAmmitCurvePoint YawValue;
				YawValue.KeyTime = Point.Time;
				YawValue.Value = Value;
				YawRemainingPoints.Add(YawValue);
			}
		}

		if (bLinearVelocity)
		{
			const float Value = Point.DeltaTransform.GetTranslation().Size2D() / Point.DeltaTime;

			// If we're the first key and a start (either detected or practical), write a 0 at the first key.
			if (Point.Key == 1 &&
				(Value < State.OwnerFlow->StartStopThreshold || State.DetectedSequenceType == EAmmitSequenceType::Start))
			{
				FAmmitCurvePoint SpeedValue;
				SpeedValue.KeyTime = 0;
				SpeedValue.Value = 0.f;
				SpeedPoints.Add(SpeedValue);
			}
			
			FAmmitCurvePoint SpeedValue;
			SpeedValue.KeyTime = Point.Time;
			SpeedValue.Value = Value;
			SpeedPoints.Add(SpeedValue);

			if (Value > MaxLinearVelocity) MaxLinearVelocity = Value;
		}

		if (bLocomotionAngle)
		{
			FAmmitCurvePoint AnglePoint;
			AnglePoint.KeyTime = Point.Time;
			AnglePoint.Value = AngleDifference;
			LocomotionAnglePoints.Add(AnglePoint);
			
			if (FMath::Abs(AngleDifference) > MaxLocomotionAngle) MaxLocomotionAngle = FMath::Abs(AngleDifference);
		}

		if (bLinearAxisVelocity)
		{
			FAmmitCurvePoint PointX;
			PointX.KeyTime = Point.Time;
			PointX.Value = Velocity.X;
			VelocityPointsX.Add(PointX);

			FAmmitCurvePoint PointY;
			PointY.KeyTime = Point.Time;
			PointY.Value = Velocity.Y;
			VelocityPointsY.Add(PointY);

			FAmmitCurvePoint PointZ;
			PointZ.KeyTime = Point.Time;
			PointZ.Value = Velocity.Z;
			VelocityPointsZ.Add(PointZ);
		}
	}

	// All points have been gathered.
	if (bDistanceMatch) WriteDistanceTrack(State);
	if (bYawRemaining) WriteYawTrack(State);
	if (bLinearVelocity) WriteLinearVelocityTrack(State);
	if (bLocomotionAngle) WriteLocomotionAngleTrack(State);
	if (bLinearAxisVelocity) WriteSeparateVelocityTracks(State);
}

void UCurveGenerationModifier::PostProcessAnimation_Implementation(const FAmmitModifierState& State)
{

}

void UCurveGenerationModifier::FinalReport_Implementation(const FAmmitModifierState& State)
{

}

void UCurveGenerationModifier::WriteDistanceTrack(const FAmmitModifierState& State)
{
	FName TrackName = FName(TEXT("Distance"));
	bool bShouldWriteDistanceTrack = true;

	switch(State.DetectedSequenceType)
	{
	case EAmmitSequenceType::Idle:
	case EAmmitSequenceType::TurnInPlace:
	case EAmmitSequenceType::Loop:
		bShouldWriteDistanceTrack = false;
		break;
	case EAmmitSequenceType::Unknown:
	case EAmmitSequenceType::Start:
	case EAmmitSequenceType::Stop:
	case EAmmitSequenceType::Pivot:
		bShouldWriteDistanceTrack = true;
		break;
	case EAmmitSequenceType::JumpOrClimb:
		bShouldWriteDistanceTrack = true;
		if (MaxDistancePoint.Z > 0.f)
		{
			TrackName = FName(TEXT("DistanceToApex"));
		}
		else
		{
			TrackName = FName(TEXT("DistanceToGround"));
		}
		break;
	}

	if (!bShouldWriteDistanceTrack) return;

	WriteKeysToTrack(State, TrackName, DistanceMatchPoints);
}

void UCurveGenerationModifier::WriteYawTrack(const FAmmitModifierState& State)
{
	FName TrackName = FName(TEXT("YawRemaining"));

	// Write track if we have a sufficiently large change in yaw, even if it's not turn-in-place/pivot.
	bool bShouldWriteYawTrack = FMath::Abs(MaxYawAngle) > State.OwnerFlow->IdleTurnThreshold;
	
	if (State.DetectedSequenceType == EAmmitSequenceType::Pivot ||
		State.DetectedSequenceType == EAmmitSequenceType::TurnInPlace)
	{
		// Always write track for turn-in-place and pivot.
		bShouldWriteYawTrack = true;
	}

	if (!bShouldWriteYawTrack) return;

	WriteKeysToTrack(State, TrackName, YawRemainingPoints);
}

void UCurveGenerationModifier::WriteLinearVelocityTrack(const FAmmitModifierState& State)
{
	if (State.DetectedSequenceType == EAmmitSequenceType::Idle
		|| State.DetectedSequenceType == EAmmitSequenceType::TurnInPlace) return;

	if (MaxLinearVelocity < 20.f) return;

	FName TrackName = FName(TEXT("LinearVelocity"));
	WriteKeysToTrack(State, TrackName, SpeedPoints);
}

void UCurveGenerationModifier::WriteLocomotionAngleTrack(const FAmmitModifierState& State)
{
	if (State.DetectedSequenceType == EAmmitSequenceType::Idle ||
		State.DetectedSequenceType == EAmmitSequenceType::TurnInPlace ||
		State.DetectedSequenceType == EAmmitSequenceType::JumpOrClimb) return;

	FName TrackName = FName(TEXT("LocomotionAngle"));
	WriteKeysToTrack(State, TrackName, LocomotionAnglePoints);
}

void UCurveGenerationModifier::WriteSeparateVelocityTracks(const FAmmitModifierState& State)
{
	if (State.DetectedSequenceType == EAmmitSequenceType::Idle ||
		State.DetectedSequenceType == EAmmitSequenceType::TurnInPlace) return;

	// For now we're going to just manually set X and Y to true if we're writing anything; they exist
	// as parameters so that logic could maybe be added.
	const bool bWriteX = true; 
	const bool bWriteY = true;
	const bool bWriteZ = State.DetectedSequenceType == EAmmitSequenceType::JumpOrClimb;

	if (bWriteX) WriteKeysToTrack(State, FName(TEXT("LinearVelocity_X")), VelocityPointsX);
	if (bWriteY) WriteKeysToTrack(State, FName(TEXT("LinearVelocity_Y")), VelocityPointsY);
	if (bWriteZ) WriteKeysToTrack(State, FName(TEXT("LinearVelocity_Z")), VelocityPointsZ);
}

void UCurveGenerationModifier::WriteKeysToTrack(const FAmmitModifierState& State, const FName& TrackName,
                                                const TArray<FAmmitCurvePoint>& Points)
{
	UAmmitAnimLibrary::WriteCurveTrackFromKeys(State.AnimationSequence, TrackName, Points, State);
}
