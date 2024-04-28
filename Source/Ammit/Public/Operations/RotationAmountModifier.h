// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "AmmitBaseModifier.h"
#include "RotationAmountModifier.generated.h"

/**
 * 
 */
UCLASS()
class AMMIT_API URotationAmountModifier : public UAmmitBaseModifier
{
	GENERATED_BODY()

public:

	virtual bool InitializeOperation_Implementation(const FAmmitModifierState& Context) override;
	virtual bool PreProcessAnimation_Implementation(const FAmmitModifierState& Context) override;
	virtual void ProcessAnimation_Implementation(const FAmmitModifierState& Context) override;
	virtual void PostProcessAnimation_Implementation(const FAmmitModifierState& Context) override;
	virtual void FinalReport_Implementation(const FAmmitModifierState& Context) override;

	UPROPERTY(Transient)
	TArray<FAmmitCurvePoint> RotationAmountTrack;

	UPROPERTY(Transient)
	TArray<FAmmitCurvePoint> FootLock_L;

	UPROPERTY(Transient)
	TArray<FAmmitCurvePoint> FootLock_R;
	
};
