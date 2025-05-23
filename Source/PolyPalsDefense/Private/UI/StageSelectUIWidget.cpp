// StageSelectUIWidget.cpp
#include "UI/StageSelectUIWidget.h"
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
    if (Easy)
    {
        Easy->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnEasyClicked);
    }
    if (Normal)
    {
        Normal->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnNormalClicked);
    }
    if (Hard)
    {
        Hard->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnHardClicked);
    }
}

void UStageSelectUIWidget::OnExitGameClicked()
{
    RemoveFromParent();
}

void UStageSelectUIWidget::OnEasyClicked()
{
    OnStageSelected("Easy");
}

void UStageSelectUIWidget::OnNormalClicked()
{
    OnStageSelected("Normal");
}

void UStageSelectUIWidget::OnHardClicked()
{
    OnStageSelected("Hard");
}

void UStageSelectUIWidget::OnStageSelected(FName StageName)
{
    // TODO: Set difficulty variable here if needed: GGameDifficulty = StageName;
    OpenLobbyUI();
}

void UStageSelectUIWidget::OpenLobbyUI()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        UUserWidget* LobbyWidget = CreateWidget(PC, LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/WBP_Lobby.WBP_Lobby_C")));
        if (LobbyWidget)
        {
            LobbyWidget->AddToViewport();
        }
    }
    RemoveFromParent();
}
