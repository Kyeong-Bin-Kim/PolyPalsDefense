<<<<<<< HEAD
// Fill out your copyright notice in the Description page of Project Settings.

=======
>>>>>>> UI
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerUpgradeWidget.generated.h"

<<<<<<< HEAD
class APlacedTower;
class UButton;
=======
class UButton;
class APlacedTower;
>>>>>>> UI

UCLASS()
class POLYPALSDEFENSE_API UTowerUpgradeWidget : public UUserWidget
{
	GENERATED_BODY()
<<<<<<< HEAD

public:
	void SetOwnerByFuckingTower(APlacedTower* const InTower);
=======
	
public:
	UFUNCTION(BlueprintCallable)
	void SetTargetTower(APlacedTower* InTower);
>>>>>>> UI

protected:
	virtual void NativeConstruct() override;

<<<<<<< HEAD
private:
	UFUNCTION()
	void OnClickedUpgrade();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeButton;
	UPROPERTY()
	TObjectPtr<APlacedTower> OwnerTower;
=======
	UFUNCTION()
	void OnUpgradeButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeButton;

	UPROPERTY()
	TObjectPtr<APlacedTower> TargetTower;
>>>>>>> UI
};
