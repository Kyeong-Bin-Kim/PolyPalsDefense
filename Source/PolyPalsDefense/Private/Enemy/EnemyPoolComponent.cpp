#include "EnemyPoolComponent.h"
#include "Enemy/EnemyPawn.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AssetManagement/PolyPalsDefenseAssetManager.h"

UEnemyPoolComponent::UEnemyPoolComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyPoolComponent::BeginPlay()
{
    Super::BeginPlay();
}

AEnemyPawn* UEnemyPoolComponent::AcquireEnemy(const FPrimaryAssetId& AssetId)
{ 
    if (EnemyPool.Contains(AssetId) && EnemyPool[AssetId].Num() > 0)
    {
        AEnemyPawn* Pooled = EnemyPool[AssetId].Pop();
        if (Pooled)
        {
            Pooled->SetActorHiddenInGame(false);
            Pooled->SetActorEnableCollision(true);
            return Pooled;
        }
    }

    if (!EnemyClass) return nullptr;

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AEnemyPawn* Spawned = World->SpawnActor<AEnemyPawn>(EnemyClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
    if (Spawned)
    {
        Spawned->InitializeFromAssetId(AssetId, nullptr);
    }

    return Spawned;
}

void UEnemyPoolComponent::ReleaseEnemy(AEnemyPawn* Enemy)
{
    if (!Enemy || !Enemy->IsValidLowLevel()) return;

    FPrimaryAssetId Id = Enemy->GetPrimaryAssetId();

    Enemy->SetActorHiddenInGame(true);
    Enemy->SetActorEnableCollision(false);

    EnemyPool.FindOrAdd(Id).Add(Enemy);
}
