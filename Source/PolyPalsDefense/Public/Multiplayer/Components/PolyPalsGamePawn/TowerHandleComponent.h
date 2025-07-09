// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/Tower/TowerEnums.h"
#include "TowerHandleComponent.generated.h"

class APlacedTower;
class UTowerUpgradeWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYPALSDEFENSE_API UTowerHandleComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTowerHandleComponent();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * 월드 클릭(RPC나 InputComponent 에서 호출) 시,
     * 클릭된 타워의 업그레이드 위젯을 토글합니다.
     */
    void HandleLeftClick();

protected:
    // 업그레이드 UI를 표시할 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UTowerUpgradeWidget> TowerUpgradeWidgetClass;

private:
    UPROPERTY()
    UTowerUpgradeWidget* UpgradeWidgetInstance = nullptr;

    // 마지막으로 포커스된 타워 포인터
    UPROPERTY()
    APlacedTower* FocusedTower = nullptr;
};
