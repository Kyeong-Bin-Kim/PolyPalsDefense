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

void ULobbySlotWidget::SetupSlot(const FString& InLobbyID, const FString& InLobbyName, int32 InCurrentPlayers, int32 InMaxPlayers)
{
    LobbyID = InLobbyID;

    if (LobbyNameText)
    {
        LobbyNameText->SetText(FText::FromString(InLobbyName));
    }

    if (PlayerCountText)
    {
        FString PlayerText = FString::Printf(TEXT("%d/%d"), InCurrentPlayers, InMaxPlayers);
        PlayerCountText->SetText(FText::FromString(PlayerText));
    }
}

void ULobbySlotWidget::HandleJoinClicked()
{
    OnJoinLobbyClicked.Broadcast(LobbyID);
}
