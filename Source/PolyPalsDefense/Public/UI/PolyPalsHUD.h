#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PolyPalsHUD.generated.h"

class UGamePlayingUIWidget;

/**
 * 寃뚯엫 ?뚮젅??以??쒖떆?섎뒗 HUD ?대옒??
 * - 寃뚯엫 ?쒖옉 ??HUD ?꾩젽???붾㈃???쒖떆
 * - 寃뚯엫 ???섏튂 蹂寃????꾩젽???뺣낫 ?꾨떖
 */
UCLASS()
class POLYPALSDEFENSE_API APolyPalsHUD : public AHUD
{
    GENERATED_BODY()

public:
    // 怨⑤뱶媛 蹂寃쎈맆 ???몄텧
    UFUNCTION()
    void UpdateGoldOnUI(int32 NewGold);

    // ?⑥씠釉?愿???뺣낫 媛깆떊
    UFUNCTION()
    void UpdateWaveInfoOnUI();

protected:
    virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

    // ?꾩젽 ?대옒??(WBP_GamePlaying)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
    TSubclassOf<UGamePlayingUIWidget> GamePlayingWidgetClass;

private:
    // ?앹꽦???꾩젽 ?몄뒪?댁뒪
    UPROPERTY()
    UGamePlayingUIWidget* GamePlayingWidget;

    // ?뺢린 媛깆떊????대㉧
    UPROPERTY()
    FTimerHandle TimerHandle_UpdateWaveInfo;

    /** WaveManager媛 ?ㅽ룿???뚭퉴吏 二쇨린?곸쑝濡??먯깋 */
    UPROPERTY()
    FTimerHandle TimerHandle_FindWaveManager;

    /** WaveManager 諛붿씤???쒕룄 ?⑥닔 */
    void TryBindToWaveManager();

    bool bIsWaveManagerBound = false;

    /** ?ㅼ쓬 ?⑥씠釉??쒖옉 ??꾩뒪?ы봽 */
    float NextWaveTargetTimestamp = -1.f;

public:
    // ?몃??먯꽌 ?꾩젽 ?묎렐 ???ъ슜
    FORCEINLINE UGamePlayingUIWidget* GetGamePlayingWidget() const { return GamePlayingWidget; }
};
