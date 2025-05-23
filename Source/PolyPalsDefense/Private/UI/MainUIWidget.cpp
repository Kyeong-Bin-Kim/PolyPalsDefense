// MainUIWidget.cpp

#include "UI/MainUIWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UMainUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ExitGame) ExitGame->OnClicked.AddDynamic(this, &UMainUIWidget::OnExitGameClicked);
    if (Settings) Settings->OnClicked.AddDynamic(this, &UMainUIWidget::OnSettingsClicked);
    if (StartGame) StartGame->OnClicked.AddDynamic(this, &UMainUIWidget::OnStartGameClicked);
    if (MakeRoom) MakeRoom->OnClicked.AddDynamic(this, &UMainUIWidget::OnMakeRoomClicked);
    if (FindRoom) FindRoom->OnClicked.AddDynamic(this, &UMainUIWidget::OnFindRoomClicked);
}

void UMainUIWidget::OnExitGameClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UMainUIWidget::OnSettingsClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        UUserWidget* SettingUI = CreateWidget(PC, LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/WBP_SettingUI.WBP_SettingUI_C")));
        if (SettingUI) SettingUI->AddToViewport();
    }
}

void UMainUIWidget::OnStartGameClicked()
{
    if (MultiplayerButtons) MultiplayerButtons->SetVisibility(ESlateVisibility::Visible);
}

void UMainUIWidget::OnMakeRoomClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        UUserWidget* StageSelectUI = CreateWidget(PC, LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/WBP_StageSelectUI.WBP_StageSelectUI_C")));
        if (StageSelectUI) StageSelectUI->AddToViewport();
    }
}

void UMainUIWidget::OnFindRoomClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        UUserWidget* LobbyListUI = CreateWidget(PC, LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/WBP_LobbyList.WBP_LobbyList_C")));
        if (LobbyListUI) LobbyListUI->AddToViewport();
    }
}
