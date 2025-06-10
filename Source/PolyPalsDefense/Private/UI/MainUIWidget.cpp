#include "UI/MainUIWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "StageSelectUIWidget.h"
#include "LobbyUIWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UMainUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ExitGame) ExitGame->OnClicked.AddDynamic(this, &UMainUIWidget::OnExitGameClicked);
    //if (Settings) Settings->OnClicked.AddDynamic(this, &UMainUIWidget::OnSettingsClicked); ���� �߰� ����
    if (StartGame) StartGame->OnClicked.AddDynamic(this, &UMainUIWidget::OnStartGameClicked);
    if (CreateRoom) CreateRoom->OnClicked.AddDynamic(this, &UMainUIWidget::OnCreateRoomClicked);
    if (SearchRoom) SearchRoom->OnClicked.AddDynamic(this, &UMainUIWidget::OnSearchRoomClicked);

    if (MultiplayerButtons)
    {
        MultiplayerButtons->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (StartGame)
    {
        // ���� ��Ÿ�� ���
        CachedStartButtonStyle = StartGame->GetStyle();
    }
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
    bIsMultiplayerMenuOpen = !bIsMultiplayerMenuOpen;

    if (MultiplayerButtons)
    {
        MultiplayerButtons->SetVisibility(
            bIsMultiplayerMenuOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
        );
    }

    if (StartGame)
    {
        if (bIsMultiplayerMenuOpen)
        {
            // ���� ����ó�� ���̰�: Pressed ��Ÿ���� Normal/Hovered�� ����
            FButtonStyle PressedStyle = CachedStartButtonStyle;
            PressedStyle.Normal = PressedStyle.Pressed;
            PressedStyle.Hovered = PressedStyle.Pressed;
            StartGame->SetStyle(PressedStyle);
        }
        else
        {
            StartGame->SetStyle(CachedStartButtonStyle); // �����ص� ���� ��Ÿ�Ϸ� ����
        }
    }
}

FReply UMainUIWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsMultiplayerMenuOpen)
    {
        // MultiplayerButtons �ܺ� Ŭ�� �� ���� �ʱ�ȭ
        if (!MultiplayerButtons->IsHovered())
        {
            bIsMultiplayerMenuOpen = false;

            if (MultiplayerButtons)
            {
                MultiplayerButtons->SetVisibility(ESlateVisibility::Collapsed);
            }

            if (StartGame)
            {
                StartGame->SetStyle(CachedStartButtonStyle); // �����ص� ���� ��Ÿ�Ϸ� ����
            }
        }
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UMainUIWidget::SetPlayerNameText(const FString& Name)
{
    if (PlayerNameText)
    {
        PlayerNameText->SetText(FText::FromString(Name));
    }
}

void UMainUIWidget::OnCreateRoomClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (StageSelectWidgetClass)
        {
            // ����
            UStageSelectUIWidget* StageUIWidget = CreateWidget<UStageSelectUIWidget>(GetWorld(), StageSelectWidgetClass);

            if (StageUIWidget)
            {
                // Viewport �߰�
                StageUIWidget->AddToViewport();

                // ���� Main UI�� ����
                this->RemoveFromParent();
            }
        }
    }
}

void UMainUIWidget::HandleStageSelected(FName StageName)
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (LobbyUIWidgetClass)
        {
            ULobbyUIWidget* LobbyWidget = CreateWidget<ULobbyUIWidget>(PC, LobbyUIWidgetClass);

            if (LobbyWidget)
            {
                LobbyWidget->SetSelectedStage(StageName);
                LobbyWidget->AddToViewport();
            }
        }
    }
}

void UMainUIWidget::OnSearchRoomClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        UUserWidget* LobbyListUI = CreateWidget(PC, LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/WBP_LobbyList.WBP_LobbyList_C")));
        if (LobbyListUI) LobbyListUI->AddToViewport();
    }
}
