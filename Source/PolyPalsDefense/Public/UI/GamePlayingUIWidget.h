#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayingUIWidget.generated.h"

class UTextBlock;
class UHorizontalBox;
class UButton;

/**
 * ?ㅼ젣 寃뚯엫?뚮젅??以?UI瑜??대떦?섎뒗 ?꾩젽 ?대옒??
 * (?앸챸, 怨⑤뱶, ?쇱슫?? ???? ?⑥씠釉??쒓컙, ???踰꾪듉 ?⑤꼸 ??
 */
UCLASS()
class POLYPALSDEFENSE_API UGamePlayingUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    /** ?쇱슫??UI ?쒖떆 */
    UFUNCTION(BlueprintCallable)
    void SetRound(int32 Current, int32 Total);

    /** 怨⑤뱶 UI ?쒖떆 */
    UFUNCTION(BlueprintCallable)
    void SetGold(int32 Gold);

    /** ?앸챸 UI ?쒖떆 */
    UFUNCTION(BlueprintCallable)
    void SetLife(int32 Life);

    /** ?⑥? ????UI ?쒖떆 */
    UFUNCTION(BlueprintCallable)
    void SetEnemiesRemaining(int32 Current, int32 Total);

    /** ?ㅼ쓬 ?⑥씠釉??쒓컙 UI ?쒖떆 */
    UFUNCTION(BlueprintCallable)
    void SetNextWaveTime(float Seconds);

    /** ????⑤꼸 ?닿린/?リ린 踰꾪듉 */
    UFUNCTION()
    void OnToggleTowerButtonClicked();

    /** ???踰꾪듉 ?대┃ 泥섎━ */
    UFUNCTION()
    void OnTower1ButtonClicked();

    UFUNCTION()
    void OnTower2ButtonClicked();

    UFUNCTION()
    void OnTower3ButtonClicked();

protected:
    /** ?꾩젽 諛붿씤???붿냼??*/
    UPROPERTY(meta = (BindWidget))
    UTextBlock* RoundText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GoldText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* LifeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EnemiesRemainingText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NextWaveTimeText;

    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* TowerIconsBox;

    UPROPERTY(meta = (BindWidget))
    UButton* ToggleTowerButton;

    UPROPERTY(meta = (BindWidget))
    UButton* Tower1Button;

    UPROPERTY(meta = (BindWidget))
    UButton* Tower2Button;

    UPROPERTY(meta = (BindWidget))
    UButton* Tower3Button;

private:
    bool bIsTowerPanelVisible = false;
};
