#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayingUIWidget.generated.h"

class UTextBlock;
class UHorizontalBox;
class UButton;

/**
 * ���� �����÷��� �� UI�� ����ϴ� ���� Ŭ����
 * (����, ���, ����, �� ��, ���̺� �ð�, Ÿ�� ��ư �г� ��)
 */
UCLASS()
class POLYPALSDEFENSE_API UGamePlayingUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    /** ���� UI ǥ�� */
    UFUNCTION(BlueprintCallable)
    void SetRound(int32 Current, int32 Total);

    /** ��� UI ǥ�� */
    UFUNCTION(BlueprintCallable)
    void SetGold(int32 Gold);

    /** ���� UI ǥ�� */
    UFUNCTION(BlueprintCallable)
    void SetLife(int32 Life);

    /** ���� �� �� UI ǥ�� */
    UFUNCTION(BlueprintCallable)
    void SetEnemiesRemaining(int32 Current, int32 Total);

    /** ���� ���̺� �ð� UI ǥ�� */
    UFUNCTION(BlueprintCallable)
    void SetNextWaveTime(float Seconds);

    /** Ÿ�� �г� ����/�ݱ� ��ư */
    UFUNCTION()
    void OnToggleTowerButtonClicked();

    /** Ÿ�� ��ư Ŭ�� ó�� */
    UFUNCTION()
    void OnTower1ButtonClicked();

    UFUNCTION()
    void OnTower2ButtonClicked();

    UFUNCTION()
    void OnTower3ButtonClicked();

protected:
    /** ���� ���ε� ��ҵ� */
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
