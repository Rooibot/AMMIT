// Copyright 2024 Rooibot Games, LLC


#include "Operations/RotationAmountModifier.h"

#include "AnimationBlueprintLibrary.h"
#include "Utility/AmmitAnimLibrary.h"

bool URotationAmountModifier::InitializeOperation_Implementation(const FAmmitModifierState& Context)
{
	return true;
}

bool URotationAmountModifier::PreProcessAnimation_Implementation(const FAmmitModifierState& Context)
{
	if (Context.DetectedSequenceType != EAmmitSequenceType::TurnInPlace && Context.DetectedSequenceType != EAmmitSequenceType::Pivot)
	{
		return false;
	}

	if (Context.RootPoints.Num() < 2) { return false; }

	RotationAmountTrack.Empty();
	FootLock_L.Empty();
	FootLock_R.Empty();
	
	FAmmitCurvePoint Point;
	float MaxRotation = 0.f;
	
	for (int32 Idx = 0; Idx < Context.RootPoints.Num(); Idx++)
	{
		int32 NextIdx = Idx + 1;
		float Value = 0.f;
		if (NextIdx < Context.RootPoints.Num())
		{
			Value = Context.RootPoints[NextIdx].DeltaTransform.GetRotation().Rotator().Yaw;
		}
		Point.KeyTime = Context.RootPoints[Idx].Time;
		Point.Value = Value;

		if (FMath::Abs(Value) > MaxRotation) MaxRotation = FMath::Abs(Value);

		RotationAmountTrack.Add(Point);
	}
	
	return RotationAmountTrack.Num() > 1 && MaxRotation >= 1.f;
}

void URotationAmountModifier::ProcessAnimation_Implementation(const FAmmitModifierState& Context)
{
	UAmmitAnimLibrary::WriteCurveTrackFromKeys(Context.AnimationSequence, FName(TEXT("RotationAmount")), RotationAmountTrack, Context);
}

void URotationAmountModifier::PostProcessAnimation_Implementation(const FAmmitModifierState& Context)
{

}

void URotationAmountModifier::FinalReport_Implementation(const FAmmitModifierState& Context)
{

}
