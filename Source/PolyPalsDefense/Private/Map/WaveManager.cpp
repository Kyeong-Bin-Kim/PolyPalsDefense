#include "WaveManager.h"
#include "WaveSpawner.h"
#include "Enemy/EnemyPawn.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "PolyPalsHUD.h"
#include "Net/UnrealNetwork.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}

void AWaveManager::NotifyWaveInfoChanged()
{
    OnWaveInfoChanged.Broadcast();
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();

    if (APolyPalsState* PState = GetWorld()->GetGameState<APolyPalsState>())
    {
        PState->OnAllPlayersReady.AddDynamic(this, &AWaveManager::OnPlayersReady);
        PState->OnGameOver.AddDynamic(this, &AWaveManager::HandleGameOver);
    }

    if (!WaveSpawner)
    {
        WaveSpawner = Cast<AWaveSpawner>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AWaveSpawner::StaticClass()));
    }

    if (!WaveSpawner)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveManager] No WaveSpawner found!"));
        return;
    }

    if (HasAuthority())
    {
        OnPlayersReady();
    }
}

void AWaveManager::OnPlayersReady()
{
    GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this, &AWaveManager::StartFirstRound,
        PreparationTime,
        false
    );
}

void AWaveManager::HandleGameOver()
{
    GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);

    if (WaveSpawner)
    {
        WaveSpawner->SetActorTickEnabled(false);
        WaveSpawner->SetActorEnableCollision(false);
    }
}

void AWaveManager::StartRound(int32 RoundIndex)
{
    bIsPreparingPhase = false;

    if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
    {
        GS->SetCurrentRound(RoundIndex);
    }

    WaveSpawner->StartWave(RoundIndex);
}

void AWaveManager::StartFirstRound()
{
    bIsPreparingPhase = true;
    StartRound(CurrentRoundIndex);
    RoundStartTimestamp = GetWorld()->GetTimeSeconds();

    GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this, &AWaveManager::EndRound,
        RoundDuration,
        false
    );
}

int32 AWaveManager::GetTotalEnemiesThisWave() const
{
    return WaveSpawner ? WaveSpawner->TotalEnemiesThisWave : 0;
}

int32 AWaveManager::GetRemainingEnemiesThisWave() const
{
    TArray<AActor*> FoundEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyPawn::StaticClass(), FoundEnemies);

    return FoundEnemies.Num();
}



float AWaveManager::GetRoundElapsedTime() const
{
    return GetWorld()->GetTimeSeconds() - RoundStartTimestamp;
}

void AWaveManager::EndRound()
{
    if (GetWorld()->GetGameState<APolyPalsState>()->IsGameOver())
        return;

    if (CurrentRoundIndex >= TotalRoundCount)
    {
        CurrentRoundIndex = TotalRoundCount;

        NotifyWaveInfoChanged();

        if (APolyPalsState* PState = GetWorld()->GetGameState<APolyPalsState>())
        {
            PState->SetGameClear();
        }

        return;
    }

    CurrentRoundIndex++;

    StartRound(CurrentRoundIndex);

    GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this, &AWaveManager::EndRound,
        RoundDuration, false);
}

void AWaveManager::HandleEnemyReachedGoal(AEnemyPawn* Enemy)
{
    if (GetWorld()->GetGameState<APolyPalsState>()->IsGameOver())
        return;

    if (!Enemy)
        return;

    int32 Damage = Enemy->IsBoss() ? BossDamageToLife : BasicDamageToLife;

    PlayerLife -= Damage;

    if (PlayerLife <= 0)
    {
        if (APolyPalsState* PState = GetWorld()->GetGameState<APolyPalsState>())
        {
            PState->SetGameOver();
        }
    }

    if (WaveSpawner)
    {
        WaveSpawner->OnEnemyKilled();
    }

    NotifyWaveInfoChanged();

    Enemy->Destroy();
}

void AWaveManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWaveManager, PlayerLife);
    DOREPLIFETIME(AWaveManager, CurrentRoundIndex);
}

void AWaveManager::OnRep_PlayerLife()
{
    UE_LOG(LogTemp, Warning, TEXT("[Client] OnRep_PlayerLife: %d"), PlayerLife);

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (APolyPalsHUD* HUD = Cast<APolyPalsHUD>(PC->GetHUD()))
        {
            HUD->UpdateWaveInfoOnUI();
        }
    }
}

void AWaveManager::OnRep_CurrentRound()
{
    UE_LOG(LogTemp, Warning, TEXT("[Client] OnRep_CurrentRound: %d"), CurrentRoundIndex);

    NotifyWaveInfoChanged();
}
