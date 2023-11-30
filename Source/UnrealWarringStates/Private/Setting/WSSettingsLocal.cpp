// Created by xiaobeiswift  email:cit117@me.com  


#include "Setting/WSSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WSSettingsLocal)

UWSSettingsLocal::UWSSettingsLocal()
{
	
}

UWSSettingsLocal* UWSSettingsLocal::Get()
{
	return GEngine ? CastChecked<UWSSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

void UWSSettingsLocal::OnExperienceLoaded()
{
	ReapplyThingsDueToPossibleDeviceProfileChange();
}

void UWSSettingsLocal::ReapplyThingsDueToPossibleDeviceProfileChange()
{
	//TODO: Reapply things due to possible device profile change
}
