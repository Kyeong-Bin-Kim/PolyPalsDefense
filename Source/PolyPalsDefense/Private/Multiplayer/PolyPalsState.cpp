#include "PolyPalsState.h"
#include "PolyPalsPlayerState.h"
#include "PolyPalsController.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

APolyPalsState::APolyPalsState()
{
    ConnectedPlayers = 0;
    ReadyPlayers = 0;
    SelectedStage = NAME_None;
    LobbyName = TEXT("");
    bIsGameOver = false;
}

void APolyPalsState::SetCurrentRound(int32 NewRound)
{
    if (HasAuthority())
    {
        CurrentRound = NewRound;
        OnRep_CurrentRound();
    }
}


void APolyPalsState::OnRep_CurrentRound()
{
    UE_LOG(LogTemp, Log, TEXT("[GameState] CurrentRound updated: %d"), CurrentRound);
}

// 寃뚯엫 ?ㅻ쾭 ?곹깭 ?쒕쾭?먯꽌 ?ㅼ젙
void APolyPalsState::SetGameOver()
{
    if (!HasAuthority() || bIsGameOver)
        return;

    bIsGameOver = true;

    // ?쒕쾭?먯꽌 ?대깽??釉뚮줈?쒖틦?ㅽ듃 (?쒕쾭 + ?대씪 ?숆린??
    OnGameOver.Broadcast();
}

// ?좏깮???ㅽ뀒?댁? ?쒕쾭?먯꽌 ?ㅼ젙
void APolyPalsState::SetSelectedStage(FName Stage)
{
    if (HasAuthority())
    {
        SelectedStage = Stage;
        OnRep_SelectedStage(); // ?쒕쾭?먯꽌??利됱떆 泥섎━
    }
}

void APolyPalsState::SetLobbyName(const FString& Name)
{
    if (HasAuthority())
    {
        LobbyName = Name;
        OnRep_LobbyName();
    }
}

void APolyPalsState::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);
    NotifyLobbyStateChanged();
}

void APolyPalsState::RemovePlayerState(APlayerState* PlayerState)
{
    Super::RemovePlayerState(PlayerState);
    NotifyLobbyStateChanged();
}

// ?쒕쾭?먯꽌 ?묒냽?????낅뜲?댄듃
void APolyPalsState::UpdateConnectedPlayers(int32 Count)
{
    if (HasAuthority())
    {
        ConnectedPlayers = Count;
        OnRep_ConnectedPlayers();
    }
}

// ?쒕쾭?먯꽌 Ready ?꾨즺 ??怨꾩궛 諛?媛깆떊
void APolyPalsState::UpdateReadyPlayers()
{
    int32 ReadyCount = 0;

    // PlayerArray ?쒗쉶?섏뿬 Ready ?곹깭 移댁슫??
    for (APlayerState* PlayerState : PlayerArray)
    {
        APolyPalsPlayerState* MyState = Cast<APolyPalsPlayerState>(PlayerState);
        if (MyState && MyState->IsReady())
        {
            ReadyCount++;
        }
    }

    ReadyPlayers = ReadyCount;
    OnRep_ReadyPlayers();

    // 紐⑤뱺 ?뚮젅?댁뼱媛 以鍮??꾨즺??寃쎌슦, ?대깽??諛쒖깮
    if (ReadyPlayers == ConnectedPlayers && ConnectedPlayers > 0)
    {
        OnAllPlayersReady.Broadcast();
    }
}

void APolyPalsState::NotifyLobbyStateChanged()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        if (APolyPalsController* PPC = Cast<APolyPalsController>(*It))
        {
            if (PPC->IsLocalController())
            {
                PPC->ConfigureLobbyUI(SelectedStage, LobbyName);
                PPC->RefreshLobbyUI();
            }
        }
    }
}

// ?묒냽????蹂寃????대씪?먯꽌 ?몄텧
void APolyPalsState::OnRep_ConnectedPlayers()
{
    NotifyLobbyStateChanged();
}

// Ready ??蹂寃????대씪?먯꽌 ?몄텧
void APolyPalsState::OnRep_ReadyPlayers()
{
    NotifyLobbyStateChanged();
}

// ?좏깮???ㅽ뀒?댁? 蹂寃????대씪?먯꽌 ?몄텧
void APolyPalsState::OnRep_SelectedStage()
{
    NotifyLobbyStateChanged();
}

void APolyPalsState::OnRep_LobbyName()
{
    NotifyLobbyStateChanged();
}

// 寃뚯엫 ?ㅻ쾭 ?곹깭 蹂寃????대씪?먯꽌 ?몄텧
void APolyPalsState::OnRep_GameOver()
{
    OnGameOver.Broadcast(); // ?대씪?먯꽌 ?대깽??釉뚮줈?쒖틦?ㅽ듃
}

// Replication 蹂???깅줉
void APolyPalsState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APolyPalsState, ConnectedPlayers);
    DOREPLIFETIME(APolyPalsState, ReadyPlayers);
    DOREPLIFETIME(APolyPalsState, SelectedStage);
    DOREPLIFETIME(APolyPalsState, LobbyName);
    DOREPLIFETIME(APolyPalsState, CurrentRound);
    DOREPLIFETIME(APolyPalsState, bIsGameOver);
}
