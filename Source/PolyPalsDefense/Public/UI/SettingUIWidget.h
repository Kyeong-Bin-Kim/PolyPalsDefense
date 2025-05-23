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
    // 예시로 볼륨 조절용 슬라이더와 종료 버튼
    UPROPERTY(meta = (BindWidget))
    class USlider* MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    class UButton* CloseButton;

    UFUNCTION()
    void OnMasterVolumeChanged(float Value);

    UFUNCTION()
    void OnCloseButtonClicked();
};