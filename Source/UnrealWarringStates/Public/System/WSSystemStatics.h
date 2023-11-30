// Created by xiaobeiswift  email:cit117@me.com  

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/SoftObjectPtr.h"
#include "WSSystemStatics.generated.h"

template <typename T> class TSubclassOf;

class AActor;
class UActorComponent;
class UObject;
struct FFrame;

UCLASS()
class  UWSSystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 从主要资产ID获取指定类型软对象引用
	static TSoftObjectPtr<> GetTypedSoftObjectReferenceFromPrimaryAssetId(FPrimaryAssetId PrimaryAssetId, TSubclassOf<UObject> ExpectedAssetType);

	// 从用户面对的体验ID获取主要资产ID
	static FPrimaryAssetId GetPrimaryAssetIdFromUserFacingExperienceName(const FString& AdvertisedExperienceID);

	// 播放下一个游戏
	static void PlayNextGame(const UObject* WorldContextObject);

	// 在TargetActor的所有网格组件的所有部分上设置标量参数值
	static void SetScalarParameterValueOnAllMeshComponents(AActor* TargetActor, const FName ParameterName,const float ParameterValue, bool bIncludeChildActors = true);

	// 在TargetActor的所有网格组件的所有部分上设置向量参数值
	static void SetVectorParameterValueOnAllMeshComponents(AActor* TargetActor, const FName ParameterName,const FVector ParameterValue,bool bIncludeChildActors = true);

	// 在TargetActor的所有网格组件的所有部分上设置颜色参数值
	static void SetColorParameterValueOnAllMeshComponents(AActor* TargetActor, const FName ParameterName,const FLinearColor ParameterValue,bool bIncludeChildActors = true);

	// 根据给定的类获取所有该类的组件
	static TArray<UActorComponent*> FindComponentsByClass(AActor* TargetActor,TSubclassOf<UActorComponent> ComponentClass,bool bIncludeChildActors = true);
};
