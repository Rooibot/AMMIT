// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ammit.h"
#include "AmmitLog.h"
#include "Editor/AmmitFlowAssetTypeActions.h"
#include "Editor/AmmitStyle.h"

#define LOCTEXT_NAMESPACE "AMMIT"

DEFINE_LOG_CATEGORY(LogAmmit)

void FAmmitModule::StartupModule()
{
	AmmitFlowAssetTypeActions = MakeShared<FAmmitFlowAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AmmitFlowAssetTypeActions.ToSharedRef());

	const FString ProjectResourceDir = FPaths::ProjectPluginsDir() / TEXT("AMMIT/Resources");
	const FString EngineResourceDir = FPaths::EnginePluginsDir() / TEXT("AMMIT/Resources");

	// AmmitTypeCategoryBit = FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName(TEXT("AMMIT")), LOCTEXT("AmmitCategory", "AMMIT"));
	
	FAmmitStyle::Initialize();
	FAmmitStyle::ReloadTextures();
}

void FAmmitModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AmmitFlowAssetTypeActions.ToSharedRef());

	FAmmitStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAmmitModule, Ammit)