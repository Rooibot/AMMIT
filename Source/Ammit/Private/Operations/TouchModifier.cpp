// Copyright 2024 Rooibot Games, LLC


#include "Operations/TouchModifier.h"

bool UTouchModifier::InitializeOperation_Implementation(const FAmmitModifierState& Context)
{
	return true;
}

bool UTouchModifier::PreProcessAnimation_Implementation(const FAmmitModifierState& Context)
{
	return true;
}

void UTouchModifier::ProcessAnimation_Implementation(const FAmmitModifierState& Context)
{
	// All we do is mark the package dirty -- effectively, we are a UNIX 'touch' command.
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	Context.AnimationSequence->MarkPackageDirty();
}

void UTouchModifier::PostProcessAnimation_Implementation(const FAmmitModifierState& Context)
{
}

void UTouchModifier::FinalReport_Implementation(const FAmmitModifierState& Context)
{
}
