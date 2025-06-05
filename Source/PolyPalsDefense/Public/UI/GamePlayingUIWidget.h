#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayingUIWidget.generated.h"

class UTextBlock;
class UHorizontalBox;
class UButton;

/**
 * 실제 게임플레이 중 UI를 담당하는 위젯 클래스
 * (생명, 골드, 라운드, 적 수, 웨이브 시간, 타워 버튼 패널 등)
 */
UCLASS()
class POLYPALSDEFENSE_API UGamePlayingUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    /** 라운드 UI 표시 */
    UFUNCTION(BlueprintCallable)
    void SetRound(int32 Current, int32 Total);

    /** 골드 UI 표시 */
    UFUNCTION(BlueprintCallable)
    void SetGold(int32 Gold);

    /** 생명 UI 표시 */
    UFUNCTION(BlueprintCallable)
    void SetLife(int32 Life);

    /** 남은 적 수 UI 표시 */
    UFUNCTION(BlueprintCallable)
    void SetEnemiesRemaining(int32 Current, int32 Total);

    /** 다음 웨이브 시간 UI 표시 */
    UFUNCTION(BlueprintCallable)
    void SetNextWaveTime(float Seconds);

    /** 타워 패널 열기/닫기 버튼 */
    UFUNCTION()
    void OnToggleTowerButtonClicked();

    /** 타워 버튼 클릭 처리 */
    UFUNCTION()
    void OnTower1ButtonClicked();

    UFUNCTION()
    void OnTower2ButtonClicked();

    UFUNCTION()
    void OnTower3ButtonClicked();

protected:
    /** 위젯 바인딩 요소들 */
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
