#include "UI/MainUIWidget.h"
#include "PolyPalsController.h"
#include "IPInputWidget.h"
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

    if (UWorld* World = GetWorld())
    {
        ENetMode Mode = World->GetNetMode();
        switch (Mode)
        {
        case NM_Standalone:
            UE_LOG(LogTemp, Log, TEXT("## NetMode: Standalone ##"));
            break;
        case NM_DedicatedServer:
            UE_LOG(LogTemp, Log, TEXT("## NetMode: DedicatedServer ##"));
            break;
        case NM_ListenServer:
            UE_LOG(LogTemp, Log, TEXT("## NetMode: ListenServer ##"));
            break;
        case NM_Client:
            UE_LOG(LogTemp, Log, TEXT("## NetMode: Client ##"));
            break;
        default:
            UE_LOG(LogTemp, Log, TEXT("## NetMode: Unknown(%d) ##"), (int32)Mode);
            break;
        }
    }

    if (ExitGame) ExitGame->OnClicked.AddDynamic(this, &UMainUIWidget::OnExitGameClicked);
    if (StartGame) StartGame->OnClicked.AddDynamic(this, &UMainUIWidget::OnStartGameClicked);
    if (CreateRoom) CreateRoom->OnClicked.AddDynamic(this, &UMainUIWidget::OnCreateRoomClicked);
    if (SearchRoom) SearchRoom->OnClicked.AddDynamic(this, &UMainUIWidget::OnSearchRoomClicked);

    if (MultiplayerButtons)
    {
        MultiplayerButtons->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (StartGame)
    {
        CachedStartButtonStyle = StartGame->GetStyle();
    }
}

void UMainUIWidget::OnExitGameClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
    }
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
            FButtonStyle PressedStyle = CachedStartButtonStyle;
            PressedStyle.Normal = PressedStyle.Pressed;
            PressedStyle.Hovered = PressedStyle.Pressed;
            StartGame->SetStyle(PressedStyle);
        }
        else
        {
            StartGame->SetStyle(CachedStartButtonStyle);
        }
    }
}

FReply UMainUIWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsMultiplayerMenuOpen)
    {
        if (!MultiplayerButtons->IsHovered())
        {
            bIsMultiplayerMenuOpen = false;

            if (MultiplayerButtons)
            {
                MultiplayerButtons->SetVisibility(ESlateVisibility::Collapsed);
            }

            if (StartGame)
            {
                StartGame->SetStyle(CachedStartButtonStyle);
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
    if (APolyPalsController* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->ShowStageSelectUI();
    }
}

void UMainUIWidget::OnSearchRoomClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (IPInputWidgetClass)
        {
            if (UIPInputWidget* IPWidget = CreateWidget<UIPInputWidget>(PC, IPInputWidgetClass))
            {
                const int32 OverlayZOrder = 100;
                IPWidget->AddToViewport(OverlayZOrder);
            }
        }
    }
}
