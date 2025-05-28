#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PlayerSlotWidget.generated.h"

// Ready 버튼 클릭 시 위젯 자신을 넘기는 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyClicked, UPlayerSlotWidget*, ClickedSlot);

UCLASS()
class POLYPALSDEFENSE_API UPlayerSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // Ready 버튼 클릭 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Event")
    FOnReadyClicked OnReadyClicked;

    // Ready 버튼 활성/비활성 및 색상 설정
    UFUNCTION(BlueprintCallable)
    void SetReadyButtonActive(bool bIsActive);

protected:
    // Ready 버튼 (블루프린트에서 바인딩)
    UPROPERTY(meta = (BindWidget))
    UButton* ReadyButton;

    // Ready 버튼 클릭 시 호출 함수
    UFUNCTION()
    void OnReadyButtonClicked();

    // 활성화 색상 (디자이너가 블루프린트에서 설정 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FLinearColor ReadyActiveColor = FLinearColor(0.6f, 0.6f, 0.6f, 1.f);

    // 비활성화 색상 (디자이너가 블루프린트에서 설정 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FLinearColor ReadyInactiveColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.f);
};
