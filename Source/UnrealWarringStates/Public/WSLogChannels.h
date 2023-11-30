// careted by xiaobeiswift  email:cit117@me.com  

#pragma once

#include "Logging/LogMacros.h"

class UObject;

UNREALWARRINGSTATES_API DECLARE_LOG_CATEGORY_EXTERN(LogWS, Log, All);
UNREALWARRINGSTATES_API DECLARE_LOG_CATEGORY_EXTERN(LogWSExperience, Log, All);
UNREALWARRINGSTATES_API DECLARE_LOG_CATEGORY_EXTERN(LogWSAbilitySystem, Log, All);

UNREALWARRINGSTATES_API FString GetClientServerContextString(UObject* ContextObject = nullptr);

