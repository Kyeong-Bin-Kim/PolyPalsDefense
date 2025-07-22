// SettingUIWidget.cpp
#include "UI/SettingUIWidget.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

void USettingUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (MasterVolumeSlider)
    {
        MasterVolumeSlider->OnValueChanged.AddDynamic(this, &USettingUIWidget::OnMasterVolumeChanged);
    }

    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &USettingUIWidget::OnCloseButtonClicked);
    }
}

void USettingUIWidget::OnMasterVolumeChanged(float Value)
{
    // ??됰뻻: Master SoundClass 癰귥눖??鈺곌퀣??
    if (USoundClass* MasterClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/Master.Master")))
    {
        MasterClass->Properties.Volume = Value;
    }
}

void USettingUIWidget::OnCloseButtonClicked()
{
    RemoveFromParent();
}
