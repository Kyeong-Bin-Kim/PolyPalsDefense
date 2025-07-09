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

	// 타워 정보 등록 함수
	void SetTargetTower(APlacedTower* InTower);

protected:
	// 버튼 클릭 처리
	UFUNCTION()
	void OnUpgradeClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeButton;

private:
	// 현재 업그레이드 대상 타워
	UPROPERTY()
	APlacedTower* TargetTower;
};
