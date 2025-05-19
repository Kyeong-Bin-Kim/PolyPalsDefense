#include "Enemy/EnemyPoolComponent.h"
#include "Enemy/EnemyPawn.h"
#include "AssetManagement/PolyPalsDefenseAssetManager.h"
#include "Engine/World.h"

UEnemyPoolComponent::UEnemyPoolComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyPoolComponent::BeginPlay()
{
    Super::BeginPlay();
}

AEnemyPawn* UEnemyPoolComponent::CreateNewEnemy(const FPrimaryAssetId& AssetId, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss)
{
    if (EnemyPool.Contains(AssetId) && EnemyPool[AssetId].Num() > 0)
    {
        AEnemyPawn* Pooled = EnemyPool[AssetId].Pop();
        Pooled->SetActorHiddenInGame(false);
        Pooled->SetActorEnableCollision(true);
        Pooled->SetActorTickEnabled(true);
        Pooled->bIsBoss = bIsBoss;
        Pooled->InitializeFromAssetId(AssetId, nullptr, HealthMultiplier, SpeedMultiplier);
        return Pooled;
    }

    return CreateNewEnemy(AssetId, HealthMultiplier, SpeedMultiplier, bIsBoss);
}

void UEnemyPoolComponent::ReleaseEnemy(AEnemyPawn* Enemy)
{
    if (!Enemy) return;

    Enemy->SetActorHiddenInGame(true);
    Enemy->SetActorEnableCollision(false);
    Enemy->SetActorTickEnabled(false);

    EnemyPool.FindOrAdd(Enemy->GetPrimaryAssetId()).Add(Enemy);
}

AEnemyPawn* UEnemyPoolComponent::CreateNewEnemy(const FPrimaryAssetId& AssetId, float HealthMultiplier, float SpeedMultiplier, bool bIsBoss)

{
    if (!EnemyClass) return nullptr;

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AEnemyPawn* NewEnemy = World->SpawnActor<AEnemyPawn>(EnemyClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    if (NewEnemy)
    {
        NewEnemy->bIsBoss = bIsBoss;
        NewEnemy->InitializeFromAssetId(AssetId, nullptr, HealthMultiplier, SpeedMultiplier);
    }

    return NewEnemy;
}
