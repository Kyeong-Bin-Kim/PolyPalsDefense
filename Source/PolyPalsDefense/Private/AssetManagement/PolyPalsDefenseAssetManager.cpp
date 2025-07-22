#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/PrimaryAssetLabel.h"
#include "Engine/World.h"
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

// ?袁⑥쨮??븍뱜 ??뽰삂 ???λ뜃由?嚥≪뮆逾??⑥눘????????롫뮉 ??λ땾
void UPolyPalsDefenseAssetManager::StartInitialLoading()
{
    // 1. GConfig ?癒?퐣 餓λ쵎??PrimaryAssetTypesToScan ?????? TowerMaterials) ??볤탢
    {
        const FString Section = TEXT("/Script/Engine.AssetManagerSettings");
        TArray<FString> AllEntries;
        GConfig->GetArray(*Section, TEXT("PrimaryAssetTypesToScan"), AllEntries, GEngineIni);

        // TypeName ??곗쨮 餓λ쵎??筌ｋ똾寃??Map
        TMap<FString, FString> UniqueEntriesMap;
        for (const FString& Entry : AllEntries)
        {
            FString ParsedType;
            if (FParse::Value(*Entry, TEXT("PrimaryAssetType=\""), ParsedType, /*bStrip*/false))
            {
                // ??ル뮉 ?怨쀬긾???袁㏉돱筌왖 ??롮뵬??용┛
                if (int32 Pos; ParsedType.FindChar(TEXT('"'), Pos))
                {
                    ParsedType = ParsedType.Left(Pos);
                }
                UniqueEntriesMap.Add(ParsedType, Entry);
            }
            else
            {
                // TypeName ???뼓????쎈솭??롢늺 ??곕뼊 域밸챶?嚥????
                UniqueEntriesMap.Add(Entry, Entry);
            }
        }

        // TMap ??揶?Value)筌?筌믩쵐釉??TArray 嚥?
        TArray<FString> DedupedEntries;
        DedupedEntries.Reserve(UniqueEntriesMap.Num());
        for (auto& Pair : UniqueEntriesMap)
        {
            DedupedEntries.Add(Pair.Value);
        }

        // GConfig ????堉?怨뚮┛ (?醫딅빍??釉?野껉퍓彛?
        GConfig->SetArray(
            *Section,
            TEXT("PrimaryAssetTypesToScan"),
            DedupedEntries,
            GEngineIni
        );
    }

    // 2. ?類ㅼ젫????쇱젟??곗쨮 ?봔筌???????λ뜃由????쎈뻬 (??由??ScanPrimaryAssetTypesFromConfig 繹먮슣? ?癒?짗 ?紐꾪뀱)
    Super::StartInitialLoading();

    UE_LOG(LogTemp, Log, TEXT("PolyPalsDefenseAssetManager Initialized (with duplicates filtered)"));
}

// ??녿┛ 獄쎻뫗???곗쨮 PrimaryDataAsset??嚥≪뮆諭??띻탢??筌?Ŋ??癒?퐣 獄쏆꼹??
UPrimaryDataAsset* UPolyPalsDefenseAssetManager::LoadPrimaryDataAsset(const FPrimaryAssetId& AssetId)
{
    // ??? 筌?Ŋ???鈺곕똻???롢늺 域밸챶?嚥?獄쏆꼹??
    if (CachedAssets.Contains(AssetId))
    {
        return CachedAssets[AssetId];
    }

    // AssetId?????臾볥릭??野껋럥以덄몴???얜굣
    FSoftObjectPath AssetPath = GetPrimaryDataAssetPath(AssetId);
    if (!AssetPath.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("??롢걵??Asset Path: %s"), *AssetId.ToString());
        return nullptr;
    }

    // ??쎈뱜?귐됥돢 筌띲끇????????????녿┛ 嚥≪뮆諭??遺욧퍕
    GetStreamableManager().RequestSyncLoad(AssetPath);

    // 嚥≪뮆諭??袁⑥┷??揶쏆빘猿쒐몴?揶쎛?紐? 筌?Ŋ???
    UPrimaryDataAsset* LoadedAsset = Cast<UPrimaryDataAsset>(GetPrimaryAssetObject(AssetId));

    // 嚥≪뮆諭??源껊궗 ??筌?Ŋ????곕떽???랁?嚥≪뮄???곗뮆??
    if (LoadedAsset)
    {
        CachedAssets.Add(AssetId, LoadedAsset);
        UE_LOG(LogTemp, Log, TEXT("??녿┛ 嚥≪뮆諭?獄?筌?Ŋ???袁⑥┷: %s"), *AssetId.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("??녿┛ 嚥≪뮆諭???쎈솭: %s"), *AssetId.ToString());
    }

    return LoadedAsset;
}

// ??쑬猷욄묾?獄쎻뫗???곗쨮 PrimaryDataAsset??嚥≪뮆諭??랁??袁⑥┷ ???꾩뮆媛??紐꾪뀱
void UPolyPalsDefenseAssetManager::LoadPrimaryDataAssetAsync(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback)
{
    // 嚥≪뮆諭??AssetId ?귐딅뮞????밴쉐
    TArray<FPrimaryAssetId> AssetsToLoad = { AssetId };

    // 嚥≪뮆諭??袁⑥┷ ???紐꾪뀱??Delegate ??밴쉐
    FAssetManagerAcquireResourceDelegate OnComplete = FAssetManagerAcquireResourceDelegate::CreateLambda([this, AssetId, OnLoadedCallback](bool bWasSuccessful)
        {
            if (bWasSuccessful)
            {
                // ??쑬猷욄묾?嚥≪뮆諭??源껊궗 ??揶쏆빘猿???얜굣
                UPrimaryDataAsset* LoadedAsset = Cast<UPrimaryDataAsset>(GetPrimaryAssetObject(AssetId));
                if (LoadedAsset)
                {
                    // 筌?Ŋ????곕떽? 獄??源껊궗 嚥≪뮄?????꾩뮆媛???쎈뻬
                    CachedAssets.Add(AssetId, LoadedAsset);
                    UE_LOG(LogTemp, Log, TEXT("??쑬猷욄묾?嚥≪뮆諭?獄?筌?Ŋ???袁⑥┷: %s"), *AssetId.ToString());
                    OnLoadedCallback(LoadedAsset);
                }
                else
                {
                    // 揶쏆빘猿?鈺곌퀬????쎈솭 ???癒?쑎 嚥≪뮄??獄?null ?꾩뮆媛?
                    UE_LOG(LogTemp, Error, TEXT("??쑬猷욄묾?嚥≪뮆諭???쎈솭 - 揶쏆빘猿쒐몴?筌≪뼚??????곸벉: %s"), *AssetId.ToString());
                    OnLoadedCallback(nullptr);
                }
            }
            else
            {
                // ??쑬猷욄묾?嚥≪뮆諭??癒?퍥 ??쎈솭 ???癒?쑎 嚥≪뮄??獄?null ?꾩뮆媛?
                UE_LOG(LogTemp, Error, TEXT("??쑬猷욄묾?嚥≪뮆諭???쎈솭 - Delegate ?紐꾪뀱 ??쎈솭: %s"), *AssetId.ToString());
                OnLoadedCallback(nullptr);
            }
        });

    // 筌왖?類λ립 ?怨쀪퐨??뽰맄嚥???쑬猷욄묾?嚥≪뮆諭??遺욧퍕
    AcquireResourcesForPrimaryAssetList(AssetsToLoad, OnComplete, EChunkPriority::Immediate);
}

// ??쑬猷욄묾?嚥≪뮆諭???쎈솭 ????녿┛ 嚥≪뮆諭뜻에???筌ｋ똾釉????노? 嚥≪뮆諭???λ땾
UPrimaryDataAsset* UPolyPalsDefenseAssetManager::LoadDataAssetWithFallback(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback)
{
    // ??? 筌?Ŋ????癒?텦????됱몵筌?筌앸맩???꾩뮆媛???쎈뻬
    if (UPrimaryDataAsset* CachedAsset = GetCachedAsset(AssetId))
    {
        OnLoadedCallback(CachedAsset);
        return CachedAsset;
    }

    // ??쑬猷욄묾?嚥≪뮆諭???뺣즲
    LoadPrimaryDataAssetAsync(AssetId, [this, AssetId, OnLoadedCallback](UPrimaryDataAsset* AsyncAsset)
        {
            if (AsyncAsset)
            {
                // ??쑬猷욄묾??源껊궗 ???꾩뮆媛?
                OnLoadedCallback(AsyncAsset);
            }
            else
            {
                // ??쑬猷욄묾???쎈솭 ????녿┛ 嚥≪뮆諭??????
                UE_LOG(LogTemp, Warning, TEXT("??쑬猷욄묾?嚥≪뮆諭???쎈솭, ??녿┛ 嚥≪뮆諭???뺣즲: %s"), *AssetId.ToString());
                UPrimaryDataAsset* SyncAsset = LoadPrimaryDataAsset(AssetId);
                if (SyncAsset)
                {
                    OnLoadedCallback(SyncAsset);
                }
                else
                {
                    // ??녿┛ 嚥≪뮆諭????쎈솭 ???癒?쑎 ?꾩뮆媛?
                    UE_LOG(LogTemp, Error, TEXT("??녿┛ 嚥≪뮆諭????쎈솭: %s"), *AssetId.ToString());
                    OnLoadedCallback(nullptr);
                }
            }
        });

    // ??쑬猷욄묾?嚥≪뮆諭???嚥?筌앸맩??null 獄쏆꼹??
    return nullptr;
}

// 筌?Ŋ??癒?퐣 ?醫륁뒞???癒?텦筌?獄쏆꼹?? ?얜똾??遺얜쭆 ????怨뺣뮉 ??볤탢
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
            // ????怨? null??野껋럩??筌?Ŋ??癒?퐣 ??볤탢
            CachedAssets.Remove(AssetId);
            UE_LOG(LogTemp, Warning, TEXT("?醫륁뒞??? ??? 筌?Ŋ????볤탢: %s"), *AssetId.ToString());
        }
    }

    return nullptr;
}

// 筌?Ŋ??癒?퐣 ?????癒?텦 ??볤탢
void UPolyPalsDefenseAssetManager::ClearCachedAsset(const FPrimaryAssetId& AssetId)
{
    if (CachedAssets.Remove(AssetId) > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("筌?Ŋ????癒?텦 ??볤탢 ?袁⑥┷: %s"), *AssetId.ToString());
    }
    else
    {
        // ??????? 筌?Ŋ?????곸뱽 ??野껋럡??
        UE_LOG(LogTemp, Warning, TEXT("筌?Ŋ????癒?텦??鈺곕똻???? ??놁벉 ?癒?뮉 ??? ??볤탢?? %s"), *AssetId.ToString());
    }
}

// ?袁⑸열?癒?퐣 ?臾롫젏 揶쎛?館釉??源???Getter
UPolyPalsDefenseAssetManager& UPolyPalsDefenseAssetManager::Get()
{
    UPolyPalsDefenseAssetManager* Singleton = Cast<UPolyPalsDefenseAssetManager>(GEngine->AssetManager);
    if (!Singleton)
    {
        // ??쇱젟 ??살첒 ??燁살꼶梨??嚥≪뮄???곗뮆??
        UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager class in DefaultEngine.ini. Must be UPolyPalsDefenseAssetManager!"));
    }
    return *Singleton;
}

// AssetRegistry ?癒?뮉 PrimaryAssetObject?????퉸 野껋럥以덄몴?獄쏆꼹??
FSoftObjectPath UPolyPalsDefenseAssetManager::GetPrimaryDataAssetPath(const FPrimaryAssetId& AssetId) const
{
    // ?믪눘? AssetRegistry?癒?퐣 ?怨쀬뵠??鈺곌퀬??
    FAssetData AssetData;
    if (GetPrimaryAssetData(AssetId, AssetData))
    {
        return AssetData.ToSoftObjectPath();
    }

    // ??쎈솭 ??PrimaryAssetObject嚥???뺣즲
    UObject* AssetObject = GetPrimaryAssetObject(AssetId);
    if (AssetObject)
    {
        return FSoftObjectPath(AssetObject);
    }

    // ??????쎈솭??롢늺 ?癒?쑎 嚥≪뮄??獄???野껋럥以?獄쏆꼹??
    UE_LOG(LogTemp, Error, TEXT("GetPrimaryDataAssetPath ??쎈솭 - ??롢걵??PrimaryAssetId: %s"), *AssetId.ToString());
    return FSoftObjectPath();
}

// UAssetManager揶쎛 ??? 癰귣똻???StreamableManager??獄쏆꼹??
FStreamableManager& UPolyPalsDefenseAssetManager::GetStreamableManager()
{
    return UAssetManager::GetStreamableManager();
}
