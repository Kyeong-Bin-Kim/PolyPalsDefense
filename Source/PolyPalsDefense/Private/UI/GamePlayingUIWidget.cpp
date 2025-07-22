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

    // 踰꾪듉 諛붿씤??
    if (ToggleTowerButton)
    {
        UE_LOG(LogTemp, Log, TEXT("ToggleTowerButton bound"));
        ToggleTowerButton->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnToggleTowerButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ToggleTowerButton is null"));
    }

    if (Tower1Button)
    {
        UE_LOG(LogTemp, Log, TEXT("Tower1Button bound"));
        Tower1Button->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnTower1ButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Tower1Button is null"));
    }

    if (Tower2Button)
    {
        UE_LOG(LogTemp, Log, TEXT("Tower2Button bound"));
        Tower2Button->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnTower2ButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Tower2Button is null"));
    }

    if (Tower3Button)
    {
        UE_LOG(LogTemp, Log, TEXT("Tower3Button bound"));
        Tower3Button->OnClicked.AddDynamic(this, &UGamePlayingUIWidget::OnTower3ButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Tower3Button is null"));
    }

    // 珥덇린?먮뒗 ????⑤꼸 ?④?
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
    UE_LOG(LogTemp, Log, TEXT("Tower1Button clicked"));

    if (auto* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->BeginSelectTower(1);
    }
}

void UGamePlayingUIWidget::OnTower2ButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Tower2Button clicked"));

    if (auto* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->BeginSelectTower(2);
    }
}

void UGamePlayingUIWidget::OnTower3ButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Tower3Button clicked"));

    if (auto* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->BeginSelectTower(3);
    }
}
