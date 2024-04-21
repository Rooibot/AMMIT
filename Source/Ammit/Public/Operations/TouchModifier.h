// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "AmmitBaseModifier.h"
#include "TouchModifier.generated.h"

/**
 * This modifier does nothing other than marking the animation dirty so that it can be re-saved. This is potentially
 * useful to force animations pulled from an earlier version of the engine to be re-saved under a newer one.
 */
UCLASS(NotBlueprintable, meta=(DisplayName="Touch (Mark Dirty)"))
class AMMIT_API UTouchModifier : public UAmmitBaseModifier
{
	GENERATED_BODY()

	virtual bool InitializeOperation_Implementation(const FAmmitModifierState& Context) override;
	virtual bool PreProcessAnimation_Implementation(const FAmmitModifierState& Context) override;
	virtual void ProcessAnimation_Implementation(const FAmmitModifierState& Context) override;
	virtual void PostProcessAnimation_Implementation(const FAmmitModifierState& Context) override;
	virtual void FinalReport_Implementation(const FAmmitModifierState& Context) override;
};
