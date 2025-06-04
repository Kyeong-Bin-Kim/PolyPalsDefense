#include "WaveManager.h"
#include "WaveSpawner.h"
#include "Enemy/EnemyPawn.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();

    // 서버 전용: 클라이언트에서는 이 함수 바로 리턴
    if (!HasAuthority())
        return;

    // 이벤트 구독: 플레이어 준비, 게임오버
    if (APolyPalsState* PState = GetWorld()->GetGameState<APolyPalsState>())
    {
        PState->OnAllPlayersReady.AddDynamic(this, &AWaveManager::OnPlayersReady);
        PState->OnGameOver.AddDynamic(this, &AWaveManager::HandleGameOver);
    }

    // WaveSpawner 바인딩
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
}

void AWaveManager::OnPlayersReady()
{
    UE_LOG(LogTemp, Log, TEXT("[WaveManager] All players ready. Preparing for %.1f sec"), PreparationTime);

    // 준비 시간이 지난 뒤 첫 라운드를 시작
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

    // 게임 오버 시 라운드 타이머 정리 및 스포너 비활성화
    GetWorld()->GetTimerManager().ClearTimer(RoundTimerHandle);
   
    if (WaveSpawner)
    {
        WaveSpawner->SetActorTickEnabled(false);
        WaveSpawner->SetActorEnableCollision(false);
    }
}

void AWaveManager::StartRound(int32 RoundIndex)
{
    if (!HasAuthority() || !WaveSpawner)
        return;

    if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
    {
        GS->SetCurrentRound(RoundIndex);
    }

    UE_LOG(LogTemp, Log, TEXT("[WaveManager] StartRound %d"), RoundIndex);
    WaveSpawner->StartWave(RoundIndex);
}

void AWaveManager::StartFirstRound()
{
    StartRound(CurrentRoundIndex);

    // 시작 시간 기록 (UI용)
    RoundStartTimestamp = GetWorld()->GetTimeSeconds();

    // 종료 타이머 설정
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
    return WaveSpawner ? WaveSpawner->RemainingEnemies : 0;
}

float AWaveManager::GetRoundElapsedTime() const
{
    if (!HasAuthority()) return 0.f;

    return GetWorld()->GetTimeSeconds() - RoundStartTimestamp;
}

void AWaveManager::EndRound()
{
    if (!HasAuthority())
        return;

    // 게임오버 상태면 무시
    if (GetWorld()->GetGameState<APolyPalsState>()->IsGameOver())
        return;

    // 다음 라운드로 이동
    CurrentRoundIndex++;
    UE_LOG(LogTemp, Log, TEXT("[WaveManager] Round ended. Next Round %d"), CurrentRoundIndex);

    StartRound(CurrentRoundIndex);

    // 타이머 재설정
    GetWorld()->GetTimerManager().SetTimer(
        RoundTimerHandle,
        this, &AWaveManager::EndRound,
        RoundDuration, false);
}

void AWaveManager::HandleEnemyReachedGoal(AEnemyPawn* Enemy)
{
    if (!HasAuthority())
        return;

    if (GetWorld()->GetGameState<APolyPalsState>()->IsGameOver())
        return;

    if (!Enemy)
        return;

    int32 Damage = Enemy->IsBoss() ? BossDamageToLife : BasicDamageToLife;
    PlayerLife -= Damage;
    UE_LOG(LogTemp, Warning, TEXT("[WaveManager] Player Life: %d"), PlayerLife);

    Enemy->Destroy();

    if (PlayerLife <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[WaveManager] Game Over!"));

        // GameState를 통해 게임오버 설정
        if (APolyPalsState* PState = GetWorld()->GetGameState<APolyPalsState>())
        {
            PState->SetGameOver();
        }
    }

	// 남은 적 수 업데이트
    if (WaveSpawner)
    {
        WaveSpawner->OnEnemyKilled(); // 새 함수 호출
    }
}
