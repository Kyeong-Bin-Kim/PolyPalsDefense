#include "UI/PolPalsDefenseHUD.h"
#include "Components/TextBlock.h"

void UPolPalsDefenseHUD::SetHealth(int32 HP)
{
    if (HealthText)
        HealthText->SetText(FText::AsNumber(HP));
}

void UPolPalsDefenseHUD::SetGold(int32 Amount)
{
    if (GoldText)
        GoldText->SetText(FText::AsNumber(Amount));
}

void UPolPalsDefenseHUD::SetRound(int32 Current, int32 Total)
{
    if (RoundText)
    {
        FString Text = FString::Printf(TEXT("%d / %d"), Current, Total);
        RoundText->SetText(FText::FromString(Text));
    }
}
