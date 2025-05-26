#include "Enemy/EnemyPoolComponent.h"
#include "Enemy/EnemyDataAsset.h"
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
    AEnemyPawn* EnemyPawn = nullptr;

    // 풀에 남은 인스턴스가 있으면 재활용
    if (EnemyPool.Contains(AssetId) && EnemyPool[AssetId].Num() > 0)
    {
        EnemyPawn = EnemyPool[AssetId].Pop();

        // 재활용 시에도 완전 초기화
        EnemyPawn->InitializeFromAssetId(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, Scale);
    }
    else
    {
        // 풀에 없으면 새로 생성
        EnemyPawn = CreateNewEnemy(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, bIsBoss, Scale);
    }

    if (EnemyPawn)
    {
        // 상태 활성화 처리 (가시성/충돌/틱 포함)
        EnemyPawn->SetIsActive(true);
    }

    return EnemyPawn;
}

void UEnemyPoolComponent::ReleaseEnemy(AEnemyPawn* Enemy)
{
    if (!Enemy)
        return;

    // 상태 비활성화 처리 (가시성/충돌/틱 포함)
    Enemy->SetIsActive(false);

    // 풀에 반환
    if (UEnemyDataAsset* Data = Enemy->GetEnemyData())
    {
        EnemyPool.FindOrAdd(Data->GetPrimaryAssetId()).Add(Enemy);
    }
}

AEnemyPawn* UEnemyPoolComponent::CreateNewEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale)
{
    if (!EnemyClass || !GetWorld())
        return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AEnemyPawn* NewEnemyPawn = GetWorld()->SpawnActor<AEnemyPawn>(
        EnemyClass,
        FVector(5000.f, 0.f, 5000.f),
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (NewEnemyPawn)
    {
        NewEnemyPawn->InitializeFromAssetId(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, Scale);
        NewEnemyPawn->SetIsActive(true);
    }

    return NewEnemyPawn;
}
