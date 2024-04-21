// Copyright 2024 Rooibot Games, LLC


#include "Flow/AmmitBaseFlow.h"

#include "GPUSkinVertexFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Operations/AmmitBaseModifier.h"

#define LOCTEXT_NAMESPACE "AMMIT"


bool UAmmitBaseFlow::CanExecuteFlow() const
{
	return !State.bIsRunning && ModifierOperations.Num() > 0 && !SourcePath.IsEmpty();
}

void UAmmitBaseFlow::AddToModifierLog(UAmmitBaseModifier* Modifier, EAmmitLogVerbosity Level, FString LogText)
{
	FString OperationName {};
	FString LogPrefix {};
	if (Modifier != nullptr)
	{
		const FString ModifierInstanceName = Modifier->GetName();
		OperationName = Modifier->GetOperationName();

		if (ModifierInstanceName == OperationName)
		{
			LogPrefix = FString::Printf(TEXT("[%ls : %ls] "), *GetName(), *OperationName);
		}
		else
		{
			LogPrefix = FString::Printf(TEXT("[%ls : %ls (%ls)] "), *GetName(), *ModifierInstanceName, *OperationName);
		}
	}
	else
	{
		LogPrefix = FString::Printf(TEXT("[%ls] "), *GetName());
	}
	
	FString GuiLine {};
	if (Modifier != nullptr)
	{
		GuiLine = FString::Printf(TEXT("[%ls] "), *OperationName);
	}

	// Warnings or errors go into the actual Unreal log, with some identifying information as to what Flow
	// and Operation the log text came from.
	if (Level == EAmmitLogVerbosity::Warning)
	{
		UE_LOG(LogAmmit, Warning, TEXT("%s%s"), *LogPrefix, *LogText)
	}
	else if (Level == EAmmitLogVerbosity::Error)
	{
		UE_LOG(LogAmmit, Error, TEXT("%s%s"), *LogPrefix, *LogText)
	}

	const FString NewLog = CurrentLog.ToString() + GuiLine + LogText + "\n";
	CurrentLog = FText::FromString(NewLog);
}