#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PlayerSlotWidget.generated.h"

// Ready 踰꾪듉 ?대┃ ???꾩젽 ?먯떊???섍린???몃━寃뚯씠???좎뼵
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyClicked, UPlayerSlotWidget*, ClickedSlot);

class APolyPalsPlayerState;
class UTextBlock;

UCLASS()
class POLYPALSDEFENSE_API UPlayerSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // Ready 踰꾪듉 ?대┃ ?대깽??
    UPROPERTY(BlueprintAssignable, Category = "Event")
    FOnReadyClicked OnReadyClicked;

    void SetPlayerName(const FString& Name);

    UFUNCTION(BlueprintCallable)
    APolyPalsPlayerState* GetAssignedPlayerState() const { return AssignedPlayerState; }

    void SetAssignedPlayerState(APolyPalsPlayerState* InState) { AssignedPlayerState = InState; }

    UFUNCTION()
    void UpdateReadyVisual(bool bReady);

    /**
     * ?뚮젅?댁뼱 ?좊떦 諛??щ’ ?곹깭 媛깆떊
     * @param InPlayerState ?좊떦???뚮젅?댁뼱 ?곹깭, nullptr?대㈃ ?щ’ 鍮꾩?
     * @param bIsLocalPlayer ?대떦 ?щ’??濡쒖뺄 ?뚮젅?댁뼱?몄? ?щ?
     */
    void ConfigureSlot(APolyPalsPlayerState* InPlayerState, bool bIsLocalPlayer);

protected:
    UPROPERTY()
    APolyPalsPlayerState* AssignedPlayerState;

    // Ready ?곹깭瑜??좉??섎뒗 ?대? ?곹깭 媛?
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsReady = false;

    // Ready 踰꾪듉 湲곕낯 ?ㅽ?????μ슜
    FButtonStyle DefaultStyle;

    // ?뚮젅?댁뼱 ?대쫫 ?띿뒪???꾩젽
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayerNameText;

    // Ready 踰꾪듉
    UPROPERTY(meta = (BindWidget))
    UButton* ReadyButton;

    // Ready 踰꾪듉 ?대┃ ???몄텧 ?⑥닔
    UFUNCTION()
    void OnReadyButtonClicked();
};
