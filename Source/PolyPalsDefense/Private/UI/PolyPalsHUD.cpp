#include "PolyPalsHUD.h"
#include "GamePlayingUIWidget.h"
#include "PolyPalsState.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsGameMode.h"

void APolyPalsHUD::BeginPlay()
{
    Super::BeginPlay();

    if (GamePlayingWidgetClass)
    {
        GamePlayingWidget = CreateWidget<UGamePlayingUIWidget>(GetWorld(), GamePlayingWidgetClass);
        if (GamePlayingWidget)
        {
            // 전체 게임 상태 정보 (라운드, 총 골드 등)
            if (const APolyPalsState* GameState = GetWorld()->GetGameState<APolyPalsState>())
            {
                GamePlayingWidget->SetRound(GameState->GetRound(), GameState->GetTotalRound());
            }

            // 내 개인 정보 (내 골드 등)
            if (const APlayerController* PC = GetOwningPlayerController())
            {
                if (const APolyPalsPlayerState* PlayerState = PC->GetPlayerState<APolyPalsPlayerState>())
                {
                    GamePlayingWidget->SetGold(PlayerState->GetPlayerGold());
                }
            }

            GamePlayingWidget->AddToViewport();
        }
    }
}
