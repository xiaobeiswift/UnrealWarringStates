// Created by xiaobeiswift  email:cit117@me.com  


#include "GameModes/WSExperienceManager.h"
#include "Engine/Engine.h"
#include "Subsystems/SubsystemCollection.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(WSExperienceManager)

#if WITH_EDITOR

// 在编辑器中开始播放时调用
void UWSExperienceManager::OnPlayInEditorBegun()
{
    // 确保GameFeaturePluginRequestCountMap为空
    ensure(GameFeaturePluginRequestCountMap.IsEmpty());
    // 清空GameFeaturePluginRequestCountMap
    GameFeaturePluginRequestCountMap.Empty();
}

// 通知插件被激活
void UWSExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
    // 如果是编辑器模式
    if (GIsEditor)
    {
        // 获取UWSExperienceManager的实例
        UWSExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UWSExperienceManager>();
        check(ExperienceManagerSubsystem);

        // 跟踪激活这个插件的请求者数量。因为并发请求会被处理，所以允许有多个加载/激活请求。
        int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
        ++Count;
    }
}

// 请求关闭插件
bool UWSExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
    // 如果是编辑器模式
    if (GIsEditor)
    {
        // 获取UWSExperienceManager的实例
        UWSExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UWSExperienceManager>();
        check(ExperienceManagerSubsystem);

        // 只允许最后一个请求者执行关闭插件的操作
        int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
        --Count;

        // 如果请求者数量为0，则从GameFeaturePluginRequestCountMap中移除该插件，并返回true
        if (Count == 0)
        {
            ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
            return true;
        }

        // 如果请求者数量不为0，则返回false
        return false;
    }

    // 如果不是编辑器模式，则允许关闭插件，并返回true
    return true;
}


#endif