#include "PolyPalsPlayerState.h"
#include "PolyPalsHUD.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void APolyPalsPlayerState::BeginPlay()
{
    Super::BeginPlay();

    if (IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0)))
    {
        // 클라이언트 본인일 경우
        OnRep_PlayerGold(); // 수동 호출로 HUD 갱신
    }
}


void APolyPalsPlayerState::SetReady(bool bReady)
{
    if (!HasAuthority())
    {
        bIsReady = bReady; // 서버에서 Ready 값 설정
    }
}

void APolyPalsPlayerState::AddGold(int32 Amount)
{
    //if (!HasAuthority())
    //{
    //    PlayerGold += Amount;


    //    OnRep_PlayerGold();
    //}
    PlayerGold += Amount;


    OnRep_PlayerGold();
}

void APolyPalsPlayerState::OnRep_PlayerGold()
{
    UE_LOG(LogTemp, Log, TEXT("[PlayerState] OnRep_PlayerGold: %d"), PlayerGold);

    if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
    {
        if (APolyPalsHUD* HUD = Cast<APolyPalsHUD>(PC->GetHUD()))
        {
            HUD->UpdateGoldOnUI(PlayerGold);
        }
    }
}

void APolyPalsPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APolyPalsPlayerState, bIsReady); // Ready 상태 복제
    DOREPLIFETIME(APolyPalsPlayerState, PlayerGold); // 개인 골드 복제
}

bool APolyPalsPlayerState::SpendGold(int32 Amount)
{
    if (!HasAuthority() || PlayerGold < Amount)
        return false;

    PlayerGold -= Amount;
    OnRep_PlayerGold();
    return true;
}