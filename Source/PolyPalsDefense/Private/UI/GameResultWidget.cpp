#include "GameResultWidget.h"
#include "Components/TextBlock.h"

void UGameResultWidget::SetResultText(const FText& InText)
{
    if (ResultText)
    {
        ResultText->SetText(InText);
    }
}