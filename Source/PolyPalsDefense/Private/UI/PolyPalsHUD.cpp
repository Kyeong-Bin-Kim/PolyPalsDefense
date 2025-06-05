#include "PolyPalsHUD.h"
#include "UI/GamePlayingUIWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsState.h"
#include "WaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void APolyPalsHUD::BeginPlay()
{
    Super::BeginPlay();
    if (GetNetMode() != NM_Client)
    {
        // 서버에서는 UI 생성 X
        return;
    }
    // 위젯 생성
    if (GamePlayingWidgetClass)
    {
        GamePlayingWidget = CreateWidget<UGamePlayingUIWidget>(GetWorld(), GamePlayingWidgetClass);
        if (GamePlayingWidget)
        {
            GamePlayingWidget->AddToViewport();

            // 초기화: 라운드
            if (const APolyPalsState* GameState = GetWorld()->GetGameState<APolyPalsState>())
            {
                GamePlayingWidget->SetRound(GameState->GetRound(), GameState->GetTotalRound());
            }

            // 초기화: 골드
            if (const APlayerController* PC = GetOwningPlayerController())
            {
                if (const APolyPalsPlayerState* PlayerState = PC->GetPlayerState<APolyPalsPlayerState>())
                {
                    GamePlayingWidget->SetGold(PlayerState->GetPlayerGold());
                }
            }
        }
    }

    // 웨이브 정보 주기적 갱신
    GetWorldTimerManager().SetTimer(
        TimerHandle_UpdateWaveInfo,
        this,
        &APolyPalsHUD::UpdateWaveInfoOnUI,
        0.5f,
        true
    );

    // 웨이브 매니저 델리게이트 바인딩
    if (AWaveManager* WaveManager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass())))
    {
        WaveManager->OnWaveInfoChanged.AddDynamic(this, &APolyPalsHUD::UpdateWaveInfoOnUI);

        // 다음 웨이브 예정 시간 미리 받아둠
        float TargetTime = WaveManager->GetWorld()->GetTimeSeconds() + WaveManager->GetPreparationTime();
        NextWaveTargetTimestamp = TargetTime;
    }

    // 디버그용 로그
    if (GEngine)
    {
        ENetMode NetMode = GetNetMode();
        UE_LOG(LogTemp, Warning, TEXT("[HUD] BeginPlay - NetMode: %d"), static_cast<int32>(NetMode));
    }
}

void APolyPalsHUD::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (GamePlayingWidget && NextWaveTargetTimestamp > 0)
    {
        float RemainingTime = FMath::Max(0.f, NextWaveTargetTimestamp - GetWorld()->GetTimeSeconds());
        GamePlayingWidget->SetNextWaveTime(RemainingTime);
    }
}

void APolyPalsHUD::UpdateGoldOnUI(int32 NewGold)
{
    if (GamePlayingWidget)
    {
        GamePlayingWidget->SetGold(NewGold);
    }
}

void APolyPalsHUD::UpdateWaveInfoOnUI()
{
    if (!GamePlayingWidget) return;

    AWaveManager* WaveManager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass()));
    if (!WaveManager) return;

    GamePlayingWidget->SetLife(WaveManager->GetRemainingLives());

    const int32 CurrentEnemies = WaveManager->GetRemainingEnemiesThisWave();
    const int32 TotalEnemies = WaveManager->GetTotalEnemiesThisWave();
    GamePlayingWidget->SetEnemiesRemaining(CurrentEnemies, TotalEnemies);

    GamePlayingWidget->SetRound(WaveManager->GetCurrentRoundIndex(), WaveManager->GetTotalRoundCount());

    float RemainingTime = FMath::Max(0.f, NextWaveTargetTimestamp - GetWorld()->GetTimeSeconds());
    if (RemainingTime <= 0.f)
    {
        if (WaveManager->IsInPreparationPhase())
        {
            NextWaveTargetTimestamp = GetWorld()->GetTimeSeconds() + WaveManager->GetRoundDuration();
        }
        else
        {
            NextWaveTargetTimestamp = GetWorld()->GetTimeSeconds() + WaveManager->GetPreparationTime();
        }
    }
}
