#include "PolyPalsPlayerState.h"
#include "Net/UnrealNetwork.h"

void APolyPalsPlayerState::SetReady(bool bReady)
{
    if (HasAuthority())
    {
        bIsReady = bReady; // 서버에서 Ready 값 설정
    }
}

void APolyPalsPlayerState::AddGold(int32 Amount)
{
    if (HasAuthority())
    {
        PlayerGold += Amount; // 서버에서 개인 골드 추가
    }
}

void APolyPalsPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APolyPalsPlayerState, bIsReady); // Ready 상태 복제
    DOREPLIFETIME(APolyPalsPlayerState, PlayerGold); // 개인 골드 복제
}
