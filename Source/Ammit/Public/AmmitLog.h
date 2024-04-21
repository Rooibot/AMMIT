#pragma once
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAmmit, Log, Warning)

UENUM(BlueprintType)
enum class EAmmitLogVerbosity : uint8
{
	Log,
	Warning,
	Error
};