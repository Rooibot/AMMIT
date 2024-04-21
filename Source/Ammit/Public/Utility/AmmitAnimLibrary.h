// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "Ammit.h"
#include "AmmitLog.h"
#include "Data/AmmitModifierState.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AmmitAnimLibrary.generated.h"

/**
 * 
 */
UCLASS(meta=(DisplayName="AMMIT Animation Library"))
class AMMIT_API UAmmitAnimLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static bool WriteCurveTrackFromKeys(UAnimSequence* Sequence, const FName TrackName,
		TArray<FAmmitCurvePoint> CurvePoints, const FAmmitModifierState Context,
		ERichCurveInterpMode InterpMode = RCIM_Cubic, ERichCurveTangentMode TangentMode = RCTM_Auto,
		ERichCurveExtrapolation PreExtrapolation = RCCE_CycleWithOffset, ERichCurveExtrapolation PostExtrapolation = RCCE_CycleWithOffset);

private:
	static void LogMessageInternal(const FString& Message, EAmmitLogVerbosity Level = EAmmitLogVerbosity::Log, const FAmmitModifierState* Context = nullptr);
	static void LogMessageInternal(const FText& Message, EAmmitLogVerbosity Level = EAmmitLogVerbosity::Log, const FAmmitModifierState* Context = nullptr)
	{
		LogMessageInternal(Message.ToString(), Level, Context);
	}
};
