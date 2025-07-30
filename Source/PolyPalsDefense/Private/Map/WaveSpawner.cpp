#include "WaveSpawner.h"
#include "StageActor.h"
#include "Enemy/EnemyPawn.h"
#include "EnemyPoolComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include "Map/WavePlanRow.h"


AWaveSpawner::AWaveSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    EnemyPool = CreateDefaultSubobject<UEnemyPoolComponent>(TEXT("EnemyPool"));
}

void AWaveSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
        return;

    if (APolyPalsState* PState = GetWorld()->GetGameState<APolyPalsState>())
    {
        PState->OnGameOver.AddDynamic(this, &AWaveSpawner::HandleGameOver);
    }

    if (!StageRef)
    {
        StageRef = Cast<AStageActor>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AStageActor::StaticClass()));
    }

    if (!StageRef)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveSpawner] No StageActor found for StageRef"));

        return;
    }

    SplinePath = StageRef->GetSpline();
    WavePlanTable = StageRef->GetWavePlanTable();
}

void AWaveSpawner::HandleGameOver()
{
    GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
    SetActorTickEnabled(false);
    SetActorEnableCollision(false);
}

void AWaveSpawner::OnEnemyKilled()
{
    RemainingEnemies = FMath::Max(0, RemainingEnemies - 1);
}

void AWaveSpawner::StartWave(int32 RoundIndex)
{
    if (!HasAuthority())
        return;

    if (!WavePlanTable)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveSpawner] Cannot start wave: WavePlanTable is null"));
        return;
    }

    FString Context;
    FName RowName = FName(*FString::Printf(TEXT("Round_%d"), RoundIndex));
    const FWavePlanRow* Plan = WavePlanTable->FindRow<FWavePlanRow>(RowName, Context);

    if (!Plan)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveSpawner] Plan not found for %s"), *RowName.ToString());
        return;
    }

    CurrentSpawnList.Empty();
    for (const FEnemySpawnEntry& Entry : Plan->SpawnList)
        for (int32 i = 0; i < Entry.Count; ++i)
            CurrentSpawnList.Add(Entry);

    int32 TotalToSpawn = CurrentSpawnList.Num();

    if (TotalToSpawn == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[WaveSpawner] No enemies to spawn in Wave %d"), RoundIndex);
        return;
    }

    float SpawnInterval = Plan->SpawnWindow / float(TotalToSpawn);

    SpawnIndex = 0;

    SpawnNextEnemy();

    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        this, &AWaveSpawner::SpawnNextEnemy,
        SpawnInterval,
        true,
        SpawnInterval
    );

    TotalEnemiesThisWave = TotalToSpawn;
    RemainingEnemies = TotalToSpawn;

}


void AWaveSpawner::SpawnNextEnemy()
{
    if (!HasAuthority())
        return;

    if (SpawnIndex >= CurrentSpawnList.Num())
    {
        UE_LOG(LogTemp, Log, TEXT("[WaveSpawner] All %d enemies spawned. Clearing timer."), CurrentSpawnList.Num());
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);

        return;
    }

    const FEnemySpawnEntry& Entry = CurrentSpawnList[SpawnIndex];

    SpawnIndex++;

    AEnemyPawn* Enemy = EnemyPool->AcquireEnemy(
        Entry.EnemyId,
        SplinePath,
        Entry.HealthMultiplier,
        Entry.SpeedMultiplier,
        Entry.bIsBoss,
        Entry.Scale
    );

    if (!Enemy)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveSpawner] AcquireEnemy returned null"));
        return;
    }

    FVector SpawnLocation = SplinePath
        ? SplinePath->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World)
        : FVector::ZeroVector;

    Enemy->SetActorLocation(SpawnLocation);
    Enemy->ForceNetUpdate();
}
