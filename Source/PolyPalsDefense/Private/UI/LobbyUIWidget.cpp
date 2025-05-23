// LobbyUIWidget.cpp
#include "UI/LobbyUIWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void ULobbyUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ExitGame)
    {
        ExitGame->OnClicked.AddDynamic(this, &ULobbyUIWidget::OnExitGameClicked);
    }

    if (StartGame)
    {
        StartGame->OnClicked.AddDynamic(this, &ULobbyUIWidget::OnStartGameClicked);
    }

    // PlayerSlotBox의 Children을 순회하며 바인딩
    if (PlayerSlotBox)
    {
        for (UWidget* Child : PlayerSlotBox->GetAllChildren())
        {
            if (UPlayerSlotWidget* PlayerSlot = Cast<UPlayerSlotWidget>(Child))
            {
                PlayerSlotWidgets.Add(PlayerSlot);
                PlayerSlot->OnReadyClicked.AddDynamic(this, &ULobbyUIWidget::HandleSlotReadyClicked);
            }
        }

    }
}

void ULobbyUIWidget::OnExitGameClicked()
{
    RemoveFromParent();
}

void ULobbyUIWidget::OnStartGameClicked()
{
    RemoveFromParent();

    UWorld* World = GetWorld();
    if (World)
    {
        UGameplayStatics::OpenLevel(World, FName("TestMap02"), true);
    }
}

void ULobbyUIWidget::HandleSlotReadyClicked(UPlayerSlotWidget* ClickedSlot)
{
    UE_LOG(LogTemp, Log, TEXT("Player slot ready clicked: %s"), *ClickedSlot->GetName());
    // 여기서 Ready 상태 업데이트나 서버 알림 등을 처리 가능
}
