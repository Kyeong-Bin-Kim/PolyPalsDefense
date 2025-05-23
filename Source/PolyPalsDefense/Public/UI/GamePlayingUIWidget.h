// GamePlayingUIWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayingUIWidget.generated.h"

UCLASS()
class POLYPALSDEFENSE_API UGamePlayingUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

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
};
