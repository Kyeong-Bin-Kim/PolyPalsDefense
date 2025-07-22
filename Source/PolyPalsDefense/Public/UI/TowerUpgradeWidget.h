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
	virtual void NativeConstruct() override;

	// ????뺣낫 ?깅줉 ?⑥닔
	void SetTargetTower(APlacedTower* InTower);

protected:
	// 踰꾪듉 ?대┃ 泥섎━
	UFUNCTION()
	void OnUpgradeClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeButton;

private:
	// ?꾩옱 ?낃렇?덉씠????????
	UPROPERTY()
	APlacedTower* TargetTower;
};
