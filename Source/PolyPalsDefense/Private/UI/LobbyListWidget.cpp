#include "LobbyListWidget.h"
#include "LobbySlotWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"

void ULobbyListWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 예시로 임시 방 ID로 초기화
    PopulateLobbyList({ TEXT("Room001"), TEXT("Room002"), TEXT("Room003") });
}

void ULobbyListWidget::PopulateLobbyList(const TArray<FString>& LobbyIDs)
{
    if (!LobbySlotClass || !LobbySlotContainer) return;

    LobbySlotContainer->ClearChildren();

    for (const FString& ID : LobbyIDs)
    {
        ULobbySlotWidget* LobbySlot = CreateWidget<ULobbySlotWidget>(GetWorld(), LobbySlotClass);
        if (LobbySlot)
        {
            LobbySlot->SetupSlot(ID, FString::Printf(TEXT("방 이름 %s"), *ID), FMath::RandRange(1, 4), 4);
            LobbySlot->OnJoinLobbyClicked.AddDynamic(this, &ULobbyListWidget::HandleJoinLobby);
            LobbySlotContainer->AddChildToVerticalBox(LobbySlot);
        }
    }
}

void ULobbyListWidget::HandleJoinLobby(const FString& LobbyID)
{
    UE_LOG(LogTemp, Log, TEXT("Joining Lobby: %s"), *LobbyID);

    // 입장 로직 구현 가능 (서버 연동 등)
}
