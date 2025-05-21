#include "WaveManager.h"
#include "WaveSpawner.h"
#include "Enemy/EnemyPawn.h"
#include "Map/StageActor.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();

    if (!StageRef.IsValid())
    {
        StageRef = Cast<AStageActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageActor::StaticClass()));
        if (!StageRef.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("WaveManager: StageRef is null even after auto search"));
            return;
        }
    }

    for (AWaveSpawner* Spawner : StageRef->WaveSpawners)
    {
        if (Spawner)
        {
            Spawner->StartWave(CurrentRoundIndex);
        }
    }

    GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle, this, &AWaveManager::EndRound,
        RoundDuration, false);
}

void AWaveManager::StartNextRound()
{
    CurrentRoundIndex++;
    StartRound(CurrentRoundIndex);

    GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle, this, &AWaveManager::EndRound,
        RoundDuration, false);
}

void AWaveManager::StartRound(int32 RoundIndex)
{
    if (StageRef)
    {
        for (AWaveSpawner* Spawner : StageRef->WaveSpawners)
        {
            if (Spawner)
            {
                Spawner->StartWave(RoundIndex);
            }
        }
    }
}

void AWaveManager::EndRound()
{
    StartNextRound();
}

void AWaveManager::HandleEnemyReachedGoal(AEnemyPawn* Enemy)
{
    if (!Enemy) return;

    int32 Damage = Enemy->IsBoss() ? BossDamageToLife : BasicDamageToLife;

    PlayerLife -= Damage;
    UE_LOG(LogTemp, Warning, TEXT("Player Life: %d"), PlayerLife);

    if (PlayerLife <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Game Over!"));
        // 게임 오버 처리 (UI, 재시작 등)
    }

    Enemy->Destroy();
}
