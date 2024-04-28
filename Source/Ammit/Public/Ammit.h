// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeCategories.h"
#include "Modules/ModuleManager.h"

class FAmmitFlowAssetTypeActions;

class FAmmitModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	EAssetTypeCategories::Type GetAmmitCategoryBit() const { return AmmitTypeCategoryBit; }
	
private:
	TSharedPtr<FAmmitFlowAssetTypeActions> AmmitFlowAssetTypeActions;

	EAssetTypeCategories::Type AmmitTypeCategoryBit;	
};
