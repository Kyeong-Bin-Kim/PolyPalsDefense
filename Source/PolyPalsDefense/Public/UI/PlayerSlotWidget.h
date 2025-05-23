// PlayerSlotWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PlayerSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyClicked, UPlayerSlotWidget*, ClickedSlot);

UCLASS()
class POLYPALSDEFENSE_API UPlayerSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UPROPERTY(BlueprintAssignable, Category = "Event")
    FOnReadyClicked OnReadyClicked;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* ReadyButton;

    UFUNCTION()
    void OnReadyButtonClicked();
};