#include "PolyPalsState.h"
#include "Net/UnrealNetwork.h"

APolyPalsState::APolyPalsState()
{
    bIsGameOver = false;
    Gold = 0;
}

void APolyPalsState::AddGold_Implementation(int32 Amount)
{
    Gold += Amount;
    OnRep_Gold(); // 서버에서도 즉시 UI 갱신 가능
}

void APolyPalsState::OnRep_Gold()
{
    UE_LOG(LogTemp, Log, TEXT("[PlayerState] 골드 갱신: %d"), Gold);

    // UI 위젯 연동(내부에서 직접 브로드캐스트[권장] OR 위젯에서 Tick or Timer로 GetGold() 호출)
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
    DOREPLIFETIME(APolyPalsState, Gold);
}