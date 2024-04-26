// Copyright 2024 Rooibot Games, LLC


#include "Operations/CompressionModifier.h"
#include "AnimationBlueprintLibrary.h"
#include "PersonaModule.h"

#define LOCTEXT_NAMESPACE "AMMIT"

bool UCompressionModifier::InitializeOperation_Implementation(const FAmmitModifierState& Context)
{
	SequencesToCompress.Empty();
	return true;
}

bool UCompressionModifier::PreProcessAnimation_Implementation(const FAmmitModifierState& Context)
{
	return true;
}

void UCompressionModifier::ProcessAnimation_Implementation(const FAmmitModifierState& Context)
{
	UAnimationBlueprintLibrary::SetCurveCompressionSettings(Context.AnimationSequence, CompressionSettings);
	SequencesToCompress.Add(Context.AnimationSequence);
}

void UCompressionModifier::PostProcessAnimation_Implementation(const FAmmitModifierState& Context)
{
	Super::PostProcessAnimation_Implementation(Context);
}

void UCompressionModifier::FinalReport_Implementation(const FAmmitModifierState& Context)
{
	Super::FinalReport_Implementation(Context);

	AddToLog(Context, EAmmitLogVerbosity::Log, LOCTEXT("CompressionModifier.CompressingCurves", "Compressing curve data on all animations..."));
	
	FPersonaModule& PersonaModule = FModuleManager::GetModuleChecked<FPersonaModule>("Persona");
	PersonaModule.ApplyCompression(SequencesToCompress, false);	
}
