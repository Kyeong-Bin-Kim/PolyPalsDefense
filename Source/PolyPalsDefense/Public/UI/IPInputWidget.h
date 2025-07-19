#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IPInputWidget.generated.h"

class UBorder;
class UEditableTextBox;
class UButton;

UCLASS()
class POLYPALSDEFENSE_API UIPInputWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
    // 클릭 차단용
    UPROPERTY(meta = (BindWidget))
    UBorder* ModalBlock;

    // IP 입력창
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* IPTextBox;

    // 접속 버튼
    UPROPERTY(meta = (BindWidget))
    UButton* ConnectButton;

    // Connect 클릭
    UFUNCTION()
    void OnConnectClicked();
};
