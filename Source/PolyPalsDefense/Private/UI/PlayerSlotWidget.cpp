#include "PlayerSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UPlayerSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Ready 踰꾪듉???대┃ ?대깽??諛붿씤??
    if (ReadyButton)
    {
        ReadyButton->OnClicked.AddDynamic(this, &UPlayerSlotWidget::OnReadyButtonClicked);
        DefaultStyle = ReadyButton->GetStyle();
    }
}

void UPlayerSlotWidget::OnReadyButtonClicked()
{
    bIsReady = !bIsReady;

    if (ReadyButton)
    {
        FButtonStyle Style = ReadyButton->GetStyle();

        if (bIsReady)
        {
            // ?뚮┝ ?곹깭: 紐⑤뱺 ?대?吏 ?뚮┝?쇰줈 怨좎젙
            Style.Normal = Style.Pressed;
            Style.Hovered = Style.Pressed;
        }
        else
        {
            // 珥덇린 ?곹깭 蹂듦?
            Style.Normal = DefaultStyle.Normal;
            Style.Hovered = DefaultStyle.Hovered;
        }

        ReadyButton->SetStyle(Style);
    }

    OnReadyClicked.Broadcast(this);
    UE_LOG(LogTemp, Log, TEXT("Ready toggled: %s (%s)"), *GetName(), bIsReady ? TEXT("Ready") : TEXT("Unready"));
}

void UPlayerSlotWidget::SetPlayerName(const FString& Name)
{
    if (PlayerNameText)
    {
        PlayerNameText->SetText(FText::FromString(Name));
    }
}

void UPlayerSlotWidget::UpdateReadyVisual(bool bReady)
{
    bIsReady = bReady;

    if (ReadyButton)
    {
        FButtonStyle ButtonStyle = ReadyButton->GetStyle();

        if (bReady)
        {
            ButtonStyle.Normal = ButtonStyle.Pressed;
            ButtonStyle.Hovered = ButtonStyle.Pressed;
        }
        else
        {
            ButtonStyle.Normal = DefaultStyle.Normal;
            ButtonStyle.Hovered = DefaultStyle.Hovered;
        }

        ReadyButton->SetStyle(ButtonStyle);
    }
}

void UPlayerSlotWidget::ConfigureSlot(APolyPalsPlayerState* InPlayerState, bool bIsLocalPlayer)
{
    AssignedPlayerState = InPlayerState;

    if (PlayerNameText)
    {
        if (InPlayerState)
        {
            PlayerNameText->SetVisibility(ESlateVisibility::Visible);
            PlayerNameText->SetText(FText::FromString(InPlayerState->GetPlayerName()));
        }
        else
        {
            PlayerNameText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    if (ReadyButton)
    {
        if (InPlayerState)
        {
            ReadyButton->SetVisibility(ESlateVisibility::Visible);
            ReadyButton->SetIsEnabled(bIsLocalPlayer);
        }
        else
        {
            ReadyButton->SetVisibility(ESlateVisibility::Collapsed);
            ReadyButton->SetIsEnabled(false);
        }
    }

    if (InPlayerState)
    {
        UpdateReadyVisual(InPlayerState->IsReady());
    }
    else
    {
        UpdateReadyVisual(false);
    }
}

