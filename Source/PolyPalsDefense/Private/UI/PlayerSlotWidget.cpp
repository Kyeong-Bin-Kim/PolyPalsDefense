#include "PlayerSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UPlayerSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Ready 버튼에 클릭 이벤트 바인딩
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
            // 눌림 상태: 모든 이미지 눌림으로 고정
            Style.Normal = Style.Pressed;
            Style.Hovered = Style.Pressed;
        }
        else
        {
            // 초기 상태 복귀
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

