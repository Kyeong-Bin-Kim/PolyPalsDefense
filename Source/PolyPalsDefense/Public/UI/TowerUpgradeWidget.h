#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerUpgradeWidget.generated.h"

class UButton;
class APlacedTower;

UCLASS()
class POLYPALSDEFENSE_API UTowerUpgradeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetTargetTower(APlacedTower* InTower);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnUpgradeButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeButton;

	UPROPERTY()
	TObjectPtr<APlacedTower> TargetTower;
};
