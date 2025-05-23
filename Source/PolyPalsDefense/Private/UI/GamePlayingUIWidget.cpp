// GamePlayingUIWidget.cpp
#include "UI/GamePlayingUIWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"

void UGamePlayingUIWidget::NativeConstruct()
{
    Super::NativeConstruct();
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
