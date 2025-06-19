#include "UI/LobbySlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void ULobbySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(this, &ULobbySlotWidget::HandleJoinClicked);
    }
}

void ULobbySlotWidget::SetupSlot(const FLobbyInfo& Info)
{
    LobbyInfo = Info;

    if (LobbyNameText)
    {
        LobbyNameText->SetText(FText::FromString(LobbyInfo.LobbyName));
    }

    if (PlayerCountText)
    {
        FString PlayerText = FString::Printf(TEXT("%d"), LobbyInfo.CurrentPlayers);
        PlayerCountText->SetText(FText::FromString(PlayerText));
    }

    if (JoinButton)
    {
        JoinButton->SetIsEnabled(LobbyInfo.CurrentPlayers < LobbyInfo.MaxPlayers && !LobbyInfo.bInProgress);
    }
}

void ULobbySlotWidget::HandleJoinClicked()
{
    OnJoinLobbyClicked.Broadcast(LobbyInfo.SessionId);
}
