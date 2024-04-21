// Copyright 2024 Rooibot Games, LLC


#include "AmmitExecutionWidget.h"

#include "AmmitStyle.h"
#include "Dialogs/DlgPickPath.h"

void SAmmitExecutionWidget::Construct(const FArguments& InArgs)
{
	Flow = InArgs._Flow;
	OnSourceDirectoryChanged = InArgs._OnSourceDirectoryChanged;
	OnTargetDirectoryChanged = InArgs._OnTargetDirectoryChanged;
	OnUseTargetDirectoryChanged = InArgs._OnUseTargetDirectoryChanged;
	OnExecuteFlow = InArgs._OnExecuteFlow;

	ChildSlot [
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(4.f, 10.f, 4.f, 4.f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(50)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.IsChecked_Raw(this, &SAmmitExecutionWidget::GetUseTargetFolder)
				.OnCheckStateChanged_Raw(this, &SAmmitExecutionWidget::OnUseTargetFolderChanged)
				.IsEnabled_Raw(this, &SAmmitExecutionWidget::HasValidFlow)
			]

			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.TextStyle(FAmmitStyle::Get(), "Text.Normal")
				.Text(INVTEXT("Create new animations in separate folder"))
			]
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(4))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(50)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.TextStyle(FAmmitStyle::Get(), "Text.Normal")
					.Text(INVTEXT("Source:"))
				]
			]

			+SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SBorder)
				.BorderImage(FAmmitStyle::Get().GetBrush("Box.RoundedBorder"))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.TextStyle(FAmmitStyle::Get(), "Text.Normal")
					.Text_Raw(this, &SAmmitExecutionWidget::GetSourceFolder)
				]
			]

			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.TextStyle(FAmmitStyle::Get(), "Text.Normal")
				.Text(INVTEXT("Browse"))
				.OnClicked_Raw(this, &SAmmitExecutionWidget::OnSourceBrowseClicked)	
				.IsEnabled_Raw(this, &SAmmitExecutionWidget::CanSetDirectories)
			]
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(4))
		[
			SAssignNew(DestinationWidgets, SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			.Visibility_Raw(this, &SAmmitExecutionWidget::GetDestinationVisibility)
			[
				SNew(SHorizontalBox)

				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(50)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.TextStyle(FAmmitStyle::Get(), "Text.Normal")
						.Text(INVTEXT("Output:"))
					]
				]

				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				[
					SNew(SBorder)
					.BorderImage(FAmmitStyle::Get().GetBrush("Box.RoundedBorder"))
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.TextStyle(FAmmitStyle::Get(), "Text.Normal")
						.Text_Raw(this, &SAmmitExecutionWidget::GetTargetFolder)
					]
				]

				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.TextStyle(FAmmitStyle::Get(), "Text.Normal")
					.Text(INVTEXT("Browse"))
					.OnClicked_Raw(this, &SAmmitExecutionWidget::OnTargetBrowseClicked)	
					.IsEnabled_Raw(this, &SAmmitExecutionWidget::CanSetDirectories)
				]				
			]
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(50)
			]

			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.TextStyle(FAmmitStyle::Get(), "Text.Normal")
				.Text(INVTEXT("Execute Flow"))
				.OnClicked_Raw(this, &SAmmitExecutionWidget::OnExecuteFlowClicked)
				.IsEnabled_Raw(this, &SAmmitExecutionWidget::HasValidFlow)
			]

			+SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SBox)
				.HAlign(HAlign_Fill)
			]
			
		]
	];
	
}

bool SAmmitExecutionWidget::HasValidFlow() const
{
	return Flow.IsSet() && Flow.Get()->CanExecuteFlow();
}

bool SAmmitExecutionWidget::CanSetDirectories() const
{
	return Flow.IsSet() && !Flow.Get()->State.bIsRunning;
}

ECheckBoxState SAmmitExecutionWidget::GetUseTargetFolder() const
{
	if (!Flow.IsSet())
	{
		return ECheckBoxState::Unchecked;
	}

	return Flow.Get()->bUseSeparateTargetDirectory ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SAmmitExecutionWidget::OnUseTargetFolderChanged(ECheckBoxState NewState)
{
	// ReSharper disable once CppExpressionWithoutSideEffects
	OnUseTargetDirectoryChanged.ExecuteIfBound(NewState == ECheckBoxState::Checked);
}

FText SAmmitExecutionWidget::GetSourceFolder() const
{
	return Flow.IsSet() ? FText::FromString(Flow.Get()->SourcePath) : INVTEXT("");
}

FText SAmmitExecutionWidget::GetTargetFolder() const
{
	return Flow.IsSet() ? FText::FromString(Flow.Get()->TargetPath) : INVTEXT("");
}

EVisibility SAmmitExecutionWidget::GetDestinationVisibility() const
{
	return Flow.IsSet() ? (Flow.Get()->bUseSeparateTargetDirectory ? EVisibility::Visible : EVisibility::Collapsed) : EVisibility::Collapsed;
}

FReply SAmmitExecutionWidget::OnSourceBrowseClicked()
{
	TSharedPtr<SDlgPickPath> PickAssetPathWidget = SNew(SDlgPickPath).Title(FText::FromString("Select source directory"));

	if (EAppReturnType::Ok == PickAssetPathWidget->ShowModal())
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		OnSourceDirectoryChanged.ExecuteIfBound(PickAssetPathWidget->GetPath().ToString());
	}
	return FReply::Handled();
}

FReply SAmmitExecutionWidget::OnTargetBrowseClicked()
{
	TSharedPtr<SDlgPickPath> PickAssetPathWidget = SNew(SDlgPickPath).Title(FText::FromString("Select target directory"));

	if (EAppReturnType::Ok == PickAssetPathWidget->ShowModal())
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		OnTargetDirectoryChanged.ExecuteIfBound(PickAssetPathWidget->GetPath().ToString());
	}
	return FReply::Handled();
}

FReply SAmmitExecutionWidget::OnExecuteFlowClicked()
{
	// ReSharper disable once CppExpressionWithoutSideEffects
	OnExecuteFlow.ExecuteIfBound();
	
	return FReply::Handled();
}
