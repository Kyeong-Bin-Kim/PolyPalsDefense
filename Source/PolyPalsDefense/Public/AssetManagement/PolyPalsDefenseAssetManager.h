#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PolyPalsDefenseAssetManager.generated.h"

UCLASS()
class POLYPALSDEFENSE_API UPolyPalsDefenseAssetManager : public UAssetManager
{
    GENERATED_BODY()

public:
    // 珥덇린???⑥닔
    virtual void StartInitialLoading() override;

    // ?숆린 濡쒕뱶
    UPrimaryDataAsset* LoadPrimaryDataAsset(const FPrimaryAssetId& AssetId);

    // 鍮꾨룞湲?濡쒕뱶
    void LoadPrimaryDataAssetAsync(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback);

    // ?쇱슜 諛⑹떇: 鍮꾨룞湲??ㅽ뙣 ???숆린 濡쒕뱶濡??泥?
    UPrimaryDataAsset* LoadDataAssetWithFallback(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback);

    // 罹먯떆???먯궛 媛?몄삤湲?
    UPrimaryDataAsset* GetCachedAsset(const FPrimaryAssetId& AssetId);

    // 罹먯떆?먯꽌 ?먯궛 ?쒓굅
    void ClearCachedAsset(const FPrimaryAssetId& AssetId);

    // ?대뵒?쒕뱺 ?묎렐?????덈룄濡?static ?⑥닔濡??좎뼵
    static UPolyPalsDefenseAssetManager& Get();

private:
    // 罹먯떆???먯궛 紐⑸줉
    TMap<FPrimaryAssetId, UPrimaryDataAsset*> CachedAssets;

    // PrimaryAssetId???곕Ⅸ 寃쎈줈 媛?몄삤湲?
    FSoftObjectPath GetPrimaryDataAssetPath(const FPrimaryAssetId& AssetId) const;

    // ?ㅽ듃由щ㉧ 留ㅻ땲? 媛?몄삤湲?
    FStreamableManager& GetStreamableManager();
	
};
