// Copyright 2024 Rooibot Games, LLC


#include "AmmitStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/StyleColors.h"

// Needed for some Slate macros.
#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FAmmitStyle::StyleInstance = nullptr;

void FAmmitStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}	
}

void FAmmitStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();	
}

void FAmmitStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}	
}

const ISlateStyle& FAmmitStyle::Get()
{
	return *StyleInstance;	
}

FName FAmmitStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("AmmitSlateStyle"));
	return StyleSetName;	
}

TSharedRef<FSlateStyleSet> FAmmitStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("AMMIT")->GetBaseDir() / TEXT("Resources"));

	// Rawr
	Style->Set("Icons.Ammit", new IMAGE_BRUSH_SVG(TEXT("AMMIT"), CoreStyleConstants::Icon128x128));
	Style->Set("Icons.AmmitSmall", new IMAGE_BRUSH_SVG(TEXT("AMMIT"), FVector2D(20.f, 20.f)));

	auto NormalFont = FAppStyle::GetFontStyle("PropertyWindow.NormalFont");
	auto BoldFont = FAppStyle::GetFontStyle("PropertyWindow.BoldFont");

	const auto NormalStyle = FTextBlockStyle()
		.SetFont(NormalFont)
		.SetColorAndOpacity(FSlateColor::UseForeground())
		.SetShadowOffset(FVector2D::ZeroVector)
		.SetShadowColorAndOpacity(FLinearColor::Black);
	Style->Set("Text.Normal", NormalStyle);

	const auto WarningStyle = FTextBlockStyle()
		.SetFont(NormalFont)
		.SetColorAndOpacity(FSlateColor::UseForeground())
		.SetShadowOffset(FVector2D::ZeroVector)
		.SetShadowColorAndOpacity(FLinearColor::Yellow);
	Style->Set("Text.Warning", NormalStyle);

	const auto ErrorStyle = FTextBlockStyle()
		.SetFont(NormalFont)
		.SetColorAndOpacity(FSlateColor::UseForeground())
		.SetShadowOffset(FVector2D::ZeroVector)
		.SetShadowColorAndOpacity(FLinearColor::Red);
	Style->Set("Text.Error", NormalStyle);
	
	
	const auto BoldStyle = FTextBlockStyle()
		.SetFont(BoldFont)
		.SetColorAndOpacity(FSlateColor::UseForeground())
		.SetShadowOffset(FVector2D::ZeroVector)
		.SetShadowColorAndOpacity(FLinearColor::Black);
	Style->Set("Text.Bold", BoldStyle);

	auto HeaderFont = FSlateFontInfo(NormalFont);
	HeaderFont.Size = NormalFont.Size * 2;
	const auto HeaderStyle = FTextBlockStyle()
		.SetFont(HeaderFont)
		.SetColorAndOpacity(FSlateColor::UseForeground())
		.SetShadowOffset(FVector2D(1.0, 1.0))
		.SetShadowColorAndOpacity(FLinearColor::Black);
	Style->Set("Text.Header", HeaderStyle);

	auto MonoFont = DEFAULT_FONT("Mono", 9);
	const auto MonoStyle = FTextBlockStyle()
		.SetFont(MonoFont)
		.SetColorAndOpacity(FSlateColor::UseForeground())
		.SetShadowOffset(FVector2D::ZeroVector)
		.SetShadowColorAndOpacity(FLinearColor::Black);
	Style->Set("Text.Mono", MonoStyle);
	
	auto RoundedBorder = new FSlateRoundedBoxBrush(FStyleColors::Input, 4.0f, FStyleColors::InputOutline, 1.0f);
	Style->Set("Box.RoundedBorder", RoundedBorder);
	
	return Style;	
}
