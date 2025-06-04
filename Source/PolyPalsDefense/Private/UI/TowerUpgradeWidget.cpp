#include "UI/TowerUpgradeWidget.h"
#include "Components/Button.h"
#include "Tower/PlacedTower.h"

void UTowerUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddDynamic(this, &UTowerUpgradeWidget::OnUpgradeButtonClicked);
	}
}

void UTowerUpgradeWidget::SetTargetTower(APlacedTower* InTower)
{
	TargetTower = InTower;
}

void UTowerUpgradeWidget::OnUpgradeButtonClicked()
{
	if (TargetTower)
	{
		//TargetTower->UpgradeTower(); // 실제 업그레이드 처리
	}
}
