// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerUpgradeWidget.generated.h"

class APlacedTower;
class UButton;

UCLASS()
class POLYPALSDEFENSE_API UTowerUpgradeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetOwnerByFuckingTower(APlacedTower* const InTower);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnClickedUpgrade();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeButton;
	UPROPERTY()
	TObjectPtr<APlacedTower> OwnerTower;
};
