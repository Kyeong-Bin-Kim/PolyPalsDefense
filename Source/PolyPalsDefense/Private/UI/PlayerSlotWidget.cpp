// PlayerSlotWidget.cpp

#include "UI/PlayerSlotWidget.h"

void UPlayerSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ReadyButton)
    {
        ReadyButton->OnClicked.AddDynamic(this, &UPlayerSlotWidget::OnReadyButtonClicked);
    }
}

void UPlayerSlotWidget::OnReadyButtonClicked()
{
    OnReadyClicked.Broadcast(this);
    UE_LOG(LogTemp, Log, TEXT("Ready button clicked on slot: %s"), *GetName());
}