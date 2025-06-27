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

// 게임 오버 상태 서버에서 설정
void APolyPalsState::SetGameOver()
{
    if (!HasAuthority() || bIsGameOver)
        return;

    bIsGameOver = true;

    // 서버에서 이벤트 브로드캐스트 (서버 + 클라 동기화)
    OnGameOver.Broadcast();
}

// 선택된 스테이지 서버에서 설정
void APolyPalsState::SetSelectedStage(FName Stage)
{
    if (HasAuthority())
    {
        SelectedStage = Stage;
        OnRep_SelectedStage(); // 서버에서도 즉시 처리
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

// 서버에서 접속자 수 업데이트
void APolyPalsState::UpdateConnectedPlayers(int32 Count)
{
    if (HasAuthority())
    {
        ConnectedPlayers = Count;
        OnRep_ConnectedPlayers();
    }
}

// 서버에서 Ready 완료 수 계산 및 갱신
void APolyPalsState::UpdateReadyPlayers()
{
    int32 ReadyCount = 0;

    // PlayerArray 순회하여 Ready 상태 카운트
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

    // 모든 플레이어가 준비 완료된 경우, 이벤트 발생
    if (ReadyPlayers == ConnectedPlayers && ConnectedPlayers > 0)
    {
        OnAllPlayersReady.Broadcast();
    }
}

// 접속자 수 변경 시 클라에서 호출
void APolyPalsState::OnRep_ConnectedPlayers()
{
    UE_LOG(LogTemp, Log, TEXT("[GameState] 접속자 수 갱신: %d"), ConnectedPlayers);

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APolyPalsController* PPC = Cast<APolyPalsController>(It->Get()))
        {
            if (PPC->IsLocalController())
            {
                PPC->RefreshLobbyUI();
                UE_LOG(LogTemp, Log, TEXT("RefreshLobbyUI called for %s"), *PPC->GetName());
            }
        }
    }
}

// Ready 수 변경 시 클라에서 호출
void APolyPalsState::OnRep_ReadyPlayers()
{
    UE_LOG(LogTemp, Log, TEXT("[GameState] 준비 완료 수 갱신: %d"), ReadyPlayers);

    for (APlayerState* PS : PlayerArray)
    {
        if (APlayerController* PC = Cast<APlayerController>(PS->GetOwner()))
        {
            if (APolyPalsController* PPC = Cast<APolyPalsController>(PC))
            {
                PPC->RefreshLobbyUI();
            }
        }
    }
}

// 선택된 스테이지 변경 시 클라에서 호출
void APolyPalsState::OnRep_SelectedStage()
{
    UE_LOG(LogTemp, Log, TEXT("[GameState] 선택된 스테이지 갱신: %s"), *SelectedStage.ToString());

    for (APlayerState* PS : PlayerArray)
    {
        if (APlayerController* PC = Cast<APlayerController>(PS->GetOwner()))
        {
            if (APolyPalsController* PPC = Cast<APolyPalsController>(PC))
            {
                PPC->RefreshLobbyUI();
            }
        }
    }
}

void APolyPalsState::OnRep_LobbyName()
{
    UE_LOG(LogTemp, Log, TEXT("[GameState] 로비 이름 갱신: %s"), *LobbyName);

    for (APlayerState* PS : PlayerArray)
    {
        if (APlayerController* PC = Cast<APlayerController>(PS->GetOwner()))
        {
            if (APolyPalsController* PPC = Cast<APolyPalsController>(PC))
            {
                PPC->RefreshLobbyUI();
            }
        }
    }
}

// 게임 오버 상태 변경 시 클라에서 호출
void APolyPalsState::OnRep_GameOver()
{
    OnGameOver.Broadcast(); // 클라에서 이벤트 브로드캐스트
}

// Replication 변수 등록
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
