// Created by xiaobeiswift  email:cit117@me.com  

#pragma once

#include "Engine/DataAsset.h"
#include "WSGameData.generated.h"

class UGameplayEffect;
class UObject;

UCLASS(BlueprintType, Const, Meta = (DisplayName = "虚幻战国游戏数据", ShortTooltip = "全局游戏数据资产."))
class UWSGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UWSGameData();

	// 返回已加载的游戏数据。
	static const UWSGameData& Get();

	// 用于施加伤害的游戏效果。使用 SetByCaller 来设置伤害量。
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "伤害游戏效果 (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;

	// 用于施加治疗的游戏效果。使用 SetByCaller 来设置治疗量。
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "治疗游戏效果 (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> HealGameplayEffect_SetByCaller;

	// 用于添加和移除动态标签的游戏效果。
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects")
	TSoftClassPtr<UGameplayEffect> DynamicTagGameplayEffect;
	
};
