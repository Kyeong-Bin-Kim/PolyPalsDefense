// StageSelectUIWidget.cpp
#include "UI/StageSelectUIWidget.h"
#include "UI/LobbyUIWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void UStageSelectUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ExitGame)
    {
        ExitGame->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnExitGameClicked);
    }
    if (Stage1)
    {
        Stage1->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnEasyClicked);
    }
    if (Stage2)
    {
        Stage2->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnNormalClicked);
    }
    if (Stage3)
    {
        Stage3->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnHardClicked);
    }
}

void UStageSelectUIWidget::OnExitGameClicked()
{
    RemoveFromParent();
}

void UStageSelectUIWidget::OnEasyClicked()
{
    OnStageSelected("Stage1");
}

void UStageSelectUIWidget::OnNormalClicked()
{
    OnStageSelected("Stage2");
}

void UStageSelectUIWidget::OnHardClicked()
{
    OnStageSelected("Stage3");
}

void UStageSelectUIWidget::OnStageSelected(FName StageName)
{
    LastSelectedStage = StageName;
    OpenLobbyUI();
}

void UStageSelectUIWidget::OpenLobbyUI()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        ULobbyUIWidget* LobbyWidget = CreateWidget<ULobbyUIWidget>(
            PC, LoadClass<ULobbyUIWidget>(nullptr, TEXT("/Game/UI/WBP_Lobby.WBP_Lobby_C"))
        );

        if (LobbyWidget)
        {
            LobbyWidget->SetSelectedStage(LastSelectedStage); 
            LobbyWidget->AddToViewport();
        }
    }

    RemoveFromParent();
}
