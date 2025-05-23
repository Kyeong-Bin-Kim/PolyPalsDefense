// GamePlayingUIWidget.cpp
#include "UI/GamePlayingUIWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"

#include "Multiplayer/PolyPalsController.h"
#include "Multiplayer/Components/PolyPalsController/PolyPalsInputComponent.h"

void UGamePlayingUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ToggleTowerButton)
    {
        ToggleTowerButton->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnToggleTowerButtonClicked);
    }

    if (TowerIconsBox)
    {
        TowerIconsBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (Tower1Button)
        Tower1Button->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnTower1ButtonClicked);
    if (Tower2Button)
        Tower2Button->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnTower2ButtonClicked);
    if (Tower3Button)
        Tower3Button->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnTower3ButtonClicked);
}

void UGamePlayingUIWidget::SetRound(int32 Current, int32 Total)
{
    if (RoundText)
    {
        RoundText->SetText(FText::FromString(FString::Printf(TEXT("Round: %d / %d"), Current, Total)));
    }
}

void UGamePlayingUIWidget::SetGold(int32 Gold)
{
    if (GoldText)
    {
        GoldText->SetText(FText::AsNumber(Gold));
    }
}

void UGamePlayingUIWidget::SetLife(int32 Life)
{
    if (LifeText)
    {
        LifeText->SetText(FText::AsNumber(Life));
    }
}

void UGamePlayingUIWidget::SetEnemiesRemaining(int32 Count)
{
    if (EnemiesRemainingText)
    {
        EnemiesRemainingText->SetText(FText::FromString(FString::Printf(TEXT("Enemies Left: %d"), Count)));
    }
}

void UGamePlayingUIWidget::SetNextWaveTime(float Seconds)
{
    if (NextWaveTimeText)
    {
        NextWaveTimeText->SetText(FText::FromString(FString::Printf(TEXT("Next Wave In: %.0f s"), Seconds)));
    }
}

void UGamePlayingUIWidget::OnToggleTowerButtonClicked()
{
    bIsTowerPanelVisible = !bIsTowerPanelVisible;

    if (TowerIconsBox)
    {
        TowerIconsBox->SetVisibility(
            bIsTowerPanelVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
        );
    }
}

void UGamePlayingUIWidget::OnTower1ButtonClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APolyPalsController* Controller = Cast<APolyPalsController>(PC))
        {
            if (UPolyPalsInputComponent* InputComp = Controller->GetPolyPalsInputComponent())
            {
                InputComp->OnInputTower1.ExecuteIfBound();
            }
        }
    }
}

void UGamePlayingUIWidget::OnTower2ButtonClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APolyPalsController* Controller = Cast<APolyPalsController>(PC))
        {
            if (UPolyPalsInputComponent* InputComp = Controller->GetPolyPalsInputComponent())
            {
                InputComp->OnInputTower2.ExecuteIfBound();
            }
        }
    }
}

void UGamePlayingUIWidget::OnTower3ButtonClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APolyPalsController* Controller = Cast<APolyPalsController>(PC))
        {
            if (UPolyPalsInputComponent* InputComp = Controller->GetPolyPalsInputComponent())
            {
                InputComp->OnInputTower3.ExecuteIfBound();
            }
        }
    }
}
