#include "Enemy/EnemyPoolComponent.h"
#include "Enemy/EnemyDataAsset.h"
#include "Enemy/EnemyPawn.h"
#include "Components/SplineComponent.h"
#include "Map/StageActor.h"
#include "Map/WavePlanRow.h"
#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "EngineUtils.h"
#include "Engine/World.h"

UEnemyPoolComponent::UEnemyPoolComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyPoolComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!GetOwner() || !GetOwner()->HasAuthority())
        return;
}

AEnemyPawn* UEnemyPoolComponent::AcquireEnemy(const FPrimaryAssetId& AssetId, USplineComponent* InSpline, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss, FVector Scale)
{
    AEnemyPawn* EnemyPawn = nullptr;

    if (EnemyPool.Contains(AssetId) && EnemyPool[AssetId].Num() > 0)
    {
        EnemyPawn = EnemyPool[AssetId].Pop();

        EnemyPawn->InitializeFromAssetId(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, Scale);
    }
    else
    {
        EnemyPawn = CreateNewEnemy(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, bIsBoss, Scale);
    }

    if (EnemyPawn)
    {
        EnemyPawn->bIsBoss = bIsBoss;
        EnemyPawn->SetIsActive(true);
    }

    return EnemyPawn;
}

void UEnemyPoolComponent::ReleaseEnemy(AEnemyPawn* Enemy)
{
    if (!Enemy)
        return;

    Enemy->SetIsActive(false);

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
        NewEnemyPawn->bIsBoss = bIsBoss;
        NewEnemyPawn->InitializeFromAssetId(AssetId, InSpline, HealthMultiplier, SpeedMultiplier, Scale);
        NewEnemyPawn->SetIsActive(true);
    }

    return NewEnemyPawn;
}
