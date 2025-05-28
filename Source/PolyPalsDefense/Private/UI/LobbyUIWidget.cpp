#include "UI/LobbyUIWidget.h"
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

    // 게임 시작 버튼 바인딩
    if (StartGame)
    {
        StartGame->OnClicked.AddDynamic(this, &ULobbyUIWidget::OnStartGameClicked);
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

    if (Stage)
    {
        Stage->SetText(FText::FromName(SelectedStageName));
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

    if (Stage)
    {
        Stage->SetText(FText::FromName(CurrentStage));
    }

    if (StartGame)
    {
        StartGame->SetVisibility(bIsHost ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void ULobbyUIWidget::DisableAllReadyButtons()
{
    for (UPlayerSlotWidget* PlayerSlot : PlayerSlotWidgets)
    {
        if (PlayerSlot)
        {
            PlayerSlot->SetReadyButtonActive(false);
        }
    }
}

void ULobbyUIWidget::OnExitGameClicked()
{
    RemoveFromParent();
}

void ULobbyUIWidget::OnStartGameClicked()
{
    RemoveFromParent();

    UWorld* World = GetWorld();
    if (World)
    {
        UGameplayStatics::OpenLevel(World, FName("TowerTest"), true);
    }
}

void ULobbyUIWidget::HandleSlotReadyClicked(UPlayerSlotWidget* ClickedSlot)
{
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        APolyPalsController* MyPC = Cast<APolyPalsController>(PC);
        if (MyPC)
        {
            MyPC->Server_SetReady(true); // 서버로 Ready 전송
        }
    }
}
