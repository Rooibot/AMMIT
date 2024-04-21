// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AmmitFlowAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class AMMITEDITOR_API UAmmitFlowAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UAmmitFlowAssetFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
