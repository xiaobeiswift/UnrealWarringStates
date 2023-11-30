// Created by xiaobeiswift  email:cit117@me.com  

#pragma once

#include "LoadingProcessInterface.h"
#include "Components/GameStateComponent.h"
#include "WSExperienceManagerComponent.generated.h"

namespace UE::GameFeatures { struct FResult; }

class UWSExperienceDefinition;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWSExperienceLoaded, const UWSExperienceDefinition* /*Experience*/);

enum class EWSExperienceLoadState
{
	Unloaded, // 未加载
	Loading, // 加载中
	LoadingGameFeatures, // 加载游戏特性
	LoadingChaosTestingDelay, // 加载混乱测试延迟
	ExecutingActions, // 执行动作
	Loaded, // 加载完成
	Deactivating // 休眠
};
UCLASS()
class UWSExperienceManagerComponent : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()
public:
	UWSExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface
	
	//~ILoadingProcessInterface interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface

	// 尝试设置当前体验，可以是UI或游戏体验
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	// 确保在体验加载完成后调用该委托，先于其他委托调用
	// 但是，如果体验已经加载完成，则立即调用该委托
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnWSExperienceLoaded::FDelegate&& Delegate);
	// 确保在体验加载完成后调用该委托
	// 如果体验已经加载完成，则立即调用该委托
	void CallOrRegister_OnExperienceLoaded(FOnWSExperienceLoaded::FDelegate&& Delegate);

	// 确保在体验加载完成后调用该委托
	// 如果体验已经加载完成，则立即调用该委托
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnWSExperienceLoaded::FDelegate&& Delegate);

	// 如果体验已完全加载，则返回当前体验，否则引发断言
	// (即，如果过早调用它)
	const UWSExperienceDefinition* GetCurrentExperienceChecked() const;

	// 如果体验已完全加载，则返回true
	bool IsExperienceLoaded() const;

private:
	UFUNCTION()
	void OnRep_CurrentExperience();

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();

	UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
	TObjectPtr<const UWSExperienceDefinition> CurrentExperience;

	EWSExperienceLoadState LoadState = EWSExperienceLoadState::Unloaded;

	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;

	/**
	 * 在其他委托调用之前调用的体验加载完成的委托（例如，为常规游戏设置的子系统）
	 */
	FOnWSExperienceLoaded OnExperienceLoaded_HighPriority;

	/** 体验加载完成的委托 */
	FOnWSExperienceLoaded OnExperienceLoaded;

	/** 体验加载完成的委托 */
	FOnWSExperienceLoaded OnExperienceLoaded_LowPriority;
};
