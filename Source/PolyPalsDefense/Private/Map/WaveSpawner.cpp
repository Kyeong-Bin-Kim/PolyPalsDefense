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

    // GameOver ?대깽??援щ룆
    if (APolyPalsState* PState = GetWorld()->GetGameState<APolyPalsState>())
    {
        PState->OnGameOver.AddDynamic(this, &AWaveSpawner::HandleGameOver);
    }

    // StageActor 李얘린/諛붿씤??
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

    // ?ㅽ뵆?쇱씤 諛?DataTable 珥덇린??
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
    // 寃뚯엫 ?ㅻ쾭 ???ㅽ룿 ??대㉧ ?뺣━ 諛??≫꽣 鍮꾪솢?깊솕
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

    // ?대쾲 ?쇱슫??怨꾪쉷 濡쒕뱶
    FString Context;
    FName RowName = FName(*FString::Printf(TEXT("Round_%d"), RoundIndex));
    const FWavePlanRow* Plan = WavePlanTable->FindRow<FWavePlanRow>(RowName, Context);
    if (!Plan)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveSpawner] Plan not found for %s"), *RowName.ToString());
        return;
    }

    // ?꾩껜 ?ㅽ룿 ???紐⑸줉 ?묒꽦
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

    // SpawnWindow 湲곕컲 ?명꽣踰?怨꾩궛
    float SpawnInterval = Plan->SpawnWindow / float(TotalToSpawn);
    UE_LOG(LogTemp, Log,
        TEXT("[WaveSpawner] Wave %d: will spawn %d enemies over %.2f sec ??interval=%.3f sec"),
        RoundIndex, TotalToSpawn, Plan->SpawnWindow, SpawnInterval);

    // ?몃뜳??珥덇린??
    SpawnIndex = 0;

    // 利됱떆 泥????ㅽ룿
    SpawnNextEnemy();

    // ?댄썑 ?명꽣踰뚮쭏???ㅽ룿
    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        this, &AWaveSpawner::SpawnNextEnemy,
        SpawnInterval,
        true,            // bLoop
        SpawnInterval    // InitialDelay: ?ㅼ쓬 ?몄텧源뚯? 湲곕떎由??쒓컙
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
    Enemy->ForceNetUpdate(); // 蹂듭젣 ?숆린??
}
