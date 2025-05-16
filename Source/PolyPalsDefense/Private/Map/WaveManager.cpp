#include "WaveManager.h"
#include "WaveSpawner.h"
#include "Enemy/EnemyPawn.h"
#include "TimerManager.h"
#include "Engine/World.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();
    StartNextRound();
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
    for (AWaveSpawner* Spawner : Spawners)
    {
        if (Spawner)
        {
            Spawner->StartWave(RoundIndex);
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

    int32 Damage;

    if (Enemy->IsBoss()) // IsBoss()는 EnemyPawn에 추가 필요
    {
        Damage = BossLifePenalty;
    }
    else
    {
        Damage = BasicDamage;
    }

    PlayerLife -= Damage;
    UE_LOG(LogTemp, Warning, TEXT("Player Life: %d"), PlayerLife);

    if (PlayerLife <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Game Over!"));
        // 게임 오버 처리 (UI, 재시작 등)
    }

    Enemy->Destroy();
}
