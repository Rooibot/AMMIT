// Copyright 2024 Rooibot Games, LLC


#include "AmmitFlowEditorToolkit.h"
#include "AmmitExecutionWidget.h"
#include "AmmitLogWidget.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet/KismetMathLibrary.h"
#include "Operations/AmmitBaseModifier.h"

#define LOCTEXT_NAMESPACE "AMMIT"

void FAmmitFlowEditorToolkit::InitEditor(const TArray<UObject*>& InObjects)
{
	Flow = Cast<UAmmitBaseFlow>(InObjects[0]);
	
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout(TEXT("AmmitFlowEditorLayout"))->
		AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split(
				FTabManager::NewSplitter()
				->SetSizeCoefficient(0.3f)
				->SetOrientation(Orient_Vertical)
				->Split(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.3f)
					->AddTab("AmmitFlowExecutionTab", ETabState::OpenedTab)
					)
				->Split(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab("AmmitFlowDetailsTab", ETabState::OpenedTab)
					)
				)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.4f)
				->AddTab("AmmitFlowOutputLog", ETabState::OpenedTab)
				)
			);

	FAssetEditorToolkit::InitAssetEditor(EToolkitMode::Standalone, {}, "AmmitFlowEditor",
		Layout, true, true, InObjects);
}

void FAmmitFlowEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("Ammit"));

	InTabManager->RegisterTabSpawner("AmmitFlowExecutionTab", FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		[
			SNew(SAmmitExecutionWidget)
			.Flow_Raw(this, &FAmmitFlowEditorToolkit::GetFlow)
			.OnUseTargetDirectoryChanged_Raw(this, &FAmmitFlowEditorToolkit::OnUseTargetDirectoryChanged)
			.OnSourceDirectoryChanged_Raw(this, &FAmmitFlowEditorToolkit::OnSourceDirectoryChanged)
			.OnTargetDirectoryChanged_Raw(this, &FAmmitFlowEditorToolkit::OnTargetDirectoryChanged)
			.OnExecuteFlow_Raw(this, &FAmmitFlowEditorToolkit::OnExecuteFlow)
		];
	}))
	.SetDisplayName(INVTEXT("Execution"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	InTabManager->RegisterTabSpawner("AmmitFlowOutputLog", FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
			[
				SNew(SAmmitLogWidget)
				.Flow_Raw(this, &FAmmitFlowEditorToolkit::GetFlow)
			];
	}))
	.SetDisplayName(INVTEXT("Output"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObjects(TArray<UObject*>{ Flow });
	DetailsView->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateRaw(this, &FAmmitFlowEditorToolkit::IsDetailEditorEnabled));
	InTabManager->RegisterTabSpawner("AmmitFlowDetailsTab", FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		[
			DetailsView
		];
	}))
	.SetDisplayName(INVTEXT("Settings"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FAmmitFlowEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

FName FAmmitFlowEditorToolkit::GetToolkitFName() const
{
	return FName(TEXT("AmmitFlowEditor"));
}

FText FAmmitFlowEditorToolkit::GetBaseToolkitName() const
{
	return INVTEXT("Ammit Flow Editor");
}

FString FAmmitFlowEditorToolkit::GetWorldCentricTabPrefix() const
{
	return TEXT("Ammit Flow Editor ");
}

FLinearColor FAmmitFlowEditorToolkit::GetWorldCentricTabColorScale() const
{
	return {};
}

TObjectPtr<UAmmitBaseFlow> FAmmitFlowEditorToolkit::GetFlow() const
{
	return Flow;
}

bool FAmmitFlowEditorToolkit::IsDetailEditorEnabled() const
{
	if (!Flow) return false;

	return !Flow->State.bIsRunning;
}

void FAmmitFlowEditorToolkit::OnSourceDirectoryChanged(FString NewPath)
{
	if (!Flow) return;

	Flow->SourcePath = NewPath;
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	Flow->MarkPackageDirty();}

void FAmmitFlowEditorToolkit::OnTargetDirectoryChanged(FString NewPath)
{
	if (!Flow) return;

	Flow->TargetPath = NewPath;
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	Flow->MarkPackageDirty();
}

void FAmmitFlowEditorToolkit::OnUseTargetDirectoryChanged(bool bUseSeparateDestination)
{
	if (!Flow) return;

	Flow->bUseSeparateTargetDirectory = bUseSeparateDestination;
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	Flow->MarkPackageDirty();
}

void FAmmitFlowEditorToolkit::OnExecuteFlow()
{
	if (!Flow) return;

	// Clear log.
	Flow->CurrentLog = FText();

	// Clear tally.
	SequenceTypeTally.Empty();

	Flow->State.OwnerFlow = Flow;
	Flow->State.bIsRunning = true;
	FText Message = LOCTEXT("LogInfo.AnimationCollecting", "Collecting animation sequences...");
	FlowLog(Message);	

	if (!GetAnimationAssets())
	{
		Message = LOCTEXT("LogInfo.AnimationEmpty", "No animations found in source directory.");
		FlowLog(Message);
		Flow->State.bIsRunning = false;
		return;
	}

	Message = FText::Format(LOCTEXT("LogInfo.AnimationsFound", "Found {0} animations."), AnimationBatch.Num());
	FlowLog(Message);
	Flow->State.TotalAnimationBatchCount = AnimationBatch.Num();
	
	FlowLog(TEXT("==================="));
	Message = LOCTEXT("LogInfo.InitializingOperations", "Initializing operations...");
	FlowLog(Message);
	FlowLog(TEXT(""));

	// Iterate modifier setup.
	ActiveModifiers.Empty();
	for (const auto& Modifier : Flow->ModifierOperations)
	{
		if (Modifier->InitializeOperation_Wrapper(Flow->State))
		{
			ActiveModifiers.Add(Modifier);
		}
		else
		{
			Flow->AddToModifierLog(Modifier, EAmmitLogVerbosity::Warning, LOCTEXT("LogInfo.OperationRefused", "Operation returned false from initialization, removing from set.").ToString());
		}
	}

	if (ActiveModifiers.Num() == 0)
	{
		// Every operation we had available bailed on us.
		Flow->AddToModifierLog(nullptr, EAmmitLogVerbosity::Error, LOCTEXT("LogInfo.ModifiersEmpty", "No valid operations available, aborting.").ToString());
		Flow->State.bIsRunning = false;
		return;
	}

	FlowLog(TEXT("==================="));
	Message = LOCTEXT("LogInfo.IteratingAnimations", "Iterating through all animations...");
	FlowLog(Message);
	FlowLog(TEXT(""));
	
	InternalTickDelegate = FTickerDelegate::CreateRaw(this, &FAmmitFlowEditorToolkit::TickInternal);
	InternalTickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(InternalTickDelegate);

}

bool FAmmitFlowEditorToolkit::TickInternal(float DeltaTime)
{
	if (Flow->State.CurrentAnimationBatchIndex < Flow->State.TotalAnimationBatchCount)
	{
		FAssetData Item = AnimationBatch[Flow->State.CurrentAnimationBatchIndex];
		Flow->State.SourceAsset = Item;

		UAnimSequence *Sequence = LoadSequenceFromAsset(Item);

		if (!Sequence)
		{
			Flow->State.CurrentAnimationBatchIndex++;
			const FText Message = FText::Format(LOCTEXT("AnimInfo.BadSource", "Unable to load animation {0}! Skipping."), FText::FromString(Item.PackageName.ToString()));
			Flow->AddToModifierLog(nullptr, EAmmitLogVerbosity::Error, Message.ToString());
			return true;
		}

		if (Flow->bUseSeparateTargetDirectory)
		{
			FString RelativePath = RelativePathForAsset(Item);
			const FString DestinationPath = Flow->TargetPath + TEXT("/") + RelativePath;

			if (FAssetData DestinationData; AssetDataForPath(DestinationPath, DestinationData))
			{
				Flow->State.CurrentAnimationBatchIndex++;
				const FText Message = FText::Format(LOCTEXT("AnimInfo.TargetExists", "Animation already exists at {0}! Skipping."), FText::FromString(DestinationPath));
				Flow->AddToModifierLog(nullptr, EAmmitLogVerbosity::Error, Message.ToString());
				return true;
			}
		
			Sequence = DuplicateSequenceToPath(Sequence, DestinationPath);
			if (!Sequence)
			{
				Flow->State.CurrentAnimationBatchIndex++;
				const FText Message = FText::Format(LOCTEXT("AnimInfo.BadTarget", "Unable to create destination animation {0}! Skipping."), FText::FromString(Item.PackageName.ToString()));
				Flow->AddToModifierLog(nullptr, EAmmitLogVerbosity::Error, Message.ToString());
				return true;
			}
		}

		bool bDirty = false;

		if (Flow->bForceRootMotion && !Sequence->bEnableRootMotion)
		{
			Sequence->bEnableRootMotion = true;
			bDirty = true;
		}

		if (Flow->bForceRootMotionLock && !Sequence->bForceRootLock)
		{
			Sequence->bForceRootLock = true;
			bDirty = true;
		}

		// We guard after we force the values on (if applicable) so that we restore the correct value on exit.
		TGuardValue<bool> RootMotionGuard(Sequence->bEnableRootMotion, true);
		TGuardValue<bool> RootMotionLockGuard(Sequence->bForceRootLock, true);
		
		if (bDirty)
		{
			// ReSharper disable once CppExpressionWithoutSideEffects
			Sequence->MarkPackageDirty();
		}

		UpdateStateForSequence(Flow->State, Sequence);
		
		TArray<TObjectPtr<UAmmitBaseModifier>> SequenceModifiers;
		
		Flow->State.TotalOperationCount = ActiveModifiers.Num();
		
		for (int32 Idx = 0; Idx < ActiveModifiers.Num(); Idx++)
		{
			auto& Modifier = ActiveModifiers[Idx];
			Flow->State.CurrentOperationIndex = Idx;
			Flow->State.CurrentOperationName = Modifier->GetOperationName();
			Flow->State.CurrentOperation = Modifier;
		
			if (Modifier->PreProcessAnimation_Wrapper(Flow->State))
			{
				SequenceModifiers.Add(Modifier);
			}
		}
		
		if (SequenceModifiers.Num() > 0)
		{
			Flow->State.TotalOperationCount = SequenceModifiers.Num();
			bool bNeedsRefresh = false;
		
			for (int32 Idx = 0; Idx < SequenceModifiers.Num(); Idx++)
			{
				auto& Modifier = SequenceModifiers[Idx];
				Flow->State.CurrentOperationIndex = Idx;
				Flow->State.CurrentOperationName = Modifier->GetOperationName();
				Flow->State.CurrentOperation = Modifier;

				if (bNeedsRefresh)
				{
					Modifier->PreProcessAnimation_Wrapper(Flow->State);
				}
				
				Modifier->ProcessAnimation_Wrapper(Flow->State);
				if (Flow->State.bIsDirty)
				{
					bNeedsRefresh = true;
					RefreshStateForSequence(Flow->State, Flow->State.AnimationSequence);
				}
			}

			for (int32 Idx = 0; Idx < SequenceModifiers.Num(); Idx++)
			{
				auto& Modifier = SequenceModifiers[Idx];
				Flow->State.CurrentOperationIndex = Idx;
				Flow->State.CurrentOperationName = Modifier->GetOperationName();
				Flow->State.CurrentOperation = Modifier;
				
				Modifier->PostProcessAnimation_Wrapper(Flow->State);
			}
			
			if (!Flow->State.AnimationSequence->MarkPackageDirty())
			{
				const FText Message = FText::Format(LOCTEXT("AnimInfo.NotDirty", "Unable to mark destination animation {0} dirty!"), FText::FromString(Item.PackageName.ToString()));
				Flow->AddToModifierLog(nullptr, EAmmitLogVerbosity::Warning, Message.ToString());
			}
		}

		if (!Flow->bForceRootMotion)
		{
		}
		
		Flow->State.CurrentAnimationBatchIndex++;
		return true;
	}

	if (Flow->State.bIsRunning)
	{
		FlowLog(TEXT("==================="));
		FText Message = LOCTEXT("LogInfo.FinalReport", "Generating final reports...");
		FlowLog(Message);
		FlowLog(TEXT(""));

		
		Flow->State.CurrentAnimationBatchIndex = Flow->State.TotalAnimationBatchCount;
		Flow->State.TotalOperationCount = ActiveModifiers.Num();

		for (int32 Idx = 0; Idx < ActiveModifiers.Num(); Idx++)
		{
			auto& Modifier = ActiveModifiers[Idx];
			Flow->State.CurrentOperationIndex = Idx;
			Flow->State.CurrentOperationName = Modifier->GetOperationName();
			Flow->State.CurrentOperation = Modifier;

			Modifier->FinalReport_Wrapper(Flow->State);
		}
		
		Flow->State.CurrentAnimationBatchIndex = 0;
		Flow->State.CurrentOperationIndex = 0;
		Flow->State.AnimationSequence = nullptr;
		Flow->State.CurrentOperationName = TEXT("");
		Flow->State.CurrentOperation = nullptr;
		Flow->State.bIsRunning = false;

		FTSTicker::GetCoreTicker().RemoveTicker(InternalTickDelegateHandle);
		InternalTickDelegate.Unbind();

		FlowLog(TEXT("==================="));
		Message = LOCTEXT("LogInfo.Done", "Done!");
		FlowLog(Message);
		FlowLog(TEXT(""));

		FString EnumName = TEXT("EAmmitSequenceType::");
		for (const auto& Pair : SequenceTypeTally)
		{
			FString TypeName = UEnum::GetValueAsString(Pair.Key).RightChop(EnumName.Len());
			TypeName = TypeName.LeftPad(15);
			FlowLog(FString::Printf(TEXT("%s: %d"), *TypeName, Pair.Value));
		}
		
		return false;
	}

	return false;
}

bool FAmmitFlowEditorToolkit::GetAnimationAssets()
{
	TArray<FAssetData> WorkingSet;

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	const auto AnimClass = UAnimSequence::StaticClass();

	AnimationBatch.Empty();

	// Need to do this if running in the editor with -game to make sure that the assets in the following path are available
	TArray<FString> PathsToScan;
	PathsToScan.Add(Flow->SourcePath);
	AssetRegistry.ScanPathsSynchronous(PathsToScan);

	AssetRegistry.GetAssetsByPath(FName(Flow->SourcePath), WorkingSet, true);

	for (auto& Asset : WorkingSet)
	{
		if (Asset.GetClass()->IsChildOf(AnimClass))
			AnimationBatch.Add(Asset);
	}

	return AnimationBatch.Num() > 0;	
}

bool FAmmitFlowEditorToolkit::AssetDataForPath(const FString& DestinationPath, FAssetData& OutAssetData) const
{
	IAssetRegistry& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry")).Get();

	FString ParentPath = DestinationPath;
	int32 Index;
	if (ParentPath.FindLastChar(L'/', Index))
	{
		ParentPath = ParentPath.Left(Index);
	}

	TArray<FString> Paths;
	Paths.Add(ParentPath);
	AssetRegistryModule.ScanPathsSynchronous(Paths);
	OutAssetData = AssetRegistryModule.GetAssetByObjectPath(FSoftObjectPath(DestinationPath));

	return OutAssetData.IsValid();		
}

FString FAmmitFlowEditorToolkit::RelativePathForAsset(const FAssetData& Item) const
{
	FString RelativePath = Item.AssetName.ToString();

	RelativePath = Item.GetObjectPathString();
	if (RelativePath.StartsWith(Flow->SourcePath))
	{
		RelativePath = RelativePath.Right(RelativePath.Len() - (Flow->SourcePath.Len() + 1));
	}

	int32 Index;
	if (RelativePath.FindLastChar(L'.', Index))
	{
		RelativePath = RelativePath.Left(Index);
	}

	return RelativePath;	
}

void FAmmitFlowEditorToolkit::FlowLog(const FString& String, EAmmitLogVerbosity Level) const
{
	if (!Flow) return;

	Flow->AddToModifierLog(nullptr, Level, String);
}

void FAmmitFlowEditorToolkit::FlowLog(const FText& Text, EAmmitLogVerbosity Level) const
{
	FlowLog(Text.ToString(), Level);
}

TObjectPtr<UAnimSequence> FAmmitFlowEditorToolkit::LoadSequenceFromAsset(const FAssetData& Data) const
{
	// Hop to the game thread briefly to load our asset.
	UAnimSequence *Animation = Cast<UAnimSequence>(Data.GetAsset());

	// We are getting the skeleton (and dropping the result) to ensure it is loaded.
	// To keep ReSharper or Rider from whining, let's add a note here.

	// ReSharper disable once CppExpressionWithoutSideEffects
	Animation->GetSkeleton();

	return Animation;
	
}

TObjectPtr<UAnimSequence> FAmmitFlowEditorToolkit::DuplicateSequenceToPath(const TObjectPtr<UAnimSequence> SourceSequence,
                                                                FString DestinationPath) const
{
	FString TargetName = TEXT("");
	int32 Index;
	if (DestinationPath.FindLastChar(L'/', Index))
	{
		TargetName = DestinationPath.Right(DestinationPath.Len() - (Index + 1));
		DestinationPath = DestinationPath.Left(Index);
		if (TargetName.FindLastChar(L'.', Index))
		{
			TargetName = TargetName.Left(Index);
		}

		DestinationPath.Append(TEXT("/") + TargetName);
	}

	UPackage* Package = CreatePackage(*DestinationPath);
	UAnimSequence* NewAnimation = Cast<UAnimSequence>(StaticDuplicateObject(SourceSequence.Get(), Package, FName(TargetName), RF_Public | RF_Standalone, SourceSequence->GetClass(), EDuplicateMode::Normal, EInternalObjectFlags::AllFlags));
	if (NewAnimation)
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		NewAnimation->MarkPackageDirty();
		
		IAssetRegistry& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry")).Get();
		AssetRegistryModule.AssetCreated(NewAnimation);
	}

	return NewAnimation;	
}

bool FAmmitFlowEditorToolkit::UpdateStateForSequence(FAmmitModifierState& State, UAnimSequence* Sequence)
{
	State.DetectedSequenceType = EAmmitSequenceType::Unknown;
	State.AnimationSequence = Sequence;

	FAmmitBonePoint FarthestPoint;
	FAmmitBonePoint HighestPoint;
	FAmmitBonePoint LowestPoint;
	float FarthestYaw = 0.f;

	if (!CollectTransformTracks(State, Sequence, FarthestPoint, HighestPoint, LowestPoint, FarthestYaw))
	{
		return false;
	}

	// Attempt to figure out what sort of animation we are.
	float Farthest = FarthestPoint.Transform.GetTranslation().Length();

	bool bIdle = Farthest < Flow->IdleDistanceThreshold && FarthestYaw < Flow->IdleTurnThreshold;
	bool bMove = Farthest > Flow->IdleDistanceThreshold;
	bool bTurn = FarthestYaw > Flow->IdleTurnThreshold;
	bool bJumpOrClimb = HighestPoint.Transform.GetTranslation().Z - LowestPoint.Transform.GetTranslation().Z > Flow->JumpOrClimbThreshold;

	const FAmmitBonePoint& FirstMove = State.RootPoints[1];
	const FAmmitBonePoint& LastMove = State.RootPoints.Last();
	
	if (bIdle)
	{
		State.DetectedSequenceType = EAmmitSequenceType::Idle;
	}
	else if (bTurn && !bMove)
	{
		State.DetectedSequenceType = EAmmitSequenceType::TurnInPlace;
	}
	else if (bJumpOrClimb)
	{
		State.DetectedSequenceType = EAmmitSequenceType::JumpOrClimb;
	}
	else
	{
		const FVector FirstDirection = FirstMove.DeltaTransform.GetTranslation().GetSafeNormal2D();
		const FVector LastDirection = LastMove.DeltaTransform.GetTranslation().GetSafeNormal2D();
		
		const float FirstSpeed = FirstMove.DeltaTransform.GetTranslation().Size2D() / FirstMove.DeltaTime;
		const float LastSpeed = LastMove.DeltaTransform.GetTranslation().Size2D() / LastMove.DeltaTime;
		const float Difference = LastSpeed - FirstSpeed;
		const float AngleDifference = FMath::Abs(UKismetMathLibrary::NormalizeAxis(UKismetMathLibrary::DegAcos(FirstDirection.Dot(LastDirection))));

		if (AngleDifference > Flow->PivotAngleThreshold)
		{
			State.DetectedSequenceType = EAmmitSequenceType::Pivot;
		}
		else if (FMath::Abs(Difference) > Flow->GaitShiftThreshold)
		{
			if (LastSpeed < Flow->StartStopThreshold)
			{
				State.DetectedSequenceType = EAmmitSequenceType::Stop;
			}
			else if (FirstSpeed < Flow->StartStopThreshold)
			{
				State.DetectedSequenceType = EAmmitSequenceType::Start;
			}
		}

		if (State.DetectedSequenceType == EAmmitSequenceType::Unknown)
		{
			if (Flow->bUseNamesAsContext)
			{
				// Make a best-guess based on the sequence name.
				FString SequenceName = Sequence->GetName().ToLower();

				if (SequenceName.Contains(TEXT("start")))
				{
					State.DetectedSequenceType = EAmmitSequenceType::Start;
				}
				else if (SequenceName.Contains(TEXT("stop")))
				{
					State.DetectedSequenceType = EAmmitSequenceType::Stop;
				}
				else if (SequenceName.Contains(TEXT("pivot")))
				{
					State.DetectedSequenceType = EAmmitSequenceType::Pivot;
				}
			}

			// Loop is our fallback if nothing else matches.
			if (State.DetectedSequenceType == EAmmitSequenceType::Unknown)
				State.DetectedSequenceType = EAmmitSequenceType::Loop;
		}
	}

	int32 Counter = SequenceTypeTally.Contains(State.DetectedSequenceType) ? SequenceTypeTally[State.DetectedSequenceType] : 0;
	SequenceTypeTally.Emplace(State.DetectedSequenceType, ++Counter);
	
	return true;	
}

bool FAmmitFlowEditorToolkit::CollectTransformTracks(FAmmitModifierState& State, UAnimSequence* Sequence,
	FAmmitBonePoint& FarthestPoint, FAmmitBonePoint& HighestPoint, FAmmitBonePoint& LowestPoint, float& FarthestYaw)
{
	State.RootPoints.Empty();
	
	const FReferenceSkeleton& RefSkeleton = IsValid(Sequence->GetPreviewMesh())
											? Sequence->GetPreviewMesh()->GetRefSkeleton()
											: Sequence->GetSkeleton()->GetReferenceSkeleton();
	const FName RootBoneName = RefSkeleton.GetBoneName(0);
	const IAnimationDataModel* Model = Sequence->GetDataModel();
	State.AnimationSequenceFrames = Model->GetNumberOfKeys();
	State.AnimationSequenceLength = Model->GetPlayLength();
	State.bIsDirty = false;
	
	if(State.AnimationSequenceFrames < 2)
	{
		// Not enough frames.
		return false;
	}

	const float DeltaFrame = (State.AnimationSequenceLength / State.AnimationSequenceFrames);

	FBoneReference RootBone;
	RootBone.BoneName = TEXT("root");
	
	FTransform RootTransform = FTransform::Identity;
	float LastTime = 0.f;
	
	FTransform LastTransform = FTransform::Identity;
	for (int Idx = 0; Idx < State.AnimationSequenceFrames; ++Idx)
	{
		const float FrameTime = Idx * DeltaFrame;

		RootTransform = Model->EvaluateBoneTrackTransform(RootBoneName, Idx, EAnimInterpolationType::Step);

		FAmmitBonePoint Point;
		Point.Key = Idx;
		Point.Time = FrameTime;
		Point.DeltaTime = FrameTime - LastTime;
		Point.Transform = RootTransform;
		Point.DeltaTransform = RootTransform.GetRelativeTransform(LastTransform);
		State.RootPoints.Add(Point);

		if (Point.Transform.GetTranslation().Length() > FarthestPoint.Transform.GetTranslation().Length())
		{
			FarthestPoint = Point;
		}

		if (const float Yaw = FMath::Abs(Point.Transform.GetRotation().Rotator().Yaw); Yaw > FarthestYaw)
		{
			FarthestYaw = Yaw;
		}

		if (Point.Transform.GetTranslation().Z > HighestPoint.Transform.GetTranslation().Z)
		{
			HighestPoint = Point;
		}

		if (Point.Transform.GetTranslation().Z < LowestPoint.Transform.GetTranslation().Z)
		{
			LowestPoint = Point;
		}
		
		LastTime = FrameTime;
		LastTransform = RootTransform;
	}

	return true;
}

void FAmmitFlowEditorToolkit::RefreshStateForSequence(FAmmitModifierState& State, UAnimSequence* Sequence)
{
	FAmmitBonePoint FarthestPoint;
	FAmmitBonePoint HighestPoint;
	FAmmitBonePoint LowestPoint;
	float FarthestYaw;

	CollectTransformTracks(State, Sequence, FarthestPoint, HighestPoint, LowestPoint, FarthestYaw);
}

