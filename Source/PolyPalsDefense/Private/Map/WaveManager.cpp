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
    bReplicates = true; // 由ы뵆由ъ??댁뀡 ?쒖꽦??
}

void AWaveManager::NotifyWaveInfoChanged()
{
    UE_LOG(LogTemp, Warning, TEXT("[WaveManager] NotifyWaveInfoChanged() called"));
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
    UE_LOG(LogTemp, Log, TEXT("[WaveManager] All players ready. Preparing for %.1f sec"), PreparationTime);

    GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this, &AWaveManager::StartFirstRound,
        PreparationTime,
        false
    );
}

void AWaveManager::HandleGameOver()
{
    UE_LOG(LogTemp, Warning, TEXT("[WaveSpawner] HandleGameOver() fired"));

    GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);

    if (WaveSpawner)
    {
        WaveSpawner->SetActorTickEnabled(false);
        WaveSpawner->SetActorEnableCollision(false);
    }
}

void AWaveManager::StartRound(int32 RoundIndex)
{
    //if (HasAuthority() || !WaveSpawner)
    //    return;

    bIsPreparingPhase = false;
    if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
    {
        GS->SetCurrentRound(RoundIndex);
    }

    UE_LOG(LogTemp, Log, TEXT("[WaveManager] StartRound %d"), RoundIndex);
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
    //if (!HasAuthority()) return 0.f;

    return GetWorld()->GetTimeSeconds() - RoundStartTimestamp;
}

void AWaveManager::EndRound()
{
    //if (!HasAuthority())
    //    return;

    if (GetWorld()->GetGameState<APolyPalsState>()->IsGameOver())
        return;

    CurrentRoundIndex++;
    UE_LOG(LogTemp, Log, TEXT("[WaveManager] Round ended. Next Round %d"), CurrentRoundIndex);

    StartRound(CurrentRoundIndex);

    GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this, &AWaveManager::EndRound,
        RoundDuration, false);
}

void AWaveManager::HandleEnemyReachedGoal(AEnemyPawn* Enemy)
{
    //if (!HasAuthority())
    //    return;

    if (GetWorld()->GetGameState<APolyPalsState>()->IsGameOver())
        return;

    if (!Enemy)
        return;

    int32 Damage = Enemy->IsBoss() ? BossDamageToLife : BasicDamageToLife;
    PlayerLife -= Damage;
    UE_LOG(LogTemp, Warning, TEXT("[WaveManager] Player Life: %d"), PlayerLife);

    if (PlayerLife <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveManager] Game Over!"));

        if (APolyPalsState* PState = GetWorld()->GetGameState<APolyPalsState>())
        {
            PState->SetGameOver();
        }
    }

    if (WaveSpawner)
    {
        WaveSpawner->OnEnemyKilled();
    }

    NotifyWaveInfoChanged(); // ?쒕쾭??Broadcast

    Enemy->Destroy();
}

// 由ы뵆由ъ??댁뀡 ?띿꽦 ?ㅼ젙
void AWaveManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWaveManager, PlayerLife); // 湲곗〈
    DOREPLIFETIME(AWaveManager, CurrentRoundIndex); // 異붽?
}

// ?대씪?댁뼵?몄뿉??媛?蹂듭젣 ???ㅽ뻾
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

    NotifyWaveInfoChanged(); // UI??諛섏쁺?섎룄濡??몃━寃뚯씠???몄텧
}
