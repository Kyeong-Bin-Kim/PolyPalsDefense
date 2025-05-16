#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/PrimaryAssetLabel.h"
#include "Engine/World.h"

/*
Usage Examples:

// 1. Synchronous Load Example
FPrimaryAssetId TowerId(TEXT("DataAsset"), TEXT("Tower_Basic"));
UPrimaryDataAsset* TowerData = UPolyPalsDefenseAssetManager::Get().LoadPrimaryDataAsset(TowerId);
if (TowerData)
{
    // TowerData 사용 로직
}

// 2. Asynchronous Load Example
FPrimaryAssetId EnemyId(TEXT("DataAsset"), TEXT("Enemy_Spider"));
UPolyPalsDefenseAssetManager::Get().LoadPrimaryDataAssetAsync(
    EnemyId,
    [](UPrimaryDataAsset* LoadedAsset)
    {
        if (LoadedAsset)
        {
            // LoadedAsset 사용 로직
        }
        else
        {
            // 로드 실패 처리
        }
    }
);

// 3. Cached Retrieval Example
UPrimaryDataAsset* CachedTower = UPolyPalsDefenseAssetManager::Get().GetCachedAsset(TowerId);
if (CachedTower)
{
    // CachedTower 사용
}

// 4. Clear Cache Example
UPolyPalsDefenseAssetManager::Get().ClearCachedAsset(TowerId);

// 5. DefaultEngine.ini의 PrimaryAssetTypesToScan 예시
+PrimaryAssetTypesToScan=(
    PrimaryAssetType="Enemis",                                              // 에디터에서 설정한 Primary Asset Type 이름
    AssetBaseClass="/Script/PolyPalsDefense.EnemiesDataAssets",             // 해당 타입의 기본 UObject 클래스 경로
    bHasBlueprintClasses=False,                                             // 블루프린트 자식 클래스 포함 여부
    bIsEditorOnly=False,                                                    // 에디터 전용 여부
    Directories=((Path="/Game/PrimaryDataAssets/Enemies")),                 // 스캔할 디렉터리 경로
    SpecificAssets=(),                                                      // 개별 에셋 지정 (없으면 빈 배열)
    Rules=(
        Priority=-1,                                                        // 로딩 우선순위 (낮을수록 먼저)
        ChunkId=-1,                                                         // 패키지 청크 ID
        bApplyRecursively=True,                                             // 하위 폴더까지 재귀 적용 여부
        CookRule=AlwaysCook                                                 // 쿠킹 룰 설정 (AlwaysCook, NeverCook 등)
    )
)

// DefaultEngine.ini에서는 스캔 설정이 충분하므로 PrimaryAssetLabels 섹션은 별도 추가 불필요합니다.
*/

// 프로젝트 시작 시 초기 로딩 과정을 담당하는 함수
void UPolyPalsDefenseAssetManager::StartInitialLoading()
{
    // 부모 클래스의 초기 로딩 로직 호출
    Super::StartInitialLoading();

    // 1. 초기화 로그 출력
    UE_LOG(LogTemp, Log, TEXT("PolyPalsDefenseAssetManager Initialized"));

    // 2. DefaultEngine.ini의 PrimaryAssetTypesToScan 설정을 기반으로 스캔 수행
    ScanPrimaryAssetTypesFromConfig();
    UE_LOG(LogTemp, Log, TEXT("PolyPalsDefenseAssetManager ScanPrimaryAssetTypesFromConfig"));
}

// 동기 방식으로 PrimaryDataAsset을 로드하거나 캐시에서 반환
UPrimaryDataAsset* UPolyPalsDefenseAssetManager::LoadPrimaryDataAsset(const FPrimaryAssetId& AssetId)
{
    // 이미 캐시에 존재하면 그대로 반환
    if (CachedAssets.Contains(AssetId))
    {
        return CachedAssets[AssetId];
    }

    // AssetId에 대응하는 경로를 획득
    FSoftObjectPath AssetPath = GetPrimaryDataAssetPath(AssetId);
    if (!AssetPath.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("잘못된 Asset Path: %s"), *AssetId.ToString());
        return nullptr;
    }

    // 스트리머 매니저를 사용해 동기 로드 요청
    GetStreamableManager().RequestSyncLoad(AssetPath);

    // 로드 완료된 객체를 가져와 캐스팅
    UPrimaryDataAsset* LoadedAsset = Cast<UPrimaryDataAsset>(GetPrimaryAssetObject(AssetId));

    // 로드 성공 시 캐시에 추가하고 로그 출력
    if (LoadedAsset)
    {
        CachedAssets.Add(AssetId, LoadedAsset);
        UE_LOG(LogTemp, Log, TEXT("동기 로드 및 캐싱 완료: %s"), *AssetId.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("동기 로드 실패: %s"), *AssetId.ToString());
    }

    return LoadedAsset;
}

// 비동기 방식으로 PrimaryDataAsset을 로드하고 완료 시 콜백 호출
void UPolyPalsDefenseAssetManager::LoadPrimaryDataAssetAsync(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback)
{
    // 로드할 AssetId 리스트 생성
    TArray<FPrimaryAssetId> AssetsToLoad = { AssetId };

    // 로드 완료 시 호출될 Delegate 생성
    FAssetManagerAcquireResourceDelegate OnComplete = FAssetManagerAcquireResourceDelegate::CreateLambda([this, AssetId, OnLoadedCallback](bool bWasSuccessful)
        {
            if (bWasSuccessful)
            {
                // 비동기 로드 성공 시 객체 획득
                UPrimaryDataAsset* LoadedAsset = Cast<UPrimaryDataAsset>(GetPrimaryAssetObject(AssetId));
                if (LoadedAsset)
                {
                    // 캐시에 추가 및 성공 로그 후 콜백 실행
                    CachedAssets.Add(AssetId, LoadedAsset);
                    UE_LOG(LogTemp, Log, TEXT("비동기 로드 및 캐싱 완료: %s"), *AssetId.ToString());
                    OnLoadedCallback(LoadedAsset);
                }
                else
                {
                    // 객체 조회 실패 시 에러 로그 및 null 콜백
                    UE_LOG(LogTemp, Error, TEXT("비동기 로드 실패 - 객체를 찾을 수 없음: %s"), *AssetId.ToString());
                    OnLoadedCallback(nullptr);
                }
            }
            else
            {
                // 비동기 로드 자체 실패 시 에러 로그 및 null 콜백
                UE_LOG(LogTemp, Error, TEXT("비동기 로드 실패 - Delegate 호출 실패: %s"), *AssetId.ToString());
                OnLoadedCallback(nullptr);
            }
        });

    // 지정한 우선순위로 비동기 로드 요청
    AcquireResourcesForPrimaryAssetList(AssetsToLoad, OnComplete, EChunkPriority::Immediate);
}

// 비동기 로드 실패 시 동기 로드로 대체하는 혼합 로드 함수
UPrimaryDataAsset* UPolyPalsDefenseAssetManager::LoadDataAssetWithFallback(const FPrimaryAssetId& AssetId, TFunction<void(UPrimaryDataAsset*)> OnLoadedCallback)
{
    // 이미 캐시된 자산이 있으면 즉시 콜백 실행
    if (UPrimaryDataAsset* CachedAsset = GetCachedAsset(AssetId))
    {
        OnLoadedCallback(CachedAsset);
        return CachedAsset;
    }

    // 비동기 로드 시도
    LoadPrimaryDataAssetAsync(AssetId, [this, AssetId, OnLoadedCallback](UPrimaryDataAsset* AsyncAsset)
        {
            if (AsyncAsset)
            {
                // 비동기 성공 시 콜백
                OnLoadedCallback(AsyncAsset);
            }
            else
            {
                // 비동기 실패 시 동기 로드 재시도
                UE_LOG(LogTemp, Warning, TEXT("비동기 로드 실패, 동기 로드 시도: %s"), *AssetId.ToString());
                UPrimaryDataAsset* SyncAsset = LoadPrimaryDataAsset(AssetId);
                if (SyncAsset)
                {
                    OnLoadedCallback(SyncAsset);
                }
                else
                {
                    // 동기 로드도 실패 시 에러 콜백
                    UE_LOG(LogTemp, Error, TEXT("동기 로드도 실패: %s"), *AssetId.ToString());
                    OnLoadedCallback(nullptr);
                }
            }
        });

    // 비동기 로드이므로 즉시 null 반환
    return nullptr;
}

// 캐시에서 유효한 자산만 반환, 무효화된 포인터는 제거
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
            // 포인터가 null인 경우 캐시에서 제거
            CachedAssets.Remove(AssetId);
            UE_LOG(LogTemp, Warning, TEXT("유효하지 않은 캐시 제거: %s"), *AssetId.ToString());
        }
    }

    return nullptr;
}

// 캐시에서 해당 자산 제거
void UPolyPalsDefenseAssetManager::ClearCachedAsset(const FPrimaryAssetId& AssetId)
{
    if (CachedAssets.Remove(AssetId) > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("캐싱된 자산 제거 완료: %s"), *AssetId.ToString());
    }
    else
    {
        // 해당 키가 캐시에 없을 때 경고
        UE_LOG(LogTemp, Warning, TEXT("캐시된 자산이 존재하지 않음 또는 이미 제거됨: %s"), *AssetId.ToString());
    }
}

// 전역에서 접근 가능한 싱글턴 Getter
UPolyPalsDefenseAssetManager& UPolyPalsDefenseAssetManager::Get()
{
    UPolyPalsDefenseAssetManager* Singleton = Cast<UPolyPalsDefenseAssetManager>(GEngine->AssetManager);
    if (!Singleton)
    {
        // 설정 오류 시 치명적 로그 출력
        UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager class in DefaultEngine.ini. Must be UPolyPalsDefenseAssetManager!"));
    }
    return *Singleton;
}

// AssetRegistry 또는 PrimaryAssetObject를 통해 경로를 반환
FSoftObjectPath UPolyPalsDefenseAssetManager::GetPrimaryDataAssetPath(const FPrimaryAssetId& AssetId) const
{
    // 먼저 AssetRegistry에서 데이터 조회
    FAssetData AssetData;
    if (GetPrimaryAssetData(AssetId, AssetData))
    {
        return AssetData.ToSoftObjectPath();
    }

    // 실패 시 PrimaryAssetObject로 시도
    UObject* AssetObject = GetPrimaryAssetObject(AssetId);
    if (AssetObject)
    {
        return FSoftObjectPath(AssetObject);
    }

    // 둘 다 실패하면 에러 로그 및 빈 경로 반환
    UE_LOG(LogTemp, Error, TEXT("GetPrimaryDataAssetPath 실패 - 잘못된 PrimaryAssetId: %s"), *AssetId.ToString());
    return FSoftObjectPath();
}

// UAssetManager가 내부 보유한 StreamableManager를 반환
FStreamableManager& UPolyPalsDefenseAssetManager::GetStreamableManager()
{
    return UAssetManager::GetStreamableManager();
}
