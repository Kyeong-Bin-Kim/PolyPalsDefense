#include "UI/GamePlayingUIWidget.h"
#include "PolyPalsGamePawn.h"
#include "Components/PolyPalsGamePawn/BuildTowerComponent.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "PolyPalsController.h"
#include "Engine/Engine.h"

void UGamePlayingUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ToggleTowerButton)
    {
        ToggleTowerButton->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnToggleTowerButtonClicked);
    }

    if (Tower1Button)
    {
        Tower1Button->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnTower1ButtonClicked);
    }

    if (Tower2Button)
    {
        Tower2Button->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnTower2ButtonClicked);
    }

    if (Tower3Button)
    {
        Tower3Button->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnTower3ButtonClicked);
    }

    if (TowerIconsBox)
    {
        TowerIconsBox->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UGamePlayingUIWidget::SetRound(int32 Current, int32 Total)
{
    if (RoundText)
    {
        RoundText->SetText(FText::FromString(FString::Printf(TEXT("Round: %d "), Current)));
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

void UGamePlayingUIWidget::SetEnemiesRemaining(int32 Current, int32 Total)
{
    if (EnemiesRemainingText)
    {
        FString Text = FString::Printf(TEXT("Enemies Left: %d"), Current);
        EnemiesRemainingText->SetText(FText::FromString(Text));
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
            bIsTowerPanelVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UGamePlayingUIWidget::OnTower1ButtonClicked()
{
    if (auto* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->BeginSelectTower(1);
    }
}

void UGamePlayingUIWidget::OnTower2ButtonClicked()
{
    if (auto* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->BeginSelectTower(2);
    }
}

void UGamePlayingUIWidget::OnTower3ButtonClicked()
{
    if (auto* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->BeginSelectTower(3);
    }
}
