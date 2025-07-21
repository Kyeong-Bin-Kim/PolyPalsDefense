#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PolyPalsHUD.generated.h"

class UGamePlayingUIWidget;

UCLASS()
class POLYPALSDEFENSE_API APolyPalsHUD : public AHUD
{
    GENERATED_BODY()

public:
    UFUNCTION()
    void UpdateGoldOnUI(int32 NewGold);

    UFUNCTION()
    void UpdateWaveInfoOnUI();

protected:
    virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
    TSubclassOf<UGamePlayingUIWidget> GamePlayingWidgetClass;

private:
    UPROPERTY()
    UGamePlayingUIWidget* GamePlayingWidget;

    UPROPERTY()
    FTimerHandle TimerHandle_UpdateWaveInfo;

    UPROPERTY()
    FTimerHandle TimerHandle_FindWaveManager;

    void TryBindToWaveManager();

    bool bIsWaveManagerBound = false;

    float NextWaveTargetTimestamp = -1.f;

public:
    FORCEINLINE UGamePlayingUIWidget* GetGamePlayingWidget() const { return GamePlayingWidget; }
};
