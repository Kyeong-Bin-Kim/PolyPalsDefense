#include "PolyPalsState.h"
#include "Net/UnrealNetwork.h"

APolyPalsState::APolyPalsState()
{
    bIsGameOver = false;
}

void APolyPalsState::SetGameOver()
{
    // 서버 권한이 없거나 이미 게임 오버 상태면 처리 중단
    if (!HasAuthority() || bIsGameOver)
        return;

    // 게임 오버 플래그 설정 및 이벤트 브로드캐스트
    bIsGameOver = true;
    OnGameOver.Broadcast();
}

void APolyPalsState::OnRep_GameOver()
{
    // 복제된 클라이언트에서도 이벤트 브로드캐스트
    OnGameOver.Broadcast();
}

void APolyPalsState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(APolyPalsState, bIsGameOver);
}