// Created by xiaobeiswift  email:cit117@me.com  

#pragma once

#include "GameFramework/WorldSettings.h"
#include "WSWorldSettings.generated.h"

class UWSExperienceDefinition;
/**
 * 
 */
UCLASS()
class UNREALWARRINGSTATES_API AWSWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	AWSWorldSettings(const FObjectInitializer& ObjectInitializer);

	// Returns the default experience to use when a server opens this map if it is not overridden by the user-facing experience
	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	// The default experience to use when a server opens this map if it is not overridden by the user-facing experience
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<UWSExperienceDefinition> DefaultGameplayExperience;
};
