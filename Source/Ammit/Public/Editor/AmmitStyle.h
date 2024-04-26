// Copyright 2024 Rooibot Games, LLC

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class AMMIT_API FAmmitStyle
{
	
public:
	static void Initialize();

	static void Shutdown();

	static void ReloadTextures();

	static const ISlateStyle& Get();

	static FName GetStyleSetName();

	static FMargin GetWidgetMargin()
	{
		static FMargin WidgetMargin = FMargin(3.0, 2.0, 3.0, 2.0);
		return WidgetMargin;
	}

private:
	static TSharedRef< class FSlateStyleSet > Create();

	static TSharedPtr< class FSlateStyleSet > StyleInstance; 
 
};
