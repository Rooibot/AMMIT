// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "AmmitBaseModifier.h"
#include "CompressionModifier.generated.h"

/**
 * 
 */
UCLASS()
class AMMIT_API UCompressionModifier : public UAmmitBaseModifier
{
	GENERATED_BODY()

public:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Settings")
	UAnimCurveCompressionSettings* CompressionSettings;

	TArray<TWeakObjectPtr<UAnimSequence>> SequencesToCompress;
	
	virtual bool InitializeOperation_Implementation(const FAmmitModifierState& Context) override;
	virtual bool PreProcessAnimation_Implementation(const FAmmitModifierState& Context) override;
	virtual void ProcessAnimation_Implementation(const FAmmitModifierState& Context) override;
	virtual void PostProcessAnimation_Implementation(const FAmmitModifierState& Context) override;
	virtual void FinalReport_Implementation(const FAmmitModifierState& Context) override;
	
};
