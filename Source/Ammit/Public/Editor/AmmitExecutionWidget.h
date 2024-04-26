// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "Flow/AmmitBaseFlow.h"

DECLARE_DELEGATE_OneParam(FOnDirectoryChanged, FString)
DECLARE_DELEGATE_OneParam(FOnUseTargetDirectoryChanged, bool)
DECLARE_DELEGATE(FOnExecuteFlow)

/**
 *
 */
class AMMIT_API SAmmitExecutionWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SAmmitExecutionWidget)
        : _Flow(nullptr) {}
    SLATE_ATTRIBUTE(TObjectPtr<UAmmitBaseFlow>, Flow)
    SLATE_EVENT(FOnDirectoryChanged, OnSourceDirectoryChanged)
    SLATE_EVENT(FOnDirectoryChanged, OnTargetDirectoryChanged)
    SLATE_EVENT(FOnUseTargetDirectoryChanged, OnUseTargetDirectoryChanged)
    SLATE_EVENT(FOnExecuteFlow, OnExecuteFlow)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    bool HasValidFlow() const;
    bool CanSetDirectories() const;
    ECheckBoxState GetUseTargetFolder() const;
    void OnUseTargetFolderChanged(ECheckBoxState NewState);

    FText GetSourceFolder() const;
    FText GetTargetFolder() const;

    EVisibility GetDestinationVisibility() const;

    FReply OnSourceBrowseClicked();
    FReply OnTargetBrowseClicked();
    FReply OnExecuteFlowClicked();

    TAttribute<TObjectPtr<UAmmitBaseFlow>> Flow;
    FOnDirectoryChanged OnSourceDirectoryChanged;
    FOnDirectoryChanged OnTargetDirectoryChanged;
    FOnUseTargetDirectoryChanged OnUseTargetDirectoryChanged;
    FOnExecuteFlow OnExecuteFlow;

    bool bUseSeparateDestination { false };
    TSharedPtr<SBox> DestinationWidgets;
};

