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

    // GameOver ??源???닌됰즴
    if (APolyPalsState* PState = GetWorld()->GetGameState<APolyPalsState>())
    {
        PState->OnGameOver.AddDynamic(this, &AWaveSpawner::HandleGameOver);
    }

    // StageActor 筌≪뼐由?獄쏅뗄???
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
    UE_LOG(LogTemp, Log, TEXT("[WaveSpawner] Found StageActor: %s"), *StageRef->GetName());

    // ??쎈탣??깆뵥 獄?DataTable ?λ뜃由??
    SplinePath = StageRef->GetSpline();
    WavePlanTable = StageRef->GetWavePlanTable();

    if (!SplinePath)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveSpawner] SplinePath is null"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[WaveSpawner] SplinePath initialized"));
    }

    if (!WavePlanTable)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveSpawner] WavePlanTable is null"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[WaveSpawner] WavePlanTable initialized"));
    }
}

void AWaveSpawner::HandleGameOver()
{
    // 野껊슣????살쒔 ????쎈？ ???????類ｂ봺 獄???リ숲 ??쑵??源딆넅
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

    // ??苡???깆뒲???④쑵??嚥≪뮆諭?
    FString Context;
    FName RowName = FName(*FString::Printf(TEXT("Round_%d"), RoundIndex));
    const FWavePlanRow* Plan = WavePlanTable->FindRow<FWavePlanRow>(RowName, Context);
    if (!Plan)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveSpawner] Plan not found for %s"), *RowName.ToString());
        return;
    }

    // ?袁⑷퍥 ??쎈？ ????筌뤴뫖以??臾믨쉐
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

    // SpawnWindow 疫꿸퀡而??紐낃숲甕??④쑴沅?
    float SpawnInterval = Plan->SpawnWindow / float(TotalToSpawn);
    UE_LOG(LogTemp, Log,
        TEXT("[WaveSpawner] Wave %d: will spawn %d enemies over %.2f sec ??interval=%.3f sec"),
        RoundIndex, TotalToSpawn, Plan->SpawnWindow, SpawnInterval);

    // ?紐껊쑔???λ뜃由??
    SpawnIndex = 0;

    // 筌앸맩??筌?????쎈？
    SpawnNextEnemy();

    // ??꾩뜎 ?紐낃숲甕곕슢彛????쎈？
    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        this, &AWaveSpawner::SpawnNextEnemy,
        SpawnInterval,
        true,            // bLoop
        SpawnInterval    // InitialDelay: ??쇱벉 ?紐꾪뀱繹먮슣? 疫꿸퀡?롧뵳???볦퍢
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
        UE_LOG(LogTemp, Log, TEXT("[WaveSpawner] All %d enemies spawned. Clearing timer."),
            CurrentSpawnList.Num());
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
    Enemy->ForceNetUpdate(); // 癰귣벊????녿┛??
}
