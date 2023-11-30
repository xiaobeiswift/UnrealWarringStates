// Created by xiaobeiswift  email:cit117@me.com  

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ExperienceReady.generated.h"

class AGameStateBase;
class UWSExperienceDefinition;
class UWorld;
struct FFrame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExperienceReadyAsyncDelegate);  // 声明一个异步多播委托FExperienceReadyAsyncDelegate

/**
 * 异步等待游戏状态变为准备就绪和有效，并在状态就绪时调用OnReady事件。如果游戏状态已经有效，则立即调用OnReady。
 */
UCLASS()
class UNREALWARRINGSTATES_API UAsyncAction_ExperienceReady : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	// 等待体验被确定并加载
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_ExperienceReady* WaitForExperienceReady(UObject* WorldContextObject);

	virtual void Activate() override;

public:

	// 当体验被确定并准备就绪/加载时被调用的委托
	UPROPERTY(BlueprintAssignable)
	FExperienceReadyAsyncDelegate OnReady;

private:
	void Step1_HandleGameStateSet(AGameStateBase* GameState);  // 处理游戏状态设置的第1步
	void Step2_ListenToExperienceLoading(AGameStateBase* GameState);  // 监听体验加载的第2步
	void Step3_HandleExperienceLoaded(const UWSExperienceDefinition* CurrentExperience);  // 处理体验加载完成的第3步
	void Step4_BroadcastReady();  // 广播准备就绪的第4步

	TWeakObjectPtr<UWorld> WorldPtr;  // 指向UWorld的弱指针
};

