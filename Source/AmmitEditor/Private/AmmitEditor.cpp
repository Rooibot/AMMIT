// Copyright Epic Games, Inc. All Rights Reserved.

#include "AmmitEditor.h"
#include "AmmitStyle.h"

#define LOCTEXT_NAMESPACE "AMMIT"

void FAmmitEditorModule::StartupModule()
{
	AmmitFlowAssetTypeActions = MakeShared<FAmmitFlowAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AmmitFlowAssetTypeActions.ToSharedRef());

	const FString ProjectResourceDir = FPaths::ProjectPluginsDir() / TEXT("AMMIT/Resources");
	const FString EngineResourceDir = FPaths::EnginePluginsDir() / TEXT("AMMIT/Resources");

	FAmmitStyle::Initialize();
	FAmmitStyle::ReloadTextures();
}

void FAmmitEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AmmitFlowAssetTypeActions.ToSharedRef());

	FAmmitStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAmmitEditorModule, AmmitEditor)