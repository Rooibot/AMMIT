// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"
#include "Data/AmmitModifierState.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Text/SMultiLineEditableText.h"

/**
 *
 */
class AMMITEDITOR_API SAmmitLogWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SAmmitLogWidget) {}
    SLATE_ATTRIBUTE(TObjectPtr<UAmmitBaseFlow>, Flow)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;


private:

    TOptional<float> GetTotalProgress() const;
    TOptional<float> GetSequenceProgress() const;
    FText GetSequenceName() const;
    FText GetOperationName() const;
    FText GetLogText() const;
    FAmmitModifierState& GetState() const;

	void OnUserScrolled(float ScrollOffset);
	void OnLogTextChanged(const FText& InText);

    TAttribute<TObjectPtr<UAmmitBaseFlow>> Flow;

    TSharedPtr<SMultiLineEditableText> LogOutputTextBlock;
    TSharedPtr<SScrollBox> LogOutputScrollbox;

	bool bShouldAutoscroll { true };

};
