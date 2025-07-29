#include "PolyPalsHUD.h"
#include "UI/GamePlayingUIWidget.h"
#include "UI/GameResultWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsState.h"
#include "WaveManager.h"
#include "Components/PolyPalsController/PolyPalsInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void APolyPalsHUD::TryBindToWaveManager()
{
    if (bIsWaveManagerBound)
        return;

    AWaveManager* WaveManager = Cast<AWaveManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass()));

    if (!WaveManager)
    {
        // WaveManager가 없으면 0.5초 후 다시 시도
        GetWorldTimerManager().SetTimer(TimerHandle_FindWaveManager, this, &APolyPalsHUD::TryBindToWaveManager, 1.0f, false);

        return;
    }

    bIsWaveManagerBound = true;

    // 웨이브 정보 자동 업데이트
    WaveManager->OnWaveInfoChanged.AddDynamic(this, &APolyPalsHUD::UpdateWaveInfoOnUI);

    GetWorldTimerManager().SetTimer( TimerHandle_UpdateWaveInfo, this, &APolyPalsHUD::UpdateWaveInfoOnUI, 0.5f, true);

    // 첫 타이머 초기화
    float TargetTime = WaveManager->GetWorld()->GetTimeSeconds() + WaveManager->GetPreparationTime();

    NextWaveTargetTimestamp = TargetTime;
}

void APolyPalsHUD::BeginPlay()
{
    Super::BeginPlay();

    if (GetNetMode() == NM_DedicatedServer)
        return;

    // UI 위젯 생성, 초기 값 세팅
    if (!GamePlayingWidget && GamePlayingWidgetClass)
    {
        GamePlayingWidget = CreateWidget<UGamePlayingUIWidget>(GetWorld(), GamePlayingWidgetClass);
        GamePlayingWidget->AddToViewport();

        // 초기 골드
        if (APlayerController* PC = GetOwningPlayerController())
        {
            if (APolyPalsPlayerState* PS = PC->GetPlayerState<APolyPalsPlayerState>())
            {
                int32 Gold = PS->GetPlayerGold();

                UE_LOG(LogTemp, Warning, TEXT("[HUD::BeginPlay] Read PlayerGold from PS: %d"), Gold);

                GamePlayingWidget->SetGold(Gold);
            }
        }

        // 초기 라운드
        if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
        {
            GamePlayingWidget->SetRound(GS->GetRound(), GS->GetTotalRound());

            GS->OnGameOver.AddDynamic(this, &APolyPalsHUD::HandleGameOver);
            GS->OnGameClear.AddDynamic(this, &APolyPalsHUD::HandleGameClear);
        }
    }

    // WaveManager가 스폰되면 바인딩 시도
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

void APolyPalsHUD::ShowResultWidget(const FText& Message)
{
    if (!GameResultWidget && GameResultWidgetClass)
    {
        GameResultWidget = CreateWidget<UGameResultWidget>(GetWorld(), GameResultWidgetClass);
    }

    if (GameResultWidget)
    {
        GameResultWidget->AddToViewport(100);
        GameResultWidget->SetResultText(Message);
    }

    if (APlayerController* PC = GetOwningPlayerController())
    {
        if (UPolyPalsInputComponent* InputComp = PC->FindComponentByClass<UPolyPalsInputComponent>())
        {
            InputComp->OnInputConfirm.Unbind();
            InputComp->OnInputConfirm.BindUObject(this, &APolyPalsHUD::OnConfirm);
        }
    }
}

void APolyPalsHUD::HandleGameOver()
{
    ShowResultWidget(FText::FromString(TEXT("Game Over")));
}

void APolyPalsHUD::HandleGameClear()
{
    ShowResultWidget(FText::FromString(TEXT("Game Clear")));
}

void APolyPalsHUD::OnConfirm()
{
    if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
    {
        GS->OnGameOver.RemoveDynamic(this, &APolyPalsHUD::HandleGameOver);
        GS->OnGameClear.RemoveDynamic(this, &APolyPalsHUD::HandleGameClear);
    }

    if (APlayerController* PC = GetOwningPlayerController())
    {
        if (UPolyPalsInputComponent* InputComp = PC->FindComponentByClass<UPolyPalsInputComponent>())
        {
            InputComp->OnInputConfirm.Unbind();
        }

        PC->ClientTravel(TEXT("/Game/Maps/EmptyLevel"), ETravelType::TRAVEL_Absolute);
    }
}