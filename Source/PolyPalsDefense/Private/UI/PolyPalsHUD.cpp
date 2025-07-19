#include "PolyPalsHUD.h"
#include "UI/GamePlayingUIWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsState.h"
#include "WaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void APolyPalsHUD::TryBindToWaveManager()
{
    if (bIsWaveManagerBound)
        return;

    AWaveManager* WaveManager = Cast<AWaveManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass())
    );
    if (!WaveManager)
    {
        // ���� WaveManager�� ������ 0.5�� �� �ٽ� �õ�
        GetWorldTimerManager().SetTimer(
            TimerHandle_FindWaveManager,
            this,
            &APolyPalsHUD::TryBindToWaveManager,
            0.5f,
            false
        );
        return;
    }

    bIsWaveManagerBound = true;

    // ���̺� ���� �ڵ� ������Ʈ
    WaveManager->OnWaveInfoChanged.AddDynamic(this, &APolyPalsHUD::UpdateWaveInfoOnUI);

    GetWorldTimerManager().SetTimer(
        TimerHandle_UpdateWaveInfo,
        this,
        &APolyPalsHUD::UpdateWaveInfoOnUI,
        0.5f,
        true
    );

    // ù Ÿ�̸� �ʱ�ȭ
    float TargetTime = WaveManager->GetWorld()->GetTimeSeconds()
        + WaveManager->GetPreparationTime();
    NextWaveTargetTimestamp = TargetTime;
}

void APolyPalsHUD::BeginPlay()
{
    Super::BeginPlay();

    if (GetNetMode() == NM_DedicatedServer)
        return;

    // UI ���� ����, �ʱ� �� ����
    if (!GamePlayingWidget && GamePlayingWidgetClass)
    {
        GamePlayingWidget = CreateWidget<UGamePlayingUIWidget>(GetWorld(), GamePlayingWidgetClass);
        GamePlayingWidget->AddToViewport();

        // �ʱ� ���
        if (APlayerController* PC = GetOwningPlayerController())
        {
            if (APolyPalsPlayerState* PS = PC->GetPlayerState<APolyPalsPlayerState>())
            {
                GamePlayingWidget->SetGold(PS->GetPlayerGold());
            }
        }

        // �ʱ� ����
        if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
        {
            GamePlayingWidget->SetRound(GS->GetRound(), GS->GetTotalRound());
        }
    }

    // WaveManager�� �����Ǹ� ���ε� �õ�
    GetWorldTimerManager().SetTimerForNextTick(this, &APolyPalsHUD::TryBindToWaveManager);

    UE_LOG(LogTemp, Warning, TEXT("[HUD] BeginPlay - NetMode: %d"), static_cast<int32>(GetNetMode()));
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
