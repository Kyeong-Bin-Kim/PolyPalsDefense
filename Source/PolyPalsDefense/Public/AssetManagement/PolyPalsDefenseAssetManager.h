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
    // 초기화 함수
    virtual void StartInitialLoading() override;

    // 동기 로드
    UPrimaryDataAsset* LoadPrimaryDataAsset(const FPrimaryAssetId& AssetId);

    // 비동기 로드
    void LoadPrimaryDataAssetAsync(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback);

    // 혼용 방식: 비동기 실패 시 동기 로드로 대체
    UPrimaryDataAsset* LoadDataAssetWithFallback(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback);

    // 캐시된 자산 가져오기
    UPrimaryDataAsset* GetCachedAsset(const FPrimaryAssetId& AssetId);

    // 캐시에서 자산 제거
    void ClearCachedAsset(const FPrimaryAssetId& AssetId);

    // 어디서든 접근할 수 있도록 static 함수로 선언
    static UPolyPalsDefenseAssetManager& Get();

private:
    // 캐시된 자산 목록
    TMap<FPrimaryAssetId, UPrimaryDataAsset*> CachedAssets;

    // PrimaryAssetId에 따른 경로 가져오기
    FSoftObjectPath GetPrimaryDataAssetPath(const FPrimaryAssetId& AssetId) const;

    // 스트리머 매니저 가져오기
    FStreamableManager& GetStreamableManager();
	
};
