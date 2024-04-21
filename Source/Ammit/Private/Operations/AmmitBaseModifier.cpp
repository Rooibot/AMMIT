// Copyright 2024 Rooibot Games, LLC


#include "Operations/AmmitBaseModifier.h"
#include "Flow/AmmitBaseFlow.h"

bool UAmmitBaseModifier::InitializeOperation_Implementation(const FAmmitModifierState& State)
{
	return false;
}

bool UAmmitBaseModifier::PreProcessAnimation_Implementation(const FAmmitModifierState& State)
{
	return false;
}

void UAmmitBaseModifier::ProcessAnimation_Implementation(const FAmmitModifierState& State)
{
}

void UAmmitBaseModifier::PostProcessAnimation_Implementation(const FAmmitModifierState& State)
{
}

void UAmmitBaseModifier::FinalReport_Implementation(const FAmmitModifierState& State)
{
}

FString UAmmitBaseModifier::GetDefaultOperationName_Implementation() const
{
	return GetName();
}

FString UAmmitBaseModifier::GetOperationName() const
{
	if (!OperationName.IsEmpty()) return OperationName;

	return GetDefaultOperationName_Wrapper();
}

void UAmmitBaseModifier::AddToLog(const FAmmitModifierState& Context, EAmmitLogVerbosity Level, const FString Text)
{
	if (Context.OwnerFlow)
	{
		Context.OwnerFlow->AddToModifierLog(this, Level, Text);		
	}	
}

bool UAmmitBaseModifier::GetBoneTransformAtTime(const UAnimSequence* Sequence, const FBoneReference& InputBone, float Time,
	FTransform& Transform, bool bWalkParents)
{
	if (nullptr == Sequence) return false;

	USkeleton *AnimSkeleton = Sequence->GetSkeleton();
	
	if (InputBone.BoneName.ToString().ToLower() != "root")
	{
		FTransform Result = FTransform();
		FBoneReference Bone = InputBone;
		Bone.Initialize(AnimSkeleton);
		
		int32 CurrentBoneIndex = Bone.BoneIndex;
		while (CurrentBoneIndex > 0)
		{
			FTransform OutTransform = FTransform();
			FTransform ParentTransform = FTransform();

			FSkeletonPoseBoneIndex BoneIndex = FSkeletonPoseBoneIndex(CurrentBoneIndex);
			Sequence->GetBoneTransform(OutTransform, BoneIndex, Time, true);
			
			if (!bWalkParents)
			{
				Transform = OutTransform;
				return true;
			}

			FVector AccumulatedLocation = Result.GetLocation() + OutTransform.GetLocation();
			CurrentBoneIndex = AnimSkeleton->GetReferenceSkeleton().GetParentIndex(CurrentBoneIndex);
			const FSkeletonPoseBoneIndex ParentBoneIndex = FSkeletonPoseBoneIndex(CurrentBoneIndex);
			
			Sequence->GetBoneTransform(ParentTransform, ParentBoneIndex, Time, true);
			AccumulatedLocation = ParentTransform.Rotator().RotateVector(AccumulatedLocation);
			Result.SetLocation(AccumulatedLocation);
			Result.SetRotation(OutTransform.GetRotation() * Result.GetRotation());
		}

		Transform = Result;
		return true;
	}
	else
	{
		if (Sequence->HasRootMotion())
		{
			Transform = Sequence->ExtractRootMotionFromRange(0.0f, Time);
			return true;
		}

		return false;
	}	
}

void UAmmitBaseModifier::MarkStateDirty(const FAmmitModifierState& Context)
{
	if (Context.OwnerFlow)
	{
		Context.OwnerFlow->MarkStateDirty();
	}
}

bool UAmmitBaseModifier::InitializeOperation_Wrapper(const FAmmitModifierState& State)
{
	if (!bUseBlueprintEvents) return InitializeOperation_Implementation(State);

	return InitializeOperation(State);
}

bool UAmmitBaseModifier::PreProcessAnimation_Wrapper(const FAmmitModifierState& State)
{
	if (!bUseBlueprintEvents) return PreProcessAnimation_Implementation(State);

	return PreProcessAnimation(State);
}

void UAmmitBaseModifier::ProcessAnimation_Wrapper(const FAmmitModifierState& State)
{
	if (!bUseBlueprintEvents)
	{
		ProcessAnimation_Implementation(State);
		return;
	}

	ProcessAnimation(State);
}

void UAmmitBaseModifier::PostProcessAnimation_Wrapper(const FAmmitModifierState& State)
{
	if (!bUseBlueprintEvents)
	{
		PostProcessAnimation_Implementation(State);
		return;
	}

	PostProcessAnimation(State);
}

void UAmmitBaseModifier::FinalReport_Wrapper(const FAmmitModifierState& State)
{
	if (!bUseBlueprintEvents)
	{
		FinalReport_Implementation(State);
		return;
	}

	FinalReport(State);
}

FString UAmmitBaseModifier::GetDefaultOperationName_Wrapper() const
{
	if (!bUseBlueprintEvents)
	{
		return GetDefaultOperationName_Implementation();
	}

	return GetDefaultOperationName();
}

