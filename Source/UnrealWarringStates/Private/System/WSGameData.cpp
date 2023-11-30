// Created by xiaobeiswift  email:cit117@me.com  


#include "System/WSGameData.h"
#include "System/WSAssetManager.h"
#include "Kismet/KismetArrayLibrary.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(WSGameData)
UWSGameData::UWSGameData()
{
	
}

const UWSGameData& UWSGameData::Get()
{
	return UWSAssetManager::Get().GetGameData();
}
