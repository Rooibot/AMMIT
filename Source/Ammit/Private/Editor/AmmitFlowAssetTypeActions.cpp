// Copyright 2024 Rooibot Games, LLC


#include "AmmitFlowAssetTypeActions.h"

#include "AmmitFlowEditorToolkit.h"
#include "Flow/AmmitBaseFlow.h"

#define LOCTEXT_NAMESPACE "AMMIT"

FAmmitFlowAssetTypeActions::FAmmitFlowAssetTypeActions()
{
	Submenus = { INVTEXT("AMMIT") };
}

UClass* FAmmitFlowAssetTypeActions::GetSupportedClass() const
{
	return UAmmitBaseFlow::StaticClass();
}

FText FAmmitFlowAssetTypeActions::GetName() const
{
	return INVTEXT("Animation Processing Flow");
}

FColor FAmmitFlowAssetTypeActions::GetTypeColor() const
{
	return FColor::Green;
}

uint32 FAmmitFlowAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Animation;
}

const TArray<FText>& FAmmitFlowAssetTypeActions::GetSubMenus() const
{
	return Submenus;
}

void FAmmitFlowAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                                 const EAssetTypeActivationOpenedMethod OpenedMethod, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	MakeShared<FAmmitFlowEditorToolkit>()->InitEditor(InObjects);
}
