// Copyright 2024 Rooibot Games, LLC


#include "AmmitLogWidget.h"
#include "AmmitStyle.h"
#include "Flow/AmmitBaseFlow.h"

void SAmmitLogWidget::Construct(const FArguments& InArgs)
{
	Flow = InArgs._Flow;
	
	ChildSlot [
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.Padding(10.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(FMargin(4))
				[
					SNew(SProgressBar)
					.Percent_Raw(this, &SAmmitLogWidget::GetTotalProgress)
				]

				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(4.f, 4.f, 4.f, 10.f)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(60)
						[
							SNew(STextBlock)
							.TextStyle(FAmmitStyle::Get(), "Text.Bold")
							.Text(FText::FromString(TEXT("Animation:")))
						]
					]

					+SHorizontalBox::Slot()
					.FillWidth(1.f)
					[
						SNew(STextBlock)
						.TextStyle(FAmmitStyle::Get(), "Text.Normal")
						.Text_Raw(this, &SAmmitLogWidget::GetSequenceName)				
					]
				]

				// +SVerticalBox::Slot()
				// .AutoHeight()
				// .Padding(FMargin(4))
				// [
				// 	SNew(SSeparator)
				// 	.Orientation(Orient_Horizontal)
				// 	.Thickness(1.f)
				// ]
				//
				// +SVerticalBox::Slot()
				// .AutoHeight()
				// [
				// 	SNew(SProgressBar)
				// 	.Percent_Raw(this, &SAmmitLogWidget::GetSequenceProgress)
				// ]
				//
				// +SVerticalBox::Slot()
				// .AutoHeight()
				// .Padding(4.f, 4.f, 4.f, 10.f)
				// [
				// 	SNew(SHorizontalBox)
				// 	+ SHorizontalBox::Slot()
				// 	.AutoWidth()
				// 	[
				// 		SNew(SBox)
				// 		.WidthOverride(60)
				// 		[
				// 			SNew(STextBlock)
				// 			.TextStyle(FAmmitStyle::Get(), "Text.Bold")
				// 			.Text(FText::FromString(TEXT("Operation:")))
				// 		]
				// 	]
				//
				// 	+SHorizontalBox::Slot()
				// 	.FillWidth(1.f)
				// 	[
				// 		SNew(STextBlock)
				// 		.TextStyle(FAmmitStyle::Get(), "Text.Normal")
				// 		.Text_Raw(this, &SAmmitLogWidget::GetOperationName)
				// 	]
				// ]
			]
		]
		
		+SVerticalBox::Slot()
		.FillHeight(0.9f)
		[
			SNew(SBorder)
			.Padding(10.f)
			.BorderImage(FAmmitStyle::Get().GetBrush("Box.RoundedBorder"))
			[
				SAssignNew(LogOutputScrollbox, SScrollBox)
				.OnUserScrolled_Raw(this, &SAmmitLogWidget::OnUserScrolled)
				
				+ SScrollBox::Slot()
				.FillSize(1.f)
				[
					SAssignNew(LogOutputTextBlock, SMultiLineEditableText)
					.Margin(6.f)
					.TextStyle(FAmmitStyle::Get(), "Text.Mono")
					.AutoWrapText(true)
					.IsReadOnly(true)
					.OnTextChanged_Raw(this, &SAmmitLogWidget::OnLogTextChanged)
					.Text_Raw(this, &SAmmitLogWidget::GetLogText)
				]
			]
		]
	];
}

FVector2D SAmmitLogWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return SCompoundWidget::ComputeDesiredSize(LayoutScaleMultiplier);
}

TOptional<float> SAmmitLogWidget::GetTotalProgress() const
{
	const auto& State = GetState();

	if (State.TotalAnimationBatchCount == 0) return 0.f;

	return static_cast<float>(State.CurrentAnimationBatchIndex) / static_cast<float>(State.TotalAnimationBatchCount);
}

TOptional<float> SAmmitLogWidget::GetSequenceProgress() const
{
	const auto& State = GetState();

	if (State.TotalOperationCount == 0) return 0.f;
	
	return static_cast<float>(State.CurrentOperationIndex) / static_cast<float>(State.TotalOperationCount);
}

FText SAmmitLogWidget::GetSequenceName() const
{
	const auto& State = GetState();
	if (!State.bIsRunning || !State.AnimationSequence) return INVTEXT("");
	
	return FText::FromString(State.AnimationSequence->GetName());
}

FText SAmmitLogWidget::GetOperationName() const
{
	return FText::FromString(GetState().CurrentOperationName);	
}

FText SAmmitLogWidget::GetLogText() const
{
	return Flow.Get()->CurrentLog;
}

FAmmitModifierState& SAmmitLogWidget::GetState() const
{
	return Flow.Get()->State;
}

void SAmmitLogWidget::OnUserScrolled(float ScrollOffset)
{
	bShouldAutoscroll = ScrollOffset == LogOutputScrollbox->GetScrollOffsetOfEnd();
}

void SAmmitLogWidget::OnLogTextChanged(const FText& InText)
{
	if (bShouldAutoscroll)
	{
		FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
		{
			LogOutputScrollbox->ScrollToEnd();			
		}, TStatId(), nullptr, ENamedThreads::GameThread);
	}	
}
