#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayingUIWidget.generated.h"

class AWaveManager;

UCLASS()
class POLYPALSDEFENSE_API UGamePlayingUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable)
    void SetRound(int32 Current, int32 Total);

    UFUNCTION(BlueprintCallable)
    void SetGold(int32 Gold);

    UFUNCTION(BlueprintCallable)
    void SetLife(int32 Life);

    UFUNCTION(BlueprintCallable)
    void SetEnemiesRemaining(int32 Count);

    UFUNCTION(BlueprintCallable)
    void SetNextWaveTime(float Seconds);

    UFUNCTION()
    void OnToggleTowerButtonClicked();

    UFUNCTION()
    void OnTower1ButtonClicked();

    UFUNCTION()
    void OnTower2ButtonClicked();

    UFUNCTION()
    void OnTower3ButtonClicked();

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RoundText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* GoldText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* LifeText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* EnemiesRemainingText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* NextWaveTimeText;

    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* TowerIconsBox;

    UPROPERTY(meta = (BindWidget))
    class UButton* ToggleTowerButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* Tower1Button;

    UPROPERTY(meta = (BindWidget))
    class UButton* Tower2Button;

    UPROPERTY(meta = (BindWidget))
    class UButton* Tower3Button;

    UPROPERTY()
    AWaveManager* WaveManagerRef;

    bool bIsTowerPanelVisible = false;
};
