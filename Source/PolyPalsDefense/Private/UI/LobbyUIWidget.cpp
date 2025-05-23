// LobbyUIWidget.cpp
#include "UI/LobbyUIWidget.h"
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

    if (StartGame)
    {
        StartGame->OnClicked.AddDynamic(this, &ULobbyUIWidget::OnStartGameClicked);
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
}

void ULobbyUIWidget::SetSelectedStage(FName InStageName)
{
    SelectedStageName = InStageName;
    UE_LOG(LogTemp, Log, TEXT("LobbyUIWidget received selected stage: %s"), *SelectedStageName.ToString());

    if (Stage)
    {
        FString DisplayText = FString::Printf(TEXT("%s"), *SelectedStageName.ToString());
        Stage->SetText(FText::FromString(DisplayText));
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
    UE_LOG(LogTemp, Log, TEXT("Player slot ready clicked: %s"), *ClickedSlot->GetName());
}
