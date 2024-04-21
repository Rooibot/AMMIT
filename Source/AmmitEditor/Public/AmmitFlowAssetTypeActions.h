// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/**
 *
 */
class AMMITEDITOR_API FAmmitFlowAssetTypeActions : public FAssetTypeActions_Base
{
public:
	FAmmitFlowAssetTypeActions();

	virtual UClass* GetSupportedClass() const override;
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual const TArray<FText>& GetSubMenus() const override;

	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, const EAssetTypeActivationOpenedMethod OpenedMethod, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;

private:
	TArray<FText> Submenus;
};
