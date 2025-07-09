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
    // TowerData ?ъ슜 濡쒖쭅
}

// 2. Asynchronous Load Example
FPrimaryAssetId EnemyId(TEXT("DataAsset"), TEXT("Enemy_Spider"));
UPolyPalsDefenseAssetManager::Get().LoadPrimaryDataAssetAsync(
    EnemyId,
    [](UPrimaryDataAsset* LoadedAsset)
    {
        if (LoadedAsset)
        {
            // LoadedAsset ?ъ슜 濡쒖쭅
        }
        else
        {
            // 濡쒕뱶 ?ㅽ뙣 泥섎━
        }
    }
);

// 3. Cached Retrieval Example
UPrimaryDataAsset* CachedTower = UPolyPalsDefenseAssetManager::Get().GetCachedAsset(TowerId);
if (CachedTower)
{
    // CachedTower ?ъ슜
}

// 4. Clear Cache Example
UPolyPalsDefenseAssetManager::Get().ClearCachedAsset(TowerId);

// 5. DefaultEngine.ini??PrimaryAssetTypesToScan ?덉떆
+PrimaryAssetTypesToScan=(
    PrimaryAssetType="Enemis",                                              // ?먮뵒?곗뿉???ㅼ젙??Primary Asset Type ?대쫫
    AssetBaseClass="/Script/PolyPalsDefense.EnemiesDataAssets",             // ?대떦 ??낆쓽 湲곕낯 UObject ?대옒??寃쎈줈
    bHasBlueprintClasses=False,                                             // 釉붾（?꾨┛???먯떇 ?대옒???ы븿 ?щ?
    bIsEditorOnly=False,                                                    // ?먮뵒???꾩슜 ?щ?
    Directories=((Path="/Game/PrimaryDataAssets/Enemies")),                 // ?ㅼ틪???붾젆?곕━ 寃쎈줈
    SpecificAssets=(),                                                      // 媛쒕퀎 ?먯뀑 吏??(?놁쑝硫?鍮?諛곗뿴)
    Rules=(
        Priority=-1,                                                        // 濡쒕뵫 ?곗꽑?쒖쐞 (??쓣?섎줉 癒쇱?)
        ChunkId=-1,                                                         // ?⑦궎吏 泥?겕 ID
        bApplyRecursively=True,                                             // ?섏쐞 ?대뜑源뚯? ?ш? ?곸슜 ?щ?
        CookRule=AlwaysCook                                                 // 荑좏궧 猷??ㅼ젙 (AlwaysCook, NeverCook ??
    )
)

// DefaultEngine.ini?먯꽌???ㅼ틪 ?ㅼ젙??異⑸텇?섎?濡?PrimaryAssetLabels ?뱀뀡? 蹂꾨룄 異붽? 遺덊븘?뷀빀?덈떎.
*/

// ?꾨줈?앺듃 ?쒖옉 ??珥덇린 濡쒕뵫 怨쇱젙???대떦?섎뒗 ?⑥닔
void UPolyPalsDefenseAssetManager::StartInitialLoading()
{
    // 1. GConfig ?먯꽌 以묐났 PrimaryAssetTypesToScan ??ぉ(?? TowerMaterials) ?쒓굅
    {
        const FString Section = TEXT("/Script/Engine.AssetManagerSettings");
        TArray<FString> AllEntries;
        GConfig->GetArray(*Section, TEXT("PrimaryAssetTypesToScan"), AllEntries, GEngineIni);

        // TypeName ?쇰줈 以묐났 泥댄겕??Map
        TMap<FString, FString> UniqueEntriesMap;
        for (const FString& Entry : AllEntries)
        {
            FString ParsedType;
            if (FParse::Value(*Entry, TEXT("PrimaryAssetType=\""), ParsedType, /*bStrip*/false))
            {
                // ?ル뒗 ?곗샂???꾧퉴吏 ?섎씪?닿린
                if (int32 Pos; ParsedType.FindChar(TEXT('"'), Pos))
                {
                    ParsedType = ParsedType.Left(Pos);
                }
                UniqueEntriesMap.Add(ParsedType, Entry);
            }
            else
            {
                // TypeName ?뚯떛???ㅽ뙣?섎㈃ ?쇰떒 洹몃?濡??④?
                UniqueEntriesMap.Add(Entry, Entry);
            }
        }

        // TMap ??媛?Value)留?戮묒븘??TArray 濡?
        TArray<FString> DedupedEntries;
        DedupedEntries.Reserve(UniqueEntriesMap.Num());
        for (auto& Pair : UniqueEntriesMap)
        {
            DedupedEntries.Add(Pair.Value);
        }

        // GConfig ????뼱?곌린 (?좊땲?ы븳 寃껊쭔)
        GConfig->SetArray(
            *Section,
            TEXT("PrimaryAssetTypesToScan"),
            DedupedEntries,
            GEngineIni
        );
    }

    // 2. ?뺤젣???ㅼ젙?쇰줈 遺紐??대옒??珥덇린???ㅽ뻾 (?ш린??ScanPrimaryAssetTypesFromConfig 源뚯? ?먮룞 ?몄텧)
    Super::StartInitialLoading();

    UE_LOG(LogTemp, Log, TEXT("PolyPalsDefenseAssetManager Initialized (with duplicates filtered)"));
}

// ?숆린 諛⑹떇?쇰줈 PrimaryDataAsset??濡쒕뱶?섍굅??罹먯떆?먯꽌 諛섑솚
UPrimaryDataAsset* UPolyPalsDefenseAssetManager::LoadPrimaryDataAsset(const FPrimaryAssetId& AssetId)
{
    // ?대? 罹먯떆??議댁옱?섎㈃ 洹몃?濡?諛섑솚
    if (CachedAssets.Contains(AssetId))
    {
        return CachedAssets[AssetId];
    }

    // AssetId????묓븯??寃쎈줈瑜??띾뱷
    FSoftObjectPath AssetPath = GetPrimaryDataAssetPath(AssetId);
    if (!AssetPath.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("?섎せ??Asset Path: %s"), *AssetId.ToString());
        return nullptr;
    }

    // ?ㅽ듃由щ㉧ 留ㅻ땲?瑜??ъ슜???숆린 濡쒕뱶 ?붿껌
    GetStreamableManager().RequestSyncLoad(AssetPath);

    // 濡쒕뱶 ?꾨즺??媛앹껜瑜?媛?몄? 罹먯뒪??
    UPrimaryDataAsset* LoadedAsset = Cast<UPrimaryDataAsset>(GetPrimaryAssetObject(AssetId));

    // 濡쒕뱶 ?깃났 ??罹먯떆??異붽??섍퀬 濡쒓렇 異쒕젰
    if (LoadedAsset)
    {
        CachedAssets.Add(AssetId, LoadedAsset);
        UE_LOG(LogTemp, Log, TEXT("?숆린 濡쒕뱶 諛?罹먯떛 ?꾨즺: %s"), *AssetId.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("?숆린 濡쒕뱶 ?ㅽ뙣: %s"), *AssetId.ToString());
    }

    return LoadedAsset;
}

// 鍮꾨룞湲?諛⑹떇?쇰줈 PrimaryDataAsset??濡쒕뱶?섍퀬 ?꾨즺 ??肄쒕갚 ?몄텧
void UPolyPalsDefenseAssetManager::LoadPrimaryDataAssetAsync(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback)
{
    // 濡쒕뱶??AssetId 由ъ뒪???앹꽦
    TArray<FPrimaryAssetId> AssetsToLoad = { AssetId };

    // 濡쒕뱶 ?꾨즺 ???몄텧??Delegate ?앹꽦
    FAssetManagerAcquireResourceDelegate OnComplete = FAssetManagerAcquireResourceDelegate::CreateLambda([this, AssetId, OnLoadedCallback](bool bWasSuccessful)
        {
            if (bWasSuccessful)
            {
                // 鍮꾨룞湲?濡쒕뱶 ?깃났 ??媛앹껜 ?띾뱷
                UPrimaryDataAsset* LoadedAsset = Cast<UPrimaryDataAsset>(GetPrimaryAssetObject(AssetId));
                if (LoadedAsset)
                {
                    // 罹먯떆??異붽? 諛??깃났 濡쒓렇 ??肄쒕갚 ?ㅽ뻾
                    CachedAssets.Add(AssetId, LoadedAsset);
                    UE_LOG(LogTemp, Log, TEXT("鍮꾨룞湲?濡쒕뱶 諛?罹먯떛 ?꾨즺: %s"), *AssetId.ToString());
                    OnLoadedCallback(LoadedAsset);
                }
                else
                {
                    // 媛앹껜 議고쉶 ?ㅽ뙣 ???먮윭 濡쒓렇 諛?null 肄쒕갚
                    UE_LOG(LogTemp, Error, TEXT("鍮꾨룞湲?濡쒕뱶 ?ㅽ뙣 - 媛앹껜瑜?李얠쓣 ???놁쓬: %s"), *AssetId.ToString());
                    OnLoadedCallback(nullptr);
                }
            }
            else
            {
                // 鍮꾨룞湲?濡쒕뱶 ?먯껜 ?ㅽ뙣 ???먮윭 濡쒓렇 諛?null 肄쒕갚
                UE_LOG(LogTemp, Error, TEXT("鍮꾨룞湲?濡쒕뱶 ?ㅽ뙣 - Delegate ?몄텧 ?ㅽ뙣: %s"), *AssetId.ToString());
                OnLoadedCallback(nullptr);
            }
        });

    // 吏?뺥븳 ?곗꽑?쒖쐞濡?鍮꾨룞湲?濡쒕뱶 ?붿껌
    AcquireResourcesForPrimaryAssetList(AssetsToLoad, OnComplete, EChunkPriority::Immediate);
}

// 鍮꾨룞湲?濡쒕뱶 ?ㅽ뙣 ???숆린 濡쒕뱶濡??泥댄븯???쇳빀 濡쒕뱶 ?⑥닔
UPrimaryDataAsset* UPolyPalsDefenseAssetManager::LoadDataAssetWithFallback(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback)
{
    // ?대? 罹먯떆???먯궛???덉쑝硫?利됱떆 肄쒕갚 ?ㅽ뻾
    if (UPrimaryDataAsset* CachedAsset = GetCachedAsset(AssetId))
    {
        OnLoadedCallback(CachedAsset);
        return CachedAsset;
    }

    // 鍮꾨룞湲?濡쒕뱶 ?쒕룄
    LoadPrimaryDataAssetAsync(AssetId, [this, AssetId, OnLoadedCallback](UPrimaryDataAsset* AsyncAsset)
        {
            if (AsyncAsset)
            {
                // 鍮꾨룞湲??깃났 ??肄쒕갚
                OnLoadedCallback(AsyncAsset);
            }
            else
            {
                // 鍮꾨룞湲??ㅽ뙣 ???숆린 濡쒕뱶 ?ъ떆??
                UE_LOG(LogTemp, Warning, TEXT("鍮꾨룞湲?濡쒕뱶 ?ㅽ뙣, ?숆린 濡쒕뱶 ?쒕룄: %s"), *AssetId.ToString());
                UPrimaryDataAsset* SyncAsset = LoadPrimaryDataAsset(AssetId);
                if (SyncAsset)
                {
                    OnLoadedCallback(SyncAsset);
                }
                else
                {
                    // ?숆린 濡쒕뱶???ㅽ뙣 ???먮윭 肄쒕갚
                    UE_LOG(LogTemp, Error, TEXT("?숆린 濡쒕뱶???ㅽ뙣: %s"), *AssetId.ToString());
                    OnLoadedCallback(nullptr);
                }
            }
        });

    // 鍮꾨룞湲?濡쒕뱶?대?濡?利됱떆 null 諛섑솚
    return nullptr;
}

// 罹먯떆?먯꽌 ?좏슚???먯궛留?諛섑솚, 臾댄슚?붾맂 ?ъ씤?곕뒗 ?쒓굅
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
            // ?ъ씤?곌? null??寃쎌슦 罹먯떆?먯꽌 ?쒓굅
            CachedAssets.Remove(AssetId);
            UE_LOG(LogTemp, Warning, TEXT("?좏슚?섏? ?딆? 罹먯떆 ?쒓굅: %s"), *AssetId.ToString());
        }
    }

    return nullptr;
}

// 罹먯떆?먯꽌 ?대떦 ?먯궛 ?쒓굅
void UPolyPalsDefenseAssetManager::ClearCachedAsset(const FPrimaryAssetId& AssetId)
{
    if (CachedAssets.Remove(AssetId) > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("罹먯떛???먯궛 ?쒓굅 ?꾨즺: %s"), *AssetId.ToString());
    }
    else
    {
        // ?대떦 ?ㅺ? 罹먯떆???놁쓣 ??寃쎄퀬
        UE_LOG(LogTemp, Warning, TEXT("罹먯떆???먯궛??議댁옱?섏? ?딆쓬 ?먮뒗 ?대? ?쒓굅?? %s"), *AssetId.ToString());
    }
}

// ?꾩뿭?먯꽌 ?묎렐 媛?ν븳 ?깃???Getter
UPolyPalsDefenseAssetManager& UPolyPalsDefenseAssetManager::Get()
{
    UPolyPalsDefenseAssetManager* Singleton = Cast<UPolyPalsDefenseAssetManager>(GEngine->AssetManager);
    if (!Singleton)
    {
        // ?ㅼ젙 ?ㅻ쪟 ??移섎챸??濡쒓렇 異쒕젰
        UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager class in DefaultEngine.ini. Must be UPolyPalsDefenseAssetManager!"));
    }
    return *Singleton;
}

// AssetRegistry ?먮뒗 PrimaryAssetObject瑜??듯빐 寃쎈줈瑜?諛섑솚
FSoftObjectPath UPolyPalsDefenseAssetManager::GetPrimaryDataAssetPath(const FPrimaryAssetId& AssetId) const
{
    // 癒쇱? AssetRegistry?먯꽌 ?곗씠??議고쉶
    FAssetData AssetData;
    if (GetPrimaryAssetData(AssetId, AssetData))
    {
        return AssetData.ToSoftObjectPath();
    }

    // ?ㅽ뙣 ??PrimaryAssetObject濡??쒕룄
    UObject* AssetObject = GetPrimaryAssetObject(AssetId);
    if (AssetObject)
    {
        return FSoftObjectPath(AssetObject);
    }

    // ?????ㅽ뙣?섎㈃ ?먮윭 濡쒓렇 諛?鍮?寃쎈줈 諛섑솚
    UE_LOG(LogTemp, Error, TEXT("GetPrimaryDataAssetPath ?ㅽ뙣 - ?섎せ??PrimaryAssetId: %s"), *AssetId.ToString());
    return FSoftObjectPath();
}

// UAssetManager媛 ?대? 蹂댁쑀??StreamableManager瑜?諛섑솚
FStreamableManager& UPolyPalsDefenseAssetManager::GetStreamableManager()
{
    return UAssetManager::GetStreamableManager();
}
