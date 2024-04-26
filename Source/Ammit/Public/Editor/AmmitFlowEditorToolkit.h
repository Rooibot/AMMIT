// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "Flow/AmmitBaseFlow.h"
#include "Toolkits/AssetEditorToolkit.h"

/**
 *
 */
class AMMIT_API FAmmitFlowEditorToolkit : public FAssetEditorToolkit
{
public:

	void InitEditor(const TArray<UObject*>& InObjects);

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	TObjectPtr<UAmmitBaseFlow> GetFlow() const;

	bool IsDetailEditorEnabled() const;

	void OnSourceDirectoryChanged(FString NewPath);
	void OnTargetDirectoryChanged(FString NewPath);
	void OnUseTargetDirectoryChanged(bool bUseSeparateDestination);
	void OnExecuteFlow();



private:

	bool TickInternal(float DeltaTime);
	
	bool GetAnimationAssets();
	
	bool AssetDataForPath(const FString& DestinationPath, FAssetData& OutAssetData) const;
	FString RelativePathForAsset(const FAssetData& Asset) const;

	void FlowLog(const FString& String, EAmmitLogVerbosity Level = EAmmitLogVerbosity::Log) const;
	void FlowLog(const FText& Text, EAmmitLogVerbosity Level = EAmmitLogVerbosity::Log) const;
	
	TObjectPtr<UAnimSequence> LoadSequenceFromAsset(const FAssetData& Data) const;
	TObjectPtr<UAnimSequence> DuplicateSequenceToPath(const TObjectPtr<UAnimSequence> SourceSequence, FString DestinationPath) const;

	bool UpdateStateForSequence(FAmmitModifierState& State, UAnimSequence* Sequence);
	bool CollectTransformTracks(FAmmitModifierState& State, UAnimSequence* Sequence, FAmmitBonePoint& FarthestPoint, FAmmitBonePoint& HighestPoint, FAmmitBonePoint& LowestPoint, float& FarthestYaw);
	void RefreshStateForSequence(FAmmitModifierState& State, UAnimSequence* Sequence);
	
	TObjectPtr<UAmmitBaseFlow> Flow { nullptr };

	TArray<FAssetData> AnimationBatch;
	TArray<TObjectPtr<UAmmitBaseModifier>> ActiveModifiers;

	FTickerDelegate InternalTickDelegate;
	FTSTicker::FDelegateHandle InternalTickDelegateHandle;

	TMap<EAmmitSequenceType, int32> SequenceTypeTally;
	
};
