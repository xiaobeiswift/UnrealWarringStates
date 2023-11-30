// Created by xiaobeiswift  email:cit117@me.com  


#include "GameModes/WSExperienceDefinition.h"

UWSExperienceDefinition::UWSExperienceDefinition()
{
}

EDataValidationResult UWSExperienceDefinition::IsDataValid(FDataValidationContext& Context) const
{
	return Super::IsDataValid(Context);
}

void UWSExperienceDefinition::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();
}
