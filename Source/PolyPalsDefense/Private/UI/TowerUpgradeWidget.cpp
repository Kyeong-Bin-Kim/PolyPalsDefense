#include "TowerUpgradeWidget.h"
#include "Components/Button.h"
#include "Tower/PlacedTower.h"

void UTowerUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddUniqueDynamic(this, &UTowerUpgradeWidget::OnUpgradeClicked);
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
}
