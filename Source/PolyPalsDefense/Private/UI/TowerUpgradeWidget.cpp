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

void UTowerUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UTowerUpgradeWidget::OnClickedUpgrade()
{
	UE_LOG(LogTemp, Log, TEXT("Detected click upgrade"));
}
