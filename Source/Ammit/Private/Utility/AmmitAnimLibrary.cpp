// Copyright 2024 Rooibot Games, LLC


#include "Utility/AmmitAnimLibrary.h"

#include "Flow/AmmitBaseFlow.h"

#define LOCTEXT_NAMESPACE "AMMIT"

bool UAmmitAnimLibrary::WriteCurveTrackFromKeys(UAnimSequence* Sequence, const FName TrackName,
	TArray<FAmmitCurvePoint> CurvePoints, const FAmmitModifierState Context, ERichCurveInterpMode InterpMode,
	ERichCurveTangentMode TangentMode, ERichCurveExtrapolation PreExtrapolation,
	ERichCurveExtrapolation PostExtrapolation)
{
	USkeleton* Skeleton = Sequence->GetSkeleton();
	Skeleton->AddCurveMetaData(TrackName);
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	Skeleton->MarkPackageDirty();

	IAnimationDataController& Controller = Sequence->GetController();
	Controller.OpenBracket(FText::FromString(TEXT("AMMIT")), false);

	FAnimationCurveIdentifier CurveUID = FAnimationCurveIdentifier(
		TrackName, ERawCurveTrackTypes::RCT_Float);

	// Delete any previous curve.
	if (Controller.GetModel()->FindCurve(CurveUID) != nullptr)
	{
		Controller.RemoveCurve(CurveUID, false);
	}

	if (!Controller.AddCurve(CurveUID))
	{
		FText Message = FText::Format(LOCTEXT("CurveGenerator.CurveAddFailed", "Could not add {1} curve to {0}."),
					FText::FromString(Sequence->GetName()), FText::FromName(TrackName));
		LogMessageInternal(Message, EAmmitLogVerbosity::Error, &Context);
		return false;			
	}

	FRichCurve Curve = FRichCurve();
	for (const auto& [KeyTime, Value] : CurvePoints)
	{
		const FKeyHandle Handle = Curve.AddKey(KeyTime, Value);
		Curve.SetKeyInterpMode(Handle, InterpMode, true);
		Curve.SetKeyTangentMode(Handle, TangentMode, true);
	}
	TArray<FRichCurveKey> KeyArray = Curve.GetCopyOfKeys();
	Controller.SetCurveKeys(CurveUID, KeyArray);

	FCurveAttributes Attributes = FCurveAttributes();
	Attributes.SetPreExtrapolation(PreExtrapolation);
	Attributes.SetPostExtrapolation(PostExtrapolation);
	Controller.SetCurveAttributes(CurveUID, Attributes);
	
	Controller.CloseBracket(false);

	return true;
}

void UAmmitAnimLibrary::RemoveAllCurveTracks(UAnimSequence* Sequence)
{
	IAnimationDataController& Controller = Sequence->GetController();
	Controller.OpenBracket(FText::FromString(TEXT("AMMIT")), false);

	Controller.RemoveAllCurvesOfType(ERawCurveTrackTypes::RCT_Float);
	
	Controller.CloseBracket(false);
}

void UAmmitAnimLibrary::LogMessageInternal(const FString& Message, EAmmitLogVerbosity Level,
                                           const FAmmitModifierState* Context)
{
	if (Context != nullptr && Context->OwnerFlow)
	{
		// We have an active Flow going, we can add to the modifier log directly.
		Context->OwnerFlow->AddToModifierLog(Context->CurrentOperation, Level, Message);
	}
	else
	{
		// Unreal logging fallback, woo.
		switch (Level) {
			case EAmmitLogVerbosity::Log:
				break;
			case EAmmitLogVerbosity::Warning:
				UE_LOG(LogAmmit, Warning, TEXT("%s"), *Message)
				break;
			case EAmmitLogVerbosity::Error:
				UE_LOG(LogAmmit, Error, TEXT("%s"), *Message)
				break;
		}
	}
}
