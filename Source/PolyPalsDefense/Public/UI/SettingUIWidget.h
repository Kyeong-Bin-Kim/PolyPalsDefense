// SettingUIWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingUIWidget.generated.h"

UCLASS()
class POLYPALSDEFENSE_API USettingUIWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    // ?덉떆濡?蹂쇰ⅷ 議곗젅???щ씪?대뜑? 醫낅즺 踰꾪듉
    UPROPERTY(meta = (BindWidget))
    class USlider* MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    class UButton* CloseButton;

    UFUNCTION()
    void OnMasterVolumeChanged(float Value);

    UFUNCTION()
    void OnCloseButtonClicked();
};