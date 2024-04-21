// Copyright 2024 Rooibot Games, LLC


#include "AmmitFlowAssetFactory.h"

#include "Flow/AmmitBaseFlow.h"

UAmmitFlowAssetFactory::UAmmitFlowAssetFactory()
{
	SupportedClass = UAmmitBaseFlow::StaticClass();
	bCreateNew = true;
}

UObject* UAmmitFlowAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UAmmitBaseFlow>(InParent, InClass, InName, Flags, Context);
}
