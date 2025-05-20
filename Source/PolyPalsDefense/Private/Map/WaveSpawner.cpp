#include "WaveSpawner.h"
#include "EnemyPoolComponent.h"
#include "EnemyPawn.h"
#include "Components/SplineComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

AWaveSpawner::AWaveSpawner()
{
    PrimaryActorTick.bCanEverTick = true;

    EnemyPool = CreateDefaultSubobject<UEnemyPoolComponent>(TEXT("EnemyPool"));
}

void AWaveSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (StageRef)
    {
        SplinePath = StageRef->GetSpline();
        UE_LOG(LogTemp, Log, TEXT("WaveSpawner: SplinePath set from StageRef"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WaveSpawner: StageRef is not assigned"));
    }
}

void AWaveSpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWaveSpawner::StartWave(int32 RoundIndex)
{
    if (!WavePlanTable) return;

    FString Context;
    FName RowName = FName(*FString::Printf(TEXT("Round_%d"), RoundIndex));
    const FWavePlanRow* Plan = WavePlanTable->FindRow<FWavePlanRow>(RowName, Context);

    if (!Plan) return;

    CurrentSpawnList.Empty();

    for (const FEnemySpawnEntry& Entry : Plan->SpawnList)
    {
        for (int32 i = 0; i < Entry.Count; i++)
        {
            CurrentSpawnList.Add(Entry);
        }
    }

    SpawnIndex = 0;

    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle, this, &AWaveSpawner::SpawnNextEnemy,
        Plan->SpawnInterval, true);
}

void AWaveSpawner::SpawnNextEnemy()
{
    if (SpawnIndex >= CurrentSpawnList.Num())
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
        return;
    }

    const FEnemySpawnEntry& Entry = CurrentSpawnList[SpawnIndex++];

    AEnemyPawn* Enemy = EnemyPool->AcquireEnemy(Entry.EnemyId, Entry.HealthMultiplier, Entry.SpeedMultiplier, Entry.bIsBoss, Entry.Scale);
    if (Enemy)
    {
        FVector SpawnLocation = SplinePath->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
        Enemy->SetActorLocation(SpawnLocation);
    }
}