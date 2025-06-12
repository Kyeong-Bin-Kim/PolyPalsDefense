#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PlayerSlotWidget.generated.h"

// Ready 버튼 클릭 시 위젯 자신을 넘기는 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyClicked, UPlayerSlotWidget*, ClickedSlot);

class APolyPalsPlayerState;
class UTextBlock;

UCLASS()
class POLYPALSDEFENSE_API UPlayerSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // Ready 버튼 클릭 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Event")
    FOnReadyClicked OnReadyClicked;

    void SetPlayerName(const FString& Name);

    UFUNCTION(BlueprintCallable)
    APolyPalsPlayerState* GetAssignedPlayerState() const { return AssignedPlayerState; }

    void SetAssignedPlayerState(APolyPalsPlayerState* InState) { AssignedPlayerState = InState; }

    UFUNCTION()
    void UpdateReadyVisual(bool bReady);

protected:
    UPROPERTY()
    APolyPalsPlayerState* AssignedPlayerState;

    // Ready 상태를 토글하는 내부 상태 값
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsReady = false;

    // Ready 버튼 기본 스타일 저장용
    FButtonStyle DefaultStyle;

    // 플레이어 이름 텍스트 위젯
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayerNameText;

    // Ready 버튼
    UPROPERTY(meta = (BindWidget))
    UButton* ReadyButton;

    // Ready 버튼 클릭 시 호출 함수
    UFUNCTION()
    void OnReadyButtonClicked();
};
