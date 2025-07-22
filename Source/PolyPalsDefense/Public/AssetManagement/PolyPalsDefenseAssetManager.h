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
    // ?λ뜃由????λ땾
    virtual void StartInitialLoading() override;

    // ??녿┛ 嚥≪뮆諭?
    UPrimaryDataAsset* LoadPrimaryDataAsset(const FPrimaryAssetId& AssetId);

    // ??쑬猷욄묾?嚥≪뮆諭?
    void LoadPrimaryDataAssetAsync(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback);

    // ??깆뒠 獄쎻뫗?? ??쑬猷욄묾???쎈솭 ????녿┛ 嚥≪뮆諭뜻에???筌?
    UPrimaryDataAsset* LoadDataAssetWithFallback(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback);

    // 筌?Ŋ????癒?텦 揶쎛?紐꾩궎疫?
    UPrimaryDataAsset* GetCachedAsset(const FPrimaryAssetId& AssetId);

    // 筌?Ŋ??癒?퐣 ?癒?텦 ??볤탢
    void ClearCachedAsset(const FPrimaryAssetId& AssetId);

    // ??逾??뺣군 ?臾롫젏??????덈즲嚥?static ??λ땾嚥??醫롫섧
    static UPolyPalsDefenseAssetManager& Get();

private:
    // 筌?Ŋ????癒?텦 筌뤴뫖以?
    TMap<FPrimaryAssetId, UPrimaryDataAsset*> CachedAssets;

    // PrimaryAssetId???怨뺚뀲 野껋럥以?揶쎛?紐꾩궎疫?
    FSoftObjectPath GetPrimaryDataAssetPath(const FPrimaryAssetId& AssetId) const;

    // ??쎈뱜?귐됥돢 筌띲끇??? 揶쎛?紐꾩궎疫?
    FStreamableManager& GetStreamableManager();
	
};
