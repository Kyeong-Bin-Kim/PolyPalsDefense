#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PlayerSlotWidget.generated.h"

// Ready 甕곌쑵?????????袁⑹졐 ?癒?뻿????띾┛???紐꺿봺野껊슣????醫롫섧
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyClicked, UPlayerSlotWidget*, ClickedSlot);

class APolyPalsPlayerState;
class UTextBlock;

UCLASS()
class POLYPALSDEFENSE_API UPlayerSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // Ready 甕곌쑵????????源??
    UPROPERTY(BlueprintAssignable, Category = "Event")
    FOnReadyClicked OnReadyClicked;

    void SetPlayerName(const FString& Name);

    UFUNCTION(BlueprintCallable)
    APolyPalsPlayerState* GetAssignedPlayerState() const { return AssignedPlayerState; }

    void SetAssignedPlayerState(APolyPalsPlayerState* InState) { AssignedPlayerState = InState; }

    UFUNCTION()
    void UpdateReadyVisual(bool bReady);

    /**
     * ???쟿??곷선 ?醫딅뼣 獄??????怨밴묶 揶쏄퉮??
     * @param InPlayerState ?醫딅뼣?????쟿??곷선 ?怨밴묶, nullptr??????????쑴?
     * @param bIsLocalPlayer ?????????嚥≪뮇類????쟿??곷선?紐? ???
     */
    void ConfigureSlot(APolyPalsPlayerState* InPlayerState, bool bIsLocalPlayer);

protected:
    UPROPERTY()
    APolyPalsPlayerState* AssignedPlayerState;

    // Ready ?怨밴묶???醫???롫뮉 ??? ?怨밴묶 揶?
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsReady = false;

    // Ready 甕곌쑵??疫꿸퀡??????????關??
    FButtonStyle DefaultStyle;

    // ???쟿??곷선 ??已???용뮞???袁⑹졐
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayerNameText;

    // Ready 甕곌쑵??
    UPROPERTY(meta = (BindWidget))
    UButton* ReadyButton;

    // Ready 甕곌쑵?????????紐꾪뀱 ??λ땾
    UFUNCTION()
    void OnReadyButtonClicked();
};
