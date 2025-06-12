#include "LobbyUIWidget.h"
#include "PolyPalsState.h"
#include "PolyPalsPlayerState.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PolyPalsController.h"

void ULobbyUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 퇴장 버튼 바인딩
    if (ExitGame)
    {
        ExitGame->OnClicked.AddDynamic(this, &ULobbyUIWidget::OnExitGameClicked);
    }

    // PlayerSlotBox 안의 PlayerSlot들을 모두 수집
    if (PlayerSlotBox)
    {
        for (UWidget* Child : PlayerSlotBox->GetAllChildren())
        {
            if (UPlayerSlotWidget* PlayerSlot = Cast<UPlayerSlotWidget>(Child))
            {
                PlayerSlotWidgets.Add(PlayerSlot);
                PlayerSlot->OnReadyClicked.AddDynamic(this, &ULobbyUIWidget::HandleSlotReadyClicked);
            }
        }
    }
}

void ULobbyUIWidget::SetSelectedStage(FName InStageName)
{
    SelectedStageName = InStageName;

    if (StageText)
    {
        StageText->SetText(FText::FromName(SelectedStageName));
    }
}

void ULobbyUIWidget::SetRoomTitle(const FString& InRoomTitle)
{
    if (RoomTitleText)
    {
        FString RoomName = FString::Printf(TEXT("%s's ROOM"), *InRoomTitle);
        RoomTitleText->SetText(FText::FromString(RoomName));
    }
}

void ULobbyUIWidget::UpdateLobbyInfo(int32 ConnectedPlayers, int32 ReadyPlayers, FName CurrentStage, bool bIsHost)
{
    if (MaxPlayerCountText)
    {
        MaxPlayerCountText->SetText(FText::AsNumber(ConnectedPlayers));
    }

    if (TotalReadyText)
    {
        TotalReadyText->SetText(FText::AsNumber(ReadyPlayers));
    }

    if (StageText)
    {
        StageText->SetText(FText::FromName(CurrentStage));
    }
}

void ULobbyUIWidget::UpdatePlayerSlotReadyState(APolyPalsPlayerState* PlayerState, bool bReady)
{
    for (UPlayerSlotWidget* PlayerSlot : PlayerSlotWidgets)
    {
        if (PlayerSlot && PlayerSlot->GetAssignedPlayerState() == PlayerState)
        {
            PlayerSlot->UpdateReadyVisual(bReady);
            break;
        }
    }

    // 총 준비 수 텍스트 갱신
    if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
    {
        int32 ReadyCount = GS->GetReadyPlayers();
        TotalReadyText->SetText(FText::FromString(FString::Printf(TEXT("%d"), ReadyCount)));
    }
}


void ULobbyUIWidget::OnExitGameClicked()
{
    RemoveFromParent();
}

void ULobbyUIWidget::HandleSlotReadyClicked(UPlayerSlotWidget* ClickedSlot)
{
    if (APlayerController* PC = ClickedSlot->GetOwningPlayer())
    {
        if (APolyPalsPlayerState* PS = PC->GetPlayerState<APolyPalsPlayerState>())
        {
            const bool bNowReady = !PS->IsReady();
            PS->SetReadyState(bNowReady);  // 서버에 상태 전달
        }
    }
}
