#include "Enemy/EnemyPoolComponent.h"
#include "Enemy/EnemyPawn.h"
#include "Components/SplineComponent.h"
#include "Map/StageActor.h"
#include "Map/WavePlanRow.h"
#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"

UEnemyPoolComponent::UEnemyPoolComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyPoolComponent::BeginPlay()
{
    Super::BeginPlay();

    // 서버 권한이 아니면 풀링 로직 스킵
    if (!GetOwner() || !GetOwner()->HasAuthority())
        return;
}

AEnemyPawn* UEnemyPoolComponent::AcquireEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale)
{
    // 획득할 EnemyPawn 포인터 선언
    AEnemyPawn* EnemyPawn = nullptr;

    // 풀에 남은 인스턴스가 있으면 재활용
    if (EnemyPool.Contains(AssetId) && EnemyPool[AssetId].Num() > 0)
    {
        EnemyPawn = EnemyPool[AssetId].Pop();
    }
    else
    {
        // 풀에 없으면 새로 생성
        EnemyPawn = CreateNewEnemy(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, bIsBoss, Scale);
    }

    if (EnemyPawn)
    {
        // 복제 플래그
        EnemyPawn->bIsActive = true;
        EnemyPawn->SetNetDormancy(DORM_Awake);

        // 풀링용 숨김 해제 (서버·클라이언트 공통)
        EnemyPawn->SetActorHiddenInGame(false);
        EnemyPawn->SetActorEnableCollision(true);
        EnemyPawn->SetActorTickEnabled(true);

        // 즉시 네트워크 동기화
        EnemyPawn->ForceNetUpdate();
    }

    return EnemyPawn;
}

void UEnemyPoolComponent::ReleaseEnemy(AEnemyPawn* Enemy)
{
    if (!Enemy)
        return;

    // 비활성화 상태로 변경하고 네트워크 휴면 모드 진입
    Enemy->bIsActive = false;
    Enemy->SetNetDormancy(DORM_DormantAll);
    Enemy->ForceNetUpdate();

    // 풀에 반환
    EnemyPool.FindOrAdd(Enemy->GetPrimaryAssetId()).Add(Enemy);
}

AEnemyPawn* UEnemyPoolComponent::CreateNewEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale)
{
    if (!EnemyClass)
        return nullptr;

    UWorld* World = GetWorld();
    if (!World)
        return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = nullptr;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 서버 전용 SpawnActor 호출
    AEnemyPawn* NewEnemyPawn = World->SpawnActor<AEnemyPawn>(
        EnemyClass,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (NewEnemyPawn)
    {
        // 초기화 로직 수행
        NewEnemyPawn->InitializeFromAssetId(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, Scale);

        // 활성화 상태로 설정하고 네트워크로 전파
        NewEnemyPawn->bIsActive = true;
        NewEnemyPawn->SetNetDormancy(DORM_Awake);
        NewEnemyPawn->ForceNetUpdate();
    }

    return NewEnemyPawn;
}
