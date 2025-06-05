#include "TowerUpgradeWidget.h"
#include "Components/Button.h"
#include "Tower/PlacedTower.h"

void UTowerUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddDynamic(this, &UTowerUpgradeWidget::OnUpgradeClicked);
	}
}

void UTowerUpgradeWidget::SetTargetTower(APlacedTower* InTower)
{
	TargetTower = InTower;
}

void UTowerUpgradeWidget::OnUpgradeClicked()
{
	if (TargetTower)
	{
		TargetTower->UpgradeTower();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetTower is null!"));
	}
}
