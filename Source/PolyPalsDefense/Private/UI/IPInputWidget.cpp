#include "IPInputWidget.h"
#include "Components/Border.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "PolyPalsController.h"
#include "Kismet/GameplayStatics.h"

void UIPInputWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ConnectButton)
        ConnectButton->OnClicked.AddDynamic(this, &UIPInputWidget::OnConnectClicked);
}

FReply UIPInputWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    RemoveFromParent();

    return FReply::Handled();
}

void UIPInputWidget::OnConnectClicked()
{
    if (!IPTextBox) return;

    FString Address = IPTextBox->GetText().ToString();

    if (Address.IsEmpty()) return;

    if (APolyPalsController* PC = Cast<APolyPalsController>(UGameplayStatics::GetPlayerController(this, 0)))
    {
        PC->ClientTravel(*Address, ETravelType::TRAVEL_Absolute);
    }

    // 창 닫기
    RemoveFromParent();
}
