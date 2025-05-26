#include "UI/PolyPalsHUD.h"
#include "UI/GamePlayingUIWidget.h"
#include "Multiplayer/PolyPalsState.h"
#include "Multiplayer/PolyPalsGameMode.h"

void APolyPalsHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GamePlayingWidgetClass)
	{
		GamePlayingWidget = CreateWidget<UGamePlayingUIWidget>(GetWorld(), GamePlayingWidgetClass);
		if (GamePlayingWidget)
		{
			// GameState에서 골드 정보 조회
			if (const APolyPalsState* GameState = GetWorld()->GetGameState<APolyPalsState>())
			{
				GamePlayingWidget->SetGold(GameState->GetGold());
				GamePlayingWidget->SetRound(GameState->GetRound(), GameState->GetTotalRound());
				//GamePlayingWidget->SetLife(GameState->GetLife()); // GameState에 GetHealth()가 있다면

				// 필요 시 향후 더 많은 정보 Get 함수로 추가 가능
				// 예: GamePlayingWidget->SetLife(GameState->GetLife()); (있다면)
			}

			GamePlayingWidget->AddToViewport();
		}
	}
}
