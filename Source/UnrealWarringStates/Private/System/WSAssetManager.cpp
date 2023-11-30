// Created by xiaobeiswift  email:cit117@me.com  


#include "System/WSAssetManager.h"
#include "System/WSGameData.h"
#include "Character//WSPawnData.h"
#include "WSLogChannels.h"
#include "Misc/ScopedSlowTask.h"
#include "System/FWSAssetManagerStartupJob.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WSAssetManager)

// const FName FWSBundles::Equipped("Equipped");
const FName FWSBundles::Equipped("Equipped");

// ////////////////////////////////////////////////////////////////////////

// 程序集命令：UnrealWarringStates.DumpLoadedAssets
// 用途：显示通过资源管理器加载并在内存中的所有资源
static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("UnrealWarringStates.DumpLoadedAssets"),
	TEXT("显示通过资源管理器加载并在内存中的所有资源."),
	FConsoleCommandDelegate::CreateStatic(UWSAssetManager::DumpLoadedAssets)
);

//////////////////////////////////////////////////////////////////////

#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FWSAssetManagerStartupJob(#JobFunc, [this](const FWSAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)

//////////////////////////////////////////////////////////////////////

UWSAssetManager::UWSAssetManager()
{
	DefaultPawnData = nullptr;
}

UWSAssetManager& UWSAssetManager::Get()
{
	check(GEngine);

	if (UWSAssetManager* Singleton = Cast<UWSAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogWS, Fatal, TEXT("无效的资源管理器类名在DefaultEngine.ini文件中.  它必须设置成WSAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<UWSAssetManager>();
}

void UWSAssetManager::DumpLoadedAssets()
{
	UE_LOG(LogWS, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(LogWS, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(LogWS, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(LogWS, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
	
}

const UWSGameData& UWSAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<UWSGameData>(WSGameDataPath);
}

const UWSPawnData* UWSAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

UObject* UWSAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		if (ShouldLogAssetLoads())
		{
			TUniquePtr<FScopeLogTime> LogTimePtr = MakeUnique<FScopeLogTime>(
				*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr,
				FScopeLogTime::ScopeLog_Seconds);
		}

		if (IsInitialized())
		{
			return GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool UWSAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void UWSAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void UWSAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UWSAssetManager::StartInitialLoading");

	// This does all of the scanning, need to do this now even if loads are deferred
	Super::StartInitialLoading();

	STARTUP_JOB(InitializeGameplayCueManager());

	{
		// Load base game data asset
		STARTUP_JOB_WEIGHTED(GetGameData(), 25.f);
	}

	// Run all the queued up startup jobs
	DoAllStartupJobs();
	
}
#if WITH_EDITOR
void UWSAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);
	{
		FScopedSlowTask SlowTask(0, NSLOCTEXT("WSEditor", "BeginLoadingPIEData", "Loading PIE Data"));
		constexpr bool bShowCancelButton = false;
		constexpr bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);

		const UWSGameData& LocalGameDataCommon = GetGameData();

		// Intentionally after GetGameData to avoid counting GameData time in this timer
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);

		// You could add preloading of anything else needed for the experience we'll be using here
		// (e.g., by grabbing the default experience from the world settings + the experience override in developer settings)
	}
}
#endif

UPrimaryDataAsset* UWSAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass,
	const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("LyraEditor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		constexpr bool bShowCancelButton = false;
		constexpr bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		UE_LOG(LogWS, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(LogWS, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}

void UWSAssetManager::DoAllStartupJobs()
{
    // 执行所有启动任务的函数

    SCOPED_BOOT_TIMING("UWSAssetManager::DoAllStartupJobs");
    const double AllStartupJobsStartTime = FPlatformTime::Seconds();

    if (IsRunningDedicatedServer())
    {
        // 如果正在运行专用服务器，则无需定期进度更新，只执行任务
        for (const FWSAssetManagerStartupJob& StartupJob : StartupJobs)
        {
            StartupJob.DoJob();
        }
    }
    else
    {
        if (StartupJobs.Num() > 0)
        {
            float TotalJobValue = 0.0f;
            for (const FWSAssetManagerStartupJob& StartupJob : StartupJobs)
            {
                TotalJobValue += StartupJob.JobWeight;
            }

            float AccumulatedJobValue = 0.0f;
            for (FWSAssetManagerStartupJob& StartupJob : StartupJobs)
            {
                const float JobValue = StartupJob.JobWeight;
                StartupJob.SubstepProgressDelegate.BindLambda([This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
                {
                    const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue;
                    const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue;

                    // 更新初始游戏内容加载百分比
                    This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
                });

                StartupJob.DoJob();

                StartupJob.SubstepProgressDelegate.Unbind();

                AccumulatedJobValue += JobValue;

                // 更新初始游戏内容加载百分比
                UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
            }
        }
        else
        {
            // 如果没有启动任务，则将初始游戏内容加载百分比设置为1.0
            UpdateInitialGameContentLoadPercent(1.0f);
        }
    }

    // 清空启动任务列表
    StartupJobs.Empty();

    UE_LOG(LogWS, Display, TEXT("所有启动任务完成共耗时 %.2f 秒"), FPlatformTime::Seconds() - AllStartupJobsStartTime);
}


void UWSAssetManager::InitializeGameplayCueManager()
{
	SCOPED_BOOT_TIMING("UWSAssetManager::InitializeGameplayCueManager");

	// UWSGameplayCueManager* Gcm = UWSGameplayCueManager::Get();
	// check(Gcm);
	// Gcm->LoadAlwaysLoadedCues();
}

void UWSAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{
	// Could route this to the early startup loading screen
}


