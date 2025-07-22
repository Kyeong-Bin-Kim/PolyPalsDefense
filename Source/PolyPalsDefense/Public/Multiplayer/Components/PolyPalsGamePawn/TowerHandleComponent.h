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
     * ?붾뱶 ?대┃(RPC??InputComponent ?먯꽌 ?몄텧) ??
     * ?대┃????뚯쓽 ?낃렇?덉씠???꾩젽???좉??⑸땲??
     */
    void HandleLeftClick();

protected:
    // ?낃렇?덉씠??UI瑜??쒖떆???꾩젽 ?대옒??
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UTowerUpgradeWidget> TowerUpgradeWidgetClass;

private:
    UPROPERTY()
    UTowerUpgradeWidget* UpgradeWidgetInstance = nullptr;

    // 留덉?留됱쑝濡??ъ빱?ㅻ맂 ????ъ씤??
    UPROPERTY()
    APlacedTower* FocusedTower = nullptr;
};
