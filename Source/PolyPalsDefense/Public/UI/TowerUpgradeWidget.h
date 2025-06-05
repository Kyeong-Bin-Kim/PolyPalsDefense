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

	// Ÿ�� ���� ��� �Լ�
	void SetTargetTower(APlacedTower* InTower);

protected:
	// ��ư Ŭ�� ó��
	UFUNCTION()
	void OnUpgradeClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeButton;

private:
	// ���� ���׷��̵� ��� Ÿ��
	UPROPERTY()
	APlacedTower* TargetTower;
};
