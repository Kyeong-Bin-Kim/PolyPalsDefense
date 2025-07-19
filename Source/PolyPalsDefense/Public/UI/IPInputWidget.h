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
    // Ŭ�� ���ܿ�
    UPROPERTY(meta = (BindWidget))
    UBorder* ModalBlock;

    // IP �Է�â
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* IPTextBox;

    // ���� ��ư
    UPROPERTY(meta = (BindWidget))
    UButton* ConnectButton;

    // Connect Ŭ��
    UFUNCTION()
    void OnConnectClicked();
};
