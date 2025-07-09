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
    {
        return;
    }

    AWaveManager* WaveManager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass()));
    if (!WaveManager)
    {
        // ?꾩쭅 WaveManager媛 ?녿떎硫?議곌툑 ?ㅼ뿉 ?ㅼ떆 ?쒕룄
        GetWorldTimerManager().SetTimer(TimerHandle_FindWaveManager, this, &APolyPalsHUD::TryBindToWaveManager, 1.0f, false);
        return;
    }

    bIsWaveManagerBound = true;

    // ?꾩젽 ?앹꽦
    if (!GamePlayingWidget && GamePlayingWidgetClass)
    {
        GamePlayingWidget = CreateWidget<UGamePlayingUIWidget>(GetWorld(), GamePlayingWidgetClass);

        if (GamePlayingWidget)
        {
            GamePlayingWidget->AddToViewport();

            // 珥덇린?? ?쇱슫??
            if (const APolyPalsState* GameState = GetWorld()->GetGameState<APolyPalsState>())
            {
                GamePlayingWidget->SetRound(GameState->GetRound(), GameState->GetTotalRound());
            }

            // 珥덇린?? 怨⑤뱶
            if (const APlayerController* PC = GetOwningPlayerController())
            {
                if (const APolyPalsPlayerState* PlayerState = PC->GetPlayerState<APolyPalsPlayerState>())
                {
                    GamePlayingWidget->SetGold(PlayerState->GetPlayerGold());
                }
            }
        }
    }

    // ?⑥씠釉??뺣낫 二쇨린??媛깆떊
    GetWorldTimerManager().SetTimer(
        TimerHandle_UpdateWaveInfo,
        this,
        &APolyPalsHUD::UpdateWaveInfoOnUI,
        0.5f,
        true
    );

    WaveManager->OnWaveInfoChanged.AddDynamic(this, &APolyPalsHUD::UpdateWaveInfoOnUI);

    float TargetTime = WaveManager->GetWorld()->GetTimeSeconds() + WaveManager->GetPreparationTime();
    NextWaveTargetTimestamp = TargetTime;
}

void APolyPalsHUD::BeginPlay()
{
    Super::BeginPlay();

    if (GetNetMode() == NM_DedicatedServer)
    {
        // ?쒕쾭?먯꽌??UI ?앹꽦 X
        return;
    }

    // 寃뚯엫 ?쒖옉 ?쒖젏??WaveManager媛 議댁옱?????덉쑝誘濡?二쇨린?곸쑝濡??뺤씤
    GetWorldTimerManager().SetTimerForNextTick(this, &APolyPalsHUD::TryBindToWaveManager);

    // ?붾쾭洹몄슜 濡쒓렇
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
