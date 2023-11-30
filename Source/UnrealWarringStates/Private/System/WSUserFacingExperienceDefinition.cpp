// Created by xiaobeiswift  email:cit117@me.com  


#include "System/WSUserFacingExperienceDefinition.h"
#include "CommonSessionSubsystem.h"
#include "Containers/UnrealString.h"
#include "UObject/NameTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WSUserFacingExperienceDefinition)

UCommonSession_HostSessionRequest* UWSUserFacingExperienceDefinition::CreateHostingRequest() const
{
	const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();
	const FString UserFacingExperienceName = GetPrimaryAssetId().PrimaryAssetName.ToString();
	UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	Result->OnlineMode = ECommonSessionOnlineMode::Online;
	Result->bUseLobbies = true;
	Result->MapID = MapID;
	Result->ModeNameForAdvertisement = UserFacingExperienceName;
	Result->ExtraArgs = ExtraArgs;
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);
	Result->MaxPlayerCount = MaxPlayerCount;

	// if (ULyraReplaySubsystem::DoesPlatformSupportReplays())
	// {
	// 	if (bRecordReplay)
	// 	{
	// 		Result->ExtraArgs.Add(TEXT("DemoRec"), FString());
	// 	}
	// }

	return Result;
}
