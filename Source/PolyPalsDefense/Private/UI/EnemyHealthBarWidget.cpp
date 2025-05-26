#include "UI/EnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"

void UEnemyHealthBarWidget::SetHealthBarRatio(float Ratio)
{
    if (HealthProgressBar)
    {
        HealthProgressBar->SetPercent(FMath::Clamp(Ratio, 0.f, 1.f));
    }
}
