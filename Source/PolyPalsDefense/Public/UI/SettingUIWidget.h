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
    // ??됰뻻嚥?癰귥눖??鈺곌퀣??????????? ?ル굝利?甕곌쑵??
    UPROPERTY(meta = (BindWidget))
    class USlider* MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    class UButton* CloseButton;

    UFUNCTION()
    void OnMasterVolumeChanged(float Value);

    UFUNCTION()
    void OnCloseButtonClicked();
};