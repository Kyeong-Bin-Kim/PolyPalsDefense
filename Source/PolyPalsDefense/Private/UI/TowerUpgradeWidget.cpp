<<<<<<< HEAD
// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TowerUpgradeWidget.h"
#include "Tower/PlacedTower.h"
#include "Tower/Components/TowerAttackComponent.h"

#include "Components/Button.h"

void UTowerUpgradeWidget::SetOwnerByFuckingTower(APlacedTower* const InTower)
{
	OwnerTower = InTower;
	UpgradeButton->OnClicked.AddDynamic(OwnerTower->GetAttackComponent(), &UTowerAttackComponent::ClientOnClickedUpgrade);
}
=======
#include "UI/TowerUpgradeWidget.h"
#include "Components/Button.h"
#include "Tower/PlacedTower.h"
>>>>>>> UI

void UTowerUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();

<<<<<<< HEAD
	
}

void UTowerUpgradeWidget::OnClickedUpgrade()
{
	UE_LOG(LogTemp, Log, TEXT("Detected click upgrade"));
=======
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
>>>>>>> UI
}
