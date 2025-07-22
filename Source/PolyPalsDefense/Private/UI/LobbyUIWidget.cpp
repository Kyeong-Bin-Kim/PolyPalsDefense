#include "LobbyUIWidget.h"
#include "PolyPalsState.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsController.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void ULobbyUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ExitGame)
    {
        ExitGame->OnClicked.AddDynamic(this, &ULobbyUIWidget::OnExitGameClicked);
    }

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

    if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
    {
        GS->OnLobbyCountdownUpdated.AddDynamic(this, &ULobbyUIWidget::HandleLobbyCountdownUpdated);
        GS->OnLobbyCountdownStarted.AddDynamic(this, &ULobbyUIWidget::HandleLobbyCountdownStarted);
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
        FString RoomName = FString::Printf(TEXT("%s's Room"), *InRoomTitle);
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

    if (APolyPalsState* GS = GetWorld()->GetGameState<APolyPalsState>())
    {
        int32 ReadyCount = GS->GetReadyPlayers();
        TotalReadyText->SetText(FText::FromString(FString::Printf(TEXT("%d"), ReadyCount)));
    }
}

void ULobbyUIWidget::HandleSlotReadyClicked(UPlayerSlotWidget* ClickedSlot)
{
    if (APlayerController* PC = ClickedSlot->GetOwningPlayer())
    {
        if (APolyPalsController* PPC = Cast<APolyPalsController>(PC))
        {
            if (APolyPalsPlayerState* PS = PC->GetPlayerState<APolyPalsPlayerState>())
            {
                const bool bNowReady = !PS->IsReady();
                PPC->Server_SetReady(bNowReady);
            }
        }
    }
}

void ULobbyUIWidget::RefreshPlayerSlots(const TArray<APlayerState*>& PlayerStates)
{
    TArray<APolyPalsPlayerState*> SortedStates;
    SortedStates.Reserve(PlayerStates.Num());

    for (APlayerState* PS : PlayerStates)
    {
        if (APolyPalsPlayerState* PPS = Cast<APolyPalsPlayerState>(PS))
        {
            SortedStates.Add(PPS);
        }
    }

    SortedStates.Sort([](const APolyPalsPlayerState& A, const APolyPalsPlayerState& B)
        {
            return A.GetSlotIndex() < B.GetSlotIndex();
        });

    int32 Index = 0;

    for (UPlayerSlotWidget* PlayerSlot : PlayerSlotWidgets)
    {
        if (!PlayerSlot)
            continue;

        APolyPalsPlayerState* TargetState = nullptr;
        bool bLocal = false;

        if (Index < SortedStates.Num())
        {
            TargetState = SortedStates[Index];

            if (APlayerController* PC = Cast<APlayerController>(TargetState->GetOwner()))
            {
                bLocal = PC->IsLocalController();
            }
        }

        PlayerSlot->ConfigureSlot(TargetState, bLocal);
        ++Index;
    }
}

void ULobbyUIWidget::HandleLobbyCountdownUpdated(int32 SecondsRemaining)
{
    if (CountdownText)
    {
        if (SecondsRemaining > 0)
        {
            CountdownText->SetText(FText::FromString(FString::Printf(TEXT("%d"), SecondsRemaining)));

            if (ExitGame)
            {
                ExitGame->SetVisibility(ESlateVisibility::Collapsed);
            }
        }
        else
        {
            CountdownText->SetText(FText::GetEmpty());
        }
    }
}

void ULobbyUIWidget::HandleLobbyCountdownStarted()
{
    if (ExitGame)
    {
        ExitGame->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void ULobbyUIWidget::OnExitGameClicked()
{
    if (APolyPalsController* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->LeaveLobby();
    }
}