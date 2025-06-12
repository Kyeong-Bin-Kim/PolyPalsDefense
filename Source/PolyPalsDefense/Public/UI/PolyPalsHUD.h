#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PolyPalsHUD.generated.h"

class UGamePlayingUIWidget;

/**
 * 게임 플레이 중 표시되는 HUD 클래스
 * - 게임 시작 시 HUD 위젯을 화면에 표시
 * - 게임 내 수치 변경 시 위젯에 정보 전달
 */
UCLASS()
class POLYPALSDEFENSE_API APolyPalsHUD : public AHUD
{
    GENERATED_BODY()

public:
    // 골드가 변경될 때 호출
    UFUNCTION()
    void UpdateGoldOnUI(int32 NewGold);

    // 웨이브 관련 정보 갱신
    UFUNCTION()
    void UpdateWaveInfoOnUI();

protected:
    virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

    // 위젯 클래스 (WBP_GamePlaying)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
    TSubclassOf<UGamePlayingUIWidget> GamePlayingWidgetClass;

private:
    // 생성된 위젯 인스턴스
    UPROPERTY()
    UGamePlayingUIWidget* GamePlayingWidget;

    // 정기 갱신용 타이머
    UPROPERTY()
    FTimerHandle TimerHandle_UpdateWaveInfo;

    /** WaveManager가 스폰될 때까지 주기적으로 탐색 */
    UPROPERTY()
    FTimerHandle TimerHandle_FindWaveManager;

    /** WaveManager 바인딩 시도 함수 */
    void TryBindToWaveManager();

    bool bIsWaveManagerBound = false;

    /** 다음 웨이브 시작 타임스탬프 */
    float NextWaveTargetTimestamp = -1.f;

public:
    // 외부에서 위젯 접근 시 사용
    FORCEINLINE UGamePlayingUIWidget* GetGamePlayingWidget() const { return GamePlayingWidget; }
};
