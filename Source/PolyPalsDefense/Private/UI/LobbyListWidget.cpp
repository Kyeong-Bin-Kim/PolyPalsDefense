#include "LobbyListWidget.h"
#include "LobbySlotWidget.h"
#include "PolyPalsController.h"
#include "MainUIWidget.h"
#include "PolyPalsGameInstance.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void ULobbyListWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Log, TEXT("LobbyListWidget constructed"));

    if (SearchButton)
    {
        SearchButton->OnClicked.AddDynamic(this, &ULobbyListWidget::HandleSearchClicked);
    }

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &ULobbyListWidget::OnExitGameClicked);
    }

    if (UPolyPalsGameInstance* GI = GetWorld()->GetGameInstance<UPolyPalsGameInstance>())
    {
        GI->OnSessionsFound.AddUObject(this, &ULobbyListWidget::PopulateLobbyList);
        
        StartRefreshing();
    }
}

void ULobbyListWidget::PopulateLobbyList(const TArray<FLobbyInfo>& LobbyInfos)
{
    CachedLobbies = LobbyInfos;
    ApplySearchFilter();
}

void ULobbyListWidget::ApplySearchFilter()
{
    if (!LobbySlotClass || !LobbySlotContainer)
        return;

    LobbySlotContainer->ClearChildren();

    FString Query;
    if (SearchBox)
    {
        Query = SearchBox->GetText().ToString().ToUpper().Replace(TEXT(" "), TEXT(""));
    }

    for (const FLobbyInfo& Info : CachedLobbies)
    {
        if (Info.CurrentPlayers >= Info.MaxPlayers || Info.bInProgress)
            continue;

        FString NameKey = Info.LobbyName.ToUpper().Replace(TEXT(" "), TEXT(""));
        if (!Query.IsEmpty() && !NameKey.Contains(Query))
            continue;

        ULobbySlotWidget* LobbySlot = CreateWidget<ULobbySlotWidget>(GetWorld(), LobbySlotClass);
        if (LobbySlot)
        {
            LobbySlot->SetupSlot(Info);
            LobbySlot->OnJoinLobbyClicked.AddDynamic(this, &ULobbyListWidget::HandleJoinLobby);

            if (UVerticalBoxSlot* BoxSlot = LobbySlotContainer->AddChildToVerticalBox(LobbySlot))
            {
                BoxSlot->SetVerticalAlignment(VAlign_Top);
            }
        }
    }
}

void ULobbyListWidget::HandleSearchClicked()
{
    ApplySearchFilter();
}

void ULobbyListWidget::OnExitGameClicked()
{
    if (APolyPalsController* PC = Cast<APolyPalsController>(GetOwningPlayer()))
    {
        PC->ShowMainUI();
    }
}

void ULobbyListWidget::HandleJoinLobby(const FString& LobbyID)
{
    if (UPolyPalsGameInstance* GI = GetWorld()->GetGameInstance<UPolyPalsGameInstance>())
    {
        UE_LOG(LogTemp, Log, TEXT("HandleJoinLobby called with ID %s"), *LobbyID);
        GI->JoinSteamSession(LobbyID);
    }
}

void ULobbyListWidget::NativeDestruct()
{
    if (UPolyPalsGameInstance* GI = GetWorld()->GetGameInstance<UPolyPalsGameInstance>())
    {
        GI->OnSessionsFound.RemoveAll(this);
    }

    StopRefreshing();

    Super::NativeDestruct();
}

void ULobbyListWidget::RefreshSessions()
{
    if (UPolyPalsGameInstance* GI = GetWorld()->GetGameInstance<UPolyPalsGameInstance>())
    {
        GI->FindSteamSessions();
    }
}

void ULobbyListWidget::SetVisibility(ESlateVisibility InVisibility)
{
    Super::SetVisibility(InVisibility);

    // 숨김/표시 상태에 따라 새로고침 시작/중지
    if (InVisibility == ESlateVisibility::Visible)
    {
        StartRefreshing();
    }
    else if (InVisibility == ESlateVisibility::Hidden)
    {
        StopRefreshing();
    }
}

void ULobbyListWidget::StartRefreshing()
{
    if (UPolyPalsGameInstance* GI = GetWorld()->GetGameInstance<UPolyPalsGameInstance>())
    {
        GI->FindSteamSessions();

        if (!GetWorld()->GetTimerManager().IsTimerActive(RefreshTimerHandle))
        {
            FTimerDelegate RefreshDelegate = FTimerDelegate::CreateUObject(this, &ULobbyListWidget::RefreshSessions);
            GetWorld()->GetTimerManager().SetTimer(RefreshTimerHandle, RefreshDelegate, 5.0f, true);
        }
    }
}

void ULobbyListWidget::StopRefreshing()
{
    GetWorld()->GetTimerManager().ClearTimer(RefreshTimerHandle);
}
