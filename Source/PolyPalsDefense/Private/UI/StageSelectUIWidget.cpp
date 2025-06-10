#include "StageSelectUIWidget.h"
#include "LobbyUIWidget.h"
#include "MainUIWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void UStageSelectUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ExitGame){ExitGame->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnExitGameClicked);}
    if (DirtStage){DirtStage->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnDirtClicked);}
    if (SnowStage){SnowStage->OnClicked.AddDynamic(this, &UStageSelectUIWidget::OnSnowClicked);}
}

void UStageSelectUIWidget::OnDirtClicked()
{
    HandleStageSelected(TEXT("Dirt"));
}

void UStageSelectUIWidget::OnSnowClicked()
{
    HandleStageSelected(TEXT("Snow"));
}


void UStageSelectUIWidget::OnExitGameClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (MainUIWidgetClass)
        {
            UMainUIWidget* MainUI = CreateWidget<UMainUIWidget>(PC, MainUIWidgetClass);

            if (MainUI)
            {
                MainUI->AddToViewport();
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("MainUIWidgetClass is not set on StageSelectUIWidget"));
        }
    }

    RemoveFromParent();  // StageSelect는 제거
}

void UStageSelectUIWidget::HandleStageSelected(FName StageName)
{
    LastSelectedStage = StageName;
    OpenLobbyUI();
}

void UStageSelectUIWidget::OpenLobbyUI()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (LobbyUIWidgetClass)
        {
            ULobbyUIWidget* LobbyWidget = CreateWidget<ULobbyUIWidget>(PC, LobbyUIWidgetClass);
            if (LobbyWidget)
            {
                LobbyWidget->SetSelectedStage(LastSelectedStage);
                LobbyWidget->AddToViewport();
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("LobbyUIWidgetClass not set!"));
        }
    }

    // StageSelect는 제거
    RemoveFromParent();
}
