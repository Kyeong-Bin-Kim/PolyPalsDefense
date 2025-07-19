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

void APolyPalsState::SetGameOver()
{
    if (!HasAuthority() || bIsGameOver)
        return;

    bIsGameOver = true;

    OnGameOver.Broadcast();
}

void APolyPalsState::SetSelectedStage(FName Stage)
{
    if (HasAuthority())
    {
        SelectedStage = Stage;
        OnRep_SelectedStage();
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

void APolyPalsState::UpdateConnectedPlayers(int32 Count)
{
    if (HasAuthority())
    {
        ConnectedPlayers = Count;
        OnRep_ConnectedPlayers();
    }
}

void APolyPalsState::UpdateReadyPlayers()
{
    int32 ReadyCount = 0;

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

void APolyPalsState::OnRep_ConnectedPlayers()
{
    NotifyLobbyStateChanged();
}

void APolyPalsState::OnRep_ReadyPlayers()
{
    NotifyLobbyStateChanged();
}

void APolyPalsState::OnRep_SelectedStage()
{
    NotifyLobbyStateChanged();
}

void APolyPalsState::OnRep_LobbyName()
{
    NotifyLobbyStateChanged();
}

void APolyPalsState::OnRep_GameOver()
{
    OnGameOver.Broadcast();
}

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
