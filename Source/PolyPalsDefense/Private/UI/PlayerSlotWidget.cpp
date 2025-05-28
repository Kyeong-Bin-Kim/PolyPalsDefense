#include "PlayerSlotWidget.h"
#include "Components/Button.h"

void UPlayerSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Ready 버튼에 클릭 이벤트 바인딩
    if (ReadyButton)
    {
        ReadyButton->OnClicked.AddDynamic(this, &UPlayerSlotWidget::OnReadyButtonClicked);
    }
}

void UPlayerSlotWidget::OnReadyButtonClicked()
{
    // Ready 버튼 클릭 시 델리게이트 브로드캐스트
    OnReadyClicked.Broadcast(this);

    // 디버그 로그 출력
    UE_LOG(LogTemp, Log, TEXT("Ready button clicked on slot: %s"), *GetName());
}

void UPlayerSlotWidget::SetReadyButtonActive(bool bIsActive)
{
    if (ReadyButton)
    {
        // Ready 버튼 활성/비활성
        ReadyButton->SetIsEnabled(bIsActive);

        // 버튼 스타일 복사 후 색상 적용
        FButtonStyle Style = ReadyButton->GetStyle(); // Getter 사용

        if (!bIsActive)
        {
            Style.Normal.TintColor = FSlateColor(ReadyInactiveColor); // 비활성화 색상
        }
        else
        {
            Style.Normal.TintColor = FSlateColor(ReadyActiveColor); // 활성화 색상
        }

        // 스타일 적용
        ReadyButton->SetStyle(Style); // Setter 사용
    }
}

