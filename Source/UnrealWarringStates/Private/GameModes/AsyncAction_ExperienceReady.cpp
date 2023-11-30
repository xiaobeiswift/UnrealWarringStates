// Created by xiaobeiswift  email:cit117@me.com  

#include "GameModes/AsyncAction_ExperienceReady.h" // 包含AsyncAction_ExperienceReady.h头文件

#include "GameModes/WSExperienceManagerComponent.h" // 包含WSExperienceManagerComponent.h头文件

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ExperienceReady) // 包含AsyncAction_ExperienceReady的内联C++代码

UAsyncAction_ExperienceReady::UAsyncAction_ExperienceReady(const FObjectInitializer& ObjectInitializer)
// AsyncAction_ExperienceReady的构造函数
	: Super(ObjectInitializer) // 调用基类的构造函数
{
}

UAsyncAction_ExperienceReady* UAsyncAction_ExperienceReady::WaitForExperienceReady(UObject* WorldContextObject)
// 生成等待经验准备的异步动作
{
	UAsyncAction_ExperienceReady* Action = nullptr; // 创建一个异步动作对象

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	// 从上下文对象获取世界对象
	{
		Action = NewObject<UAsyncAction_ExperienceReady>(); // 创建新的异步动作对象
		Action->WorldPtr = World; // 设置异步动作的世界对象
		Action->RegisterWithGameInstance(World); // 注册异步动作到游戏实例
	}

	return Action; // 返回异步动作对象
}

void UAsyncAction_ExperienceReady::Activate()
{
	if (UWorld* World = WorldPtr.Get())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			Step2_ListenToExperienceLoading(GameState);
		}
		else
		{
			World->GameStateSetEvent.AddUObject(this, &ThisClass::Step1_HandleGameStateSet);
		}
	}
	else
	{
		// No world so we'll never finish naturally
		SetReadyToDestroy();
	}
}

void UAsyncAction_ExperienceReady::Step1_HandleGameStateSet(AGameStateBase* GameState)
{
	if (UWorld* World = WorldPtr.Get())
	{
		World->GameStateSetEvent.RemoveAll(this);
	}

	Step2_ListenToExperienceLoading(GameState);
}

void UAsyncAction_ExperienceReady::Step2_ListenToExperienceLoading(AGameStateBase* GameState) // 监听经验加载过程
{
	check(GameState); // 断言游戏状态对象存在
	UWSExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<
		UWSExperienceManagerComponent>(); // 查找游戏状态对象上的WSExperienceManagerComponent组件
	check(ExperienceComponent); // 断言WSExperienceManagerComponent组件存在

	if (ExperienceComponent->IsExperienceLoaded()) // 如果体验已经加载完成
	{
		UWorld* World = GameState->GetWorld(); // 获取游戏状态对象所在的世界对象
		check(World); // 断言世界对象存在

		// 体验碰巧已经加载完成，但是延迟一个帧以确保人们不会编写依赖于这个始终为真的东西
		//@TODO: 考虑不延迟动态生成的对象或加载屏幕上移除后的任何时间？
		//@TODO: 可能只是在经验加载本身中注入一个随机0-1秒的延迟？
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &ThisClass::Step4_BroadcastReady)); // 设置在下一帧触发的定时器，以传播经验准备消息
	}
	else
	{
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(
			FOnWSExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleExperienceLoaded));
		// 调用或注册经验加载完成回调函数
	}
}

void UAsyncAction_ExperienceReady::Step3_HandleExperienceLoaded(const UWSExperienceDefinition* CurrentExperience)
// 处理经验加载完成事件
{
	Step4_BroadcastReady(); // 传播经验准备消息
}

void UAsyncAction_ExperienceReady::Step4_BroadcastReady()
{
	OnReady.Broadcast();
	SetReadyToDestroy(); // 设置销毁
}
