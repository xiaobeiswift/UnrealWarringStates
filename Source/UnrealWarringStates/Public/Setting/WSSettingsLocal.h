// Created by xiaobeiswift  email:cit117@me.com  

#pragma once

#include "GameFramework/GameUserSettings.h"
#include "WSSettingsLocal.generated.h"

/**
 * 
 */
UCLASS()
class UNREALWARRINGSTATES_API UWSSettingsLocal : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UWSSettingsLocal();

	static UWSSettingsLocal* Get();

	
	void OnExperienceLoaded();

private:
	void ReapplyThingsDueToPossibleDeviceProfileChange();
};
