#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/PrimaryAssetLabel.h"
#include "Engine/World.h"
#include "UObject/WeakObjectPtr.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Parse.h"

/*
Usage Examples:

// 1. Synchronous Load Example
FPrimaryAssetId TowerId(TEXT("DataAsset"), TEXT("Tower_Basic"));
UPrimaryDataAsset* TowerData = UPolyPalsDefenseAssetManager::Get().LoadPrimaryDataAsset(TowerId);
if (TowerData)
{
    // TowerData ????嚥≪뮇彛?
}

// 2. Asynchronous Load Example
FPrimaryAssetId EnemyId(TEXT("DataAsset"), TEXT("Enemy_Spider"));
UPolyPalsDefenseAssetManager::Get().LoadPrimaryDataAssetAsync(
    EnemyId,
    [](UPrimaryDataAsset* LoadedAsset)
    {
        if (LoadedAsset)
        {
            // LoadedAsset ????嚥≪뮇彛?
        }
        else
        {
            // 嚥≪뮆諭???쎈솭 筌ｌ꼶??
        }
    }
);

// 3. Cached Retrieval Example
UPrimaryDataAsset* CachedTower = UPolyPalsDefenseAssetManager::Get().GetCachedAsset(TowerId);
if (CachedTower)
{
    // CachedTower ????
}

// 4. Clear Cache Example
UPolyPalsDefenseAssetManager::Get().ClearCachedAsset(TowerId);

// 5. DefaultEngine.ini??PrimaryAssetTypesToScan ??됰뻻
+PrimaryAssetTypesToScan=(
    PrimaryAssetType="Enemis",                                              // ?癒?탵?怨쀫퓠????쇱젟??Primary Asset Type ??已?
    AssetBaseClass="/Script/PolyPalsDefense.EnemiesDataAssets",             // ????????놁벥 疫꿸퀡??UObject ?????野껋럥以?
    bHasBlueprintClasses=False,                                             // ?됰뗀竊?袁ⓥ뵛???癒?뻼 ???????釉????
    bIsEditorOnly=False,                                                    // ?癒?탵???袁⑹뒠 ???
    Directories=((Path="/Game/PrimaryDataAssets/Enemies")),                 // ??쇳떔???遺얠젂?怨뺚봺 野껋럥以?
    SpecificAssets=(),                                                      // 揶쏆뮆???癒??筌왖??(??곸몵筌???獄쏄퀣肉?
    Rules=(
        Priority=-1,                                                        // 嚥≪뮆逾??怨쀪퐨??뽰맄 (?????롮쨯 ?믪눘?)
        ChunkId=-1,                                                         // ???텕筌왖 筌?寃?ID
        bApplyRecursively=True,                                             // ??륁맄 ???묉틦?? ??? ?怨몄뒠 ???
        CookRule=AlwaysCook                                                 // ?묒쥚沅?????쇱젟 (AlwaysCook, NeverCook ??
    )
)

// DefaultEngine.ini?癒?퐣????쇳떔 ??쇱젟???겸뫖????嚥?PrimaryAssetLabels ?諭??? 癰귢쑬猷??곕떽? ?븍뜇釉?酉鍮??덈뼄.
*/

void UPolyPalsDefenseAssetManager::StartInitialLoading()
{
    {
        const FString Section = TEXT("/Script/Engine.AssetManagerSettings");
        TArray<FString> AllEntries;
        GConfig->GetArray(*Section, TEXT("PrimaryAssetTypesToScan"), AllEntries, GEngineIni);

        TMap<FString, FString> UniqueEntriesMap;
        for (const FString& Entry : AllEntries)
        {
            FString ParsedType;
            if (FParse::Value(*Entry, TEXT("PrimaryAssetType=\""), ParsedType, /*bStrip*/false))
            {
                if (int32 Pos; ParsedType.FindChar(TEXT('"'), Pos))
                {
                    ParsedType = ParsedType.Left(Pos);
                }
                UniqueEntriesMap.Add(ParsedType, Entry);
            }
            else
            {
                UniqueEntriesMap.Add(Entry, Entry);
            }
        }

        TArray<FString> DedupedEntries;
        DedupedEntries.Reserve(UniqueEntriesMap.Num());
        for (auto& Pair : UniqueEntriesMap)
        {
            DedupedEntries.Add(Pair.Value);
        }

        GConfig->SetArray(
            *Section,
            TEXT("PrimaryAssetTypesToScan"),
            DedupedEntries,
            GEngineIni
        );
    }

    Super::StartInitialLoading();

    UE_LOG(LogTemp, Log, TEXT("PolyPalsDefenseAssetManager Initialized (with duplicates filtered)"));
}

UPrimaryDataAsset* UPolyPalsDefenseAssetManager::LoadPrimaryDataAsset(const FPrimaryAssetId& AssetId)
{
    if (CachedAssets.Contains(AssetId))
    {
        UPrimaryDataAsset* Cached = CachedAssets[AssetId];

        if (::IsValid(Cached))
        {
            return Cached;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Cached asset invalid, reloading: %s"), *AssetId.ToString());

            CachedAssets.Remove(AssetId);
        }
    }

    FSoftObjectPath AssetPath = GetPrimaryDataAssetPath(AssetId);

    if (!AssetPath.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("??롢걵??Asset Path: %s"), *AssetId.ToString());
        return nullptr;
    }

    GetStreamableManager().RequestSyncLoad(AssetPath);

    UPrimaryDataAsset* LoadedAsset = Cast<UPrimaryDataAsset>(GetPrimaryAssetObject(AssetId));

    if (LoadedAsset)
    {
        LoadedAsset->AddToRoot();
        CachedAssets.Add(AssetId, LoadedAsset);
        UE_LOG(LogTemp, Log, TEXT("??녿┛ 嚥≪뮆諭?獄?筌?Ŋ???袁⑥┷: %s"), *AssetId.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("??녿┛ 嚥≪뮆諭???쎈솭: %s"), *AssetId.ToString());
    }

    return LoadedAsset;
}

void UPolyPalsDefenseAssetManager::LoadPrimaryDataAssetAsync(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback)
{
    TArray<FPrimaryAssetId> AssetsToLoad = { AssetId };

    FAssetManagerAcquireResourceDelegate OnComplete = FAssetManagerAcquireResourceDelegate::CreateLambda([this, AssetId, OnLoadedCallback](bool bWasSuccessful)
        {
            if (bWasSuccessful)
            {
                UPrimaryDataAsset* LoadedAsset = Cast<UPrimaryDataAsset>(GetPrimaryAssetObject(AssetId));

                if (LoadedAsset)
                {
                    LoadedAsset->AddToRoot();
                    CachedAssets.Add(AssetId, LoadedAsset);

                    UE_LOG(LogTemp, Log, TEXT("??쑬猷욄묾?嚥≪뮆諭?獄?筌?Ŋ???袁⑥┷: %s"), *AssetId.ToString());

                    OnLoadedCallback(LoadedAsset);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("??쑬猷욄묾?嚥≪뮆諭???쎈솭 - 揶쏆빘猿쒐몴?筌≪뼚??????곸벉: %s"), *AssetId.ToString());

                    OnLoadedCallback(nullptr);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("??쑬猷욄묾?嚥≪뮆諭???쎈솭 - Delegate ?紐꾪뀱 ??쎈솭: %s"), *AssetId.ToString());

                OnLoadedCallback(nullptr);
            }
        });

    AcquireResourcesForPrimaryAssetList(AssetsToLoad, OnComplete, EChunkPriority::Immediate);
}

UPrimaryDataAsset* UPolyPalsDefenseAssetManager::LoadDataAssetWithFallback(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback)
{
    if (UPrimaryDataAsset* CachedAsset = GetCachedAsset(AssetId))
    {
        OnLoadedCallback(CachedAsset);
        return CachedAsset;
    }

    LoadPrimaryDataAssetAsync(AssetId, [this, AssetId, OnLoadedCallback](UPrimaryDataAsset* AsyncAsset)
    {
        if (AsyncAsset)
        {
            OnLoadedCallback(AsyncAsset);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("??쑬猷욄묾?嚥≪뮆諭???쎈솭, ??녿┛ 嚥≪뮆諭???뺣즲: %s"), *AssetId.ToString());

            UPrimaryDataAsset* SyncAsset = LoadPrimaryDataAsset(AssetId);

            if (SyncAsset)
            {
                OnLoadedCallback(SyncAsset);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("??녿┛ 嚥≪뮆諭????쎈솭: %s"), *AssetId.ToString());

                OnLoadedCallback(nullptr);
            }
        }
    });

    return nullptr;
}

UPrimaryDataAsset* UPolyPalsDefenseAssetManager::GetCachedAsset(const FPrimaryAssetId& AssetId)
{
    if (UPrimaryDataAsset** CachedAssetPtr = CachedAssets.Find(AssetId))
    {
        if (*CachedAssetPtr != nullptr)
        {
            return *CachedAssetPtr;
        }
        else
        {
            CachedAssets.Remove(AssetId);

            UE_LOG(LogTemp, Warning, TEXT("?醫륁뒞??? ??? 筌?Ŋ????볤탢: %s"), *AssetId.ToString());
        }
    }

    return nullptr;
}

void UPolyPalsDefenseAssetManager::ClearCachedAsset(const FPrimaryAssetId& AssetId)
{
    if (CachedAssets.Remove(AssetId) > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("筌?Ŋ????癒?텦 ??볤탢 ?袁⑥┷: %s"), *AssetId.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("筌?Ŋ????癒?텦??鈺곕똻???? ??놁벉 ?癒?뮉 ??? ??볤탢?? %s"), *AssetId.ToString());
    }
}

UPolyPalsDefenseAssetManager& UPolyPalsDefenseAssetManager::Get()
{
    UPolyPalsDefenseAssetManager* Singleton = Cast<UPolyPalsDefenseAssetManager>(GEngine->AssetManager);

    if (!Singleton)
    {
        UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager class in DefaultEngine.ini. Must be UPolyPalsDefenseAssetManager!"));
    }

    return *Singleton;
}

FSoftObjectPath UPolyPalsDefenseAssetManager::GetPrimaryDataAssetPath(const FPrimaryAssetId& AssetId) const
{
    FAssetData AssetData;

    if (GetPrimaryAssetData(AssetId, AssetData))
    {
        return AssetData.ToSoftObjectPath();
    }

    UObject* AssetObject = GetPrimaryAssetObject(AssetId);

    if (AssetObject)
    {
        return FSoftObjectPath(AssetObject);
    }

    UE_LOG(LogTemp, Error, TEXT("GetPrimaryDataAssetPath ??쎈솭 - ??롢걵??PrimaryAssetId: %s"), *AssetId.ToString());

    return FSoftObjectPath();
}

FStreamableManager& UPolyPalsDefenseAssetManager::GetStreamableManager()
{
    return UAssetManager::GetStreamableManager();
}
